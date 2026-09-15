# BookShelf —— Qt Model/View 找 Bug 练习

一个"图书管理"小程序，用来练习 Qt 的 Model/View 架构。

**玩法：** 代码里故意埋了 Model/View 中经典、容易写错的 bug。每一关只埋 **一个** bug，
运行程序 → 观察异常现象 → 读代码定位原因 → 修复。修好一关后提交，再进入下一关，
下一关会在程序里加入新功能（以及新的 bug）。

## 环境

- Qt 6（开发时使用 Qt 6.9.0 MinGW 64-bit）
- CMake ≥ 3.16

## 构建运行

用 Qt Creator 直接打开 `CMakeLists.txt` 即可。或者命令行：

```bash
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=C:/Qt/6.9.0/mingw_64
cmake --build build
./build/BookShelf.exe
```

## 常见问题

- **年份显示成「〇〈〉」之类的怪字符**：这不是关卡 bug，而是 Windows「区域 → 其他设置 → 本地数字」
  被设置成了中文数字。委托（`QStyledItemDelegate`）会按区域格式化数字，Qt 假设数字字符是连续编码的，
  中文数字不满足，于是显示错乱。`main.cpp` 里已用 `QLocale::setDefault()` 固定为中文(中国)格式规避。

## 项目结构

```
src/
├── main.cpp
├── mainwindow.h / .cpp   # 主窗口：视图、按钮
└── bookmodel.h / .cpp    # 自定义模型 BookModel（QAbstractTableModel）
```

## 关卡进度

| 关卡 | 新增功能 | 你要找的现象（提示） | 状态 |
|------|----------|----------------------|------|
| 1 | 表格显示图书、添加图书 | 点"添加一本书"试试，对比表格和状态栏 | ✅ 已解决 |
| 2 | 编辑图书、右侧书名列表（共享同一模型） | 在表格里改书名，看看右边的列表（先别动鼠标） | ✅ 已解决 |
| 3 | 删除选中的书（支持多选） | 删一本没问题；多加几本书，用 Ctrl 选中**多行**再删，核对删掉的是不是你选的 | ✅ 已解决 |
| 4 | 点表头排序、搜索过滤（`QSortFilterProxyModel`） | 不排序时删除正常；**点「年份」表头排序**或**搜索**之后再删一本，看删掉的是哪本 | 🐛 待解决 |

> 每关的原因分析和修复方法会在解决后补充到下面的「通关记录」中，
> 也可以在 git 历史里查看每一关的 bug 提交和修复提交。

## 通关记录

### 第 1 关：插入数据没有通知视图

**现象：** 点"添加一本书"，状态栏的数量在增加，表格却一直只有最初那 3 行。

**原因：** `addBook()` 只往 `m_books` 里追加了数据，没有告诉视图"行数变了"。
视图不会主动轮询 `rowCount()`，它只在收到模型信号（`rowsInserted` 等）时才更新自己缓存的行数
（`QHeaderView` 里的 section 数量）。数据变了、信号没发，视图就不知道。

**修复：** 修改数据前后用 `beginInsertRows()` / `endInsertRows()` 包起来：

```cpp
const int row = m_books.size();
beginInsertRows(QModelIndex(), row, row);   // 先声明：将在第 row 行插入 1 行
m_books.append(book);                       // 再改数据
endInsertRows();                            // 最后通知：插入完成（发出 rowsInserted）
```

**要点：**
- `begin` 必须在改数据**之前**调用：视图和代理模型需要在变化前做准备（例如更新持久索引 `QPersistentModelIndex`）。
- `first` / `last` 都是**闭区间**，插入 1 行时两者相等。
- 同类函数：`beginRemoveRows`、`beginInsertColumns`、`beginMoveRows`、`beginResetModel`，规则相同。
- 不要自己 `emit rowsInserted(...)`，这个信号是私有的，只能由 `endInsertRows()` 发出。

### 第 2 关：setData 修改数据后没有发 dataChanged

**现象：** 在表格里改书名并回车，表格里是新名字，右边「书名一览」还是旧名字；
鼠标移到列表上（触发重绘）后才变。

**原因：** `setData()` 改了 `m_books`，但没有 `emit dataChanged(...)`。
- 表格能"看起来正常"是个假象：编辑器关闭时，**发起编辑的那个视图**会顺手重绘这个单元格，
  重绘时重新调用 `data()`，自然读到了新值。
- 右边的 `QListView` 没参与编辑，它只能靠模型信号知道数据变了。没有信号就不会重绘，
  直到鼠标悬停等原因偶然触发重绘，才"碰巧"显示新值。

所以**只用一个视图测试时，这个 bug 很容易被漏掉**。

**修复：**

```cpp
emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
return true;
```

**要点：**
- Qt 文档明确要求：`setData()` 成功修改数据后必须发出 `dataChanged`，并返回 `true`。
- 前两个参数是变化区域的左上角和右下角（闭区间）；批量修改时发一次大范围的信号，比逐个单元格发更高效。
- 第三个参数 `roles` 可选，写明哪些角色变了，视图和代理模型可以据此少做工作；不写表示所有角色都可能变了。
- 自定义信号（如 `bookCountChanged`）视图是不认识的，视图只监听 `QAbstractItemModel` 的标准信号。

### 第 3 关：循环删除多行时 QModelIndex 失效

**现象：** 只删一本没问题；选中多行一起删，删掉的书和选中的对不上，选中末尾几行时有的删不掉。

**原因：** `selectedRows()` 在循环前一次性拿到所有索引，然后一本一本删。
删掉前面的一行后，后面所有行都往上挪了一位，但列表里那些 `QModelIndex` 记录的还是旧行号。

以 5 本书 A B C D E、选中 A C E（行 0、2、4）为例：

| 步骤 | 删除行号 | 实际删掉 | 剩下 |
|------|----------|----------|------|
| 1 | 0 | A ✅ | B C D E |
| 2 | 2 | **D** ❌（C 已经挪到第 1 行） | B C E |
| 3 | 4 | 越界，`removeRows` 返回 false | B C E |

**`QModelIndex` 是"一次性"的**：模型结构一旦变化（增删行、排序等），之前拿到的 `QModelIndex` 就不可靠了，不要保存它。

**修复（本项目采用）：** 先取出行号，**从大到小**删，删下面的行不影响上面的行号。
注意 `selectedRows()` 返回的顺序是**用户选择的顺序**，不一定有序，必须自己排序。

```cpp
QList<int> rows;
for (const QModelIndex &index : selected)
    rows.append(index.row());
std::sort(rows.begin(), rows.end(), std::greater<int>());
for (int row : rows)
    m_model->removeRows(row, 1);
```

**另一种修法：** 用 `QPersistentModelIndex`。它会在模型发出 `rowsRemoved` 等信号时自动更新行号：

```cpp
QList<QPersistentModelIndex> persistent;
for (const QModelIndex &index : selected)
    persistent.append(index);
for (const QPersistentModelIndex &index : persistent)
    m_model->removeRows(index.row(), 1);
```

它能自动更新的前提，正是模型正确调用了 `beginRemoveRows()` / `endRemoveRows()`（第 1 关）。
