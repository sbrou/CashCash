#ifndef DETAILSMODEL_H
#define DETAILSMODEL_H

#include <QAbstractTableModel>

class detailsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit detailsModel(QObject *parent = nullptr);
};

#endif // DETAILSMODEL_H
