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
| 1 | 表格显示图书、添加图书 | 点"添加一本书"试试，对比表格和状态栏 | 🐛 待解决 |

> 每关的原因分析和修复方法会在解决后补充到下面的「通关记录」中，
> 也可以在 git 历史里查看每一关的 bug 提交和修复提交。

## 通关记录

（暂无）
