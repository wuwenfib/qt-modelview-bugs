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
| 2 | 编辑图书、右侧书名列表（共享同一模型） | 在表格里改书名，看看右边的列表（先别动鼠标） | 🐛 待解决 |

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
