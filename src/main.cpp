#include "mainwindow.h"

#include <QApplication>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 不跟随 Windows 区域设置里的"本地数字"（例如设成"〇一二三…"时，
    // 委托格式化数字会显示成怪字符），统一使用中文(中国)的阿拉伯数字格式。
    // 必须在创建任何控件之前设置，控件的 locale() 默认取这里的值。
    QLocale::setDefault(QLocale(QLocale::Chinese, QLocale::China));

    MainWindow window;
    window.resize(900, 480);
    window.show();

    return app.exec();
}
