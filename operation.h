#ifndef OPERATION_H
#define OPERATION_H

#include <QObject>
#include <QDate>

#include <category.h>

class Operation : public QObject
{
    Q_OBJECT
public:
    explicit Operation(QDate date, const QString &comment, double amount, const QString &cat, const QString &type, QObject *parent = nullptr);

    QDate date();
    QString category();
    double amount();
    QString description();

    void setDate(QDate);
    void setCategory(const QString&);
    void setAmount(double);
    void setDescription(const QString&);

signals:

private:
    QDate _date;
    double _amount;
    QString _description;
    QString _type;     // Essentials / Wantings / Savings
    QString _category; // food, house, ...
};

#endif // OPERATION_H
