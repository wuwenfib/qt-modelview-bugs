#pragma once

#include <QMainWindow>

class QLabel;
class QListView;
class QSortFilterProxyModel;
class QTableView;
class BookModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void addSampleBook();
    void removeSelectedBooks();
    void updateStatus(int count);

private:
    BookModel *m_model = nullptr;
    QSortFilterProxyModel *m_proxyModel = nullptr;
    QTableView *m_tableView = nullptr;
    QListView *m_titleListView = nullptr;
    QLabel *m_statusLabel = nullptr;
    int m_nextSample = 0;
};
