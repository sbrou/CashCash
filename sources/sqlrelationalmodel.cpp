#include "sqlrelationalmodel.h"
#include "defines.h"

#include <QBrush>
#include <QPainter>

#include "defines.h"

SqlRelationalTableModel::SqlRelationalTableModel(QObject *parent, const QSqlDatabase &db)
    : QSqlRelationalTableModel{parent,db}
{}

QVariant SqlRelationalTableModel::data(const QModelIndex &index, int role) const
{
    int column = index.column();

    if (role == Qt::TextAlignmentRole)
    {
        if (column == CatIndex || column == TagIndex)
            return Qt::AlignCenter;

        if (column == AmountIndex)
            return QVariant::fromValue(Qt::AlignRight | Qt::AlignVCenter);
    }


    if (role == Qt::ForegroundRole && (column == CatIndex || column == TagIndex))
        return index.data() == DEFAULT_GROUP_NAME ? QBrush(Qt::red) : QBrush(Qt::black);

    if (role == Qt::DisplayRole && column == AmountIndex)
        return QString::number(QSqlRelationalTableModel::data(index, Qt::DisplayRole).toFloat()) + " €";

    return QSqlRelationalTableModel::data(index, role);
}
