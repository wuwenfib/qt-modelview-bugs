#pragma once

#include <QMainWindow>

class QLabel;
class QTableView;
class BookModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void addSampleBook();
    void updateStatus(int count);

private:
    BookModel *m_model = nullptr;
    QTableView *m_tableView = nullptr;
    QLabel *m_statusLabel = nullptr;
    int m_nextSample = 0;
};
