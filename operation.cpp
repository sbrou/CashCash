#include "operation.h"

#include <QDebug>

Operation::Operation(QDate date, const QString &des, double amount, const QString &cat, const QString &type, QObject *parent)
    : QObject{parent},
      _date(date),
      _amount(amount),
      _description(des),
      _type(type),
      _category(cat)
{
//    qDebug() << _date << _amount << _description << _category << _type;
}

Operation::Operation(const Operation & other)
{
    _date = other._date;
    _amount = other._amount;
    _description = other._description;
    _type = other._type;
    _category = other._category;
}

QDate Operation::date()
{
    return _date;
}

QString Operation::category()
{
    return _category;
}

double Operation::amount()
{
    return _amount;
}

QString Operation::description()
{
    return _description;
}

void Operation::setDate(QDate date)
{
    _date = date;
}

void Operation::setCategory(const QString &category)
{
    _category = category;
}

void Operation::setAmount(double amount)
{
    _amount = amount;
}

void Operation::setDescription(const QString &description)
{
    _description = description;
}
