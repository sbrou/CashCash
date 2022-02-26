#ifndef OPERATIONSMODEL_H
#define OPERATIONSMODEL_H

#include <QAbstractTableModel>

#include <operation.h>

class OperationsTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit OperationsTableModel(QObject *parent = nullptr);
    explicit OperationsTableModel(QList<Operation*> ops, QObject *parent = nullptr);

    void loadOperations(QList<Operation*> ops);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex()) override;

//    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole) override;

    QList< Operation* > getOperations();

private:
    QList<Operation*> _operations;
};

#endif // OPERATIONSMODEL_H
