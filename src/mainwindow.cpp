#include "mainwindow.h"
#include "bookmodel.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QTableView>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("BookShelf - Qt Model/View 找 Bug"));

    m_model = new BookModel(this);

    m_tableView = new QTableView;
    m_tableView->setModel(m_model);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->horizontalHeader()->setStretchLastSection(true);

    auto *addButton = new QPushButton(QStringLiteral("添加一本书"));
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addSampleBook);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addButton);
    buttonLayout->addStretch();

    auto *central = new QWidget;
    auto *layout = new QVBoxLayout(central);
    layout->addLayout(buttonLayout);
    layout->addWidget(m_tableView);
    setCentralWidget(central);

    m_statusLabel = new QLabel;
    statusBar()->addWidget(m_statusLabel);
    connect(m_model, &BookModel::bookCountChanged, this, &MainWindow::updateStatus);
    updateStatus(m_model->rowCount());
}

void MainWindow::addSampleBook()
{
    static const QList<Book> samples = {
        {QStringLiteral("红楼梦"), QStringLiteral("曹雪芹"), 1791, false},
        {QStringLiteral("百年孤独"), QStringLiteral("马尔克斯"), 1967, true},
        {QStringLiteral("平凡的世界"), QStringLiteral("路遥"), 1986, false},
        {QStringLiteral("1984"), QStringLiteral("乔治·奥威尔"), 1949, true},
    };

    m_model->addBook(samples.at(m_nextSample));
    m_nextSample = (m_nextSample + 1) % samples.size();
}

void MainWindow::updateStatus(int count)
{
    m_statusLabel->setText(QStringLiteral("模型中共有 %1 本书").arg(count));
}
