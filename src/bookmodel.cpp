#include "bookmodel.h"

BookModel::BookModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_books = {
        {QStringLiteral("三体"), QStringLiteral("刘慈欣"), 2008, true},
        {QStringLiteral("活着"), QStringLiteral("余华"), 1993, true},
        {QStringLiteral("围城"), QStringLiteral("钱钟书"), 1947, false},
    };
}

int BookModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_books.size();
}

int BookModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return ColumnCount;
}

QVariant BookModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_books.size())
        return {};

    const Book &book = m_books.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case TitleColumn:  return book.title;
        case AuthorColumn: return book.author;
        case YearColumn:   return book.year;
        case ReadColumn:   return book.read ? QStringLiteral("是") : QStringLiteral("否");
        }
    }

    return {};
}

QVariant BookModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Vertical)
        return section + 1;

    switch (section) {
    case TitleColumn:  return QStringLiteral("书名");
    case AuthorColumn: return QStringLiteral("作者");
    case YearColumn:   return QStringLiteral("年份");
    case ReadColumn:   return QStringLiteral("已读");
    }
    return {};
}

void BookModel::addBook(const Book &book)
{
    m_books.append(book);
    emit bookCountChanged(m_books.size());
}
