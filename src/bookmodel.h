#pragma once

#include <QAbstractTableModel>
#include <QList>
#include <QString>

struct Book
{
    QString title;
    QString author;
    int year = 0;
    bool read = false;
};

class BookModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        TitleColumn,
        AuthorColumn,
        YearColumn,
        ReadColumn,
        ColumnCount
    };

    explicit BookModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void addBook(const Book &book);

signals:
    void bookCountChanged(int count);

private:
    QList<Book> m_books;
};
