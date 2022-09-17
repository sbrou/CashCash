#ifndef SQLRELATIONALTABLEMODEL_H
#define SQLRELATIONALTABLEMODEL_H

#include <QSqlRelationalTableModel>

class SqlRelationalTableModel : public QSqlRelationalTableModel
{
    Q_OBJECT
public:
    explicit SqlRelationalTableModel(QObject *parent = nullptr, const QSqlDatabase &db = QSqlDatabase());
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // SQLRELATIONALMODEL_H
