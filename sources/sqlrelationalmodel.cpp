#include "sqlrelationalmodel.h"
#include "defines.h"

#include <QBrush>
#include <QPainter>

#include "defines.h"

SqlRelationalTableModel::SqlRelationalTableModel(QObject *parent, const QSqlDatabase &db)
    : QSqlRelationalTableModel{parent,db}
{}

QVariant SqlRelationalTableModel::data(const QModelIndex &idx, int role) const
{
    int column = idx.column();

    if (role == Qt::TextAlignmentRole)
    {
        if (column == CatIndex || column == TagIndex)
            return Qt::AlignCenter;

        if (column == AmountIndex)
            return QVariant::fromValue(Qt::AlignRight | Qt::AlignVCenter);
    }


    if (role == Qt::ForegroundRole)
    {
        if ((column == CatIndex || column == TagIndex) && idx.data() == DEFAULT_GROUP_NAME)
            return QBrush(Qt::red);

        bool ok;
        QVariant var_amount = index(idx.row(), AmountIndex).data();

        float amount = var_amount.toFloat(&ok);
        if (!ok) {
            QString am = var_amount.toString();
            am.chop(2);
            amount = am.toFloat(&ok);
        }

        if (ok && amount > 0)
            return QBrush(Qt::darkGreen);
    }

    if (role == Qt::DisplayRole && column == AmountIndex)
        return QString::number(QSqlRelationalTableModel::data(idx, Qt::DisplayRole).toFloat()) + " €";

    return QSqlRelationalTableModel::data(idx, role);
}
