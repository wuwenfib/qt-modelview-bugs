#pragma once

#include <QMainWindow>

class QLabel;
class QListView;
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
    QTableView *m_tableView = nullptr;
    QListView *m_titleListView = nullptr;
    QLabel *m_statusLabel = nullptr;
    int m_nextSample = 0;
};
