#include "mainwindow.h"
#include "bookmodel.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QStatusBar>
#include <QTableView>
#include <QVBoxLayout>

#include <algorithm>
#include <functional>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("BookShelf - Qt Model/View 找 Bug"));

    m_model = new BookModel(this);

    // 代理模型：给表格提供排序和搜索过滤
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterKeyColumn(-1); // 在所有列里搜索
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    m_tableView = new QTableView;
    m_tableView->setModel(m_proxyModel);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
    m_tableView->setSortingEnabled(true);

    // 第二个视图：直接使用源模型，显示书在模型里的原始顺序
    m_titleListView = new QListView;
    m_titleListView->setModel(m_model);
    m_titleListView->setModelColumn(BookModel::TitleColumn);
    m_titleListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto *listPanel = new QWidget;
    auto *listLayout = new QVBoxLayout(listPanel);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->addWidget(new QLabel(QStringLiteral("书名一览（源模型，原始顺序）")));
    listLayout->addWidget(m_titleListView);

    auto *splitter = new QSplitter;
    splitter->addWidget(m_tableView);
    splitter->addWidget(listPanel);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);

    auto *addButton = new QPushButton(QStringLiteral("添加一本书"));
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addSampleBook);

    auto *removeButton = new QPushButton(QStringLiteral("删除选中的书"));
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::removeSelectedBooks);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addStretch();

    auto *searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText(QStringLiteral("搜索书名、作者、年份…"));
    searchEdit->setClearButtonEnabled(true);
    connect(searchEdit, &QLineEdit::textChanged,
            m_proxyModel, &QSortFilterProxyModel::setFilterFixedString);
    buttonLayout->addWidget(searchEdit);

    auto *central = new QWidget;
    auto *layout = new QVBoxLayout(central);
    layout->addLayout(buttonLayout);
    layout->addWidget(new QLabel(
        QStringLiteral("双击单元格编辑；Ctrl/Shift 多选；点击表头排序")));
    layout->addWidget(splitter, 1);
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

void MainWindow::removeSelectedBooks()
{
    const QModelIndexList selected = m_tableView->selectionModel()->selectedRows();

    QList<int> rows;
    for (const QModelIndex &index : selected)
        rows.append(index.row());

    // 从下往上删：删掉下面的行不会影响上面行的行号
    std::sort(rows.begin(), rows.end(), std::greater<int>());

    for (int row : rows)
        m_model->removeRows(row, 1);
}

void MainWindow::updateStatus(int count)
{
    m_statusLabel->setText(QStringLiteral("模型中共有 %1 本书").arg(count));
}
