#include "operationsmodel.h"

OperationsTableModel::OperationsTableModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

OperationsTableModel::OperationsTableModel(QList<Operation*> ops, QObject *parent)
    : QAbstractTableModel{parent}
{
    _operations = ops;
}

int OperationsTableModel::rowCount(const QModelIndex & /* parent */) const
{
    return _operations.size();
}

int OperationsTableModel::columnCount(const QModelIndex & /* parent */) const
{
    return 4;
}

QVariant OperationsTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= _operations.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        Operation* op = _operations.at(index.row());

        if (index.column() == 0)
            return op->date();
        else if (index.column() == 1)
            return op->category();
        else if (index.column() == 2)
            return op->amount();
        else if (index.column() == 3)
            return op->description();
    }
    return QVariant();
}

QVariant OperationsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Date");

            case 1:
                return tr("Catégorie");

            case 2:
                return tr("Montant");

            case 3:
                return tr("Description");

            default:
                return QVariant();
        }
    }
    return QVariant();
}

bool OperationsTableModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; row++) {
        Operation* op = new Operation(QDate(),"",0,"","");
        _operations.insert(position, op);
    }

    endInsertRows();
    return true;
}

bool OperationsTableModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; ++row) {
        _operations.removeAt(position);
    }

    endRemoveRows();
    return true;
}

bool OperationsTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();

        Operation* op = _operations.value(row);

        if (index.column() == 0)
            op->setDate(value.toDate());
        else if (index.column() == 1)
            op->setCategory(value.toString());
        else if (index.column() == 2)
            op->setAmount(value.toDouble());
        else if (index.column() == 3)
            op->setDescription(value.toString());
        else
            return false;

        _operations.replace(row, op);
        emit dataChanged(index, index);

        return true;
    }

    return false;
}

//Qt::ItemFlags OperationsTableModel::flags(const QModelIndex &index) const
//{
//    if (!index.isValid())
//        return Qt::ItemIsEnabled;

//    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
//}

QList< Operation* > OperationsTableModel::getOperations()
{
    return _operations;
}
