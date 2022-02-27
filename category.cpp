#include "category.h"

Category::Category(QObject *parent)
    : QObject{parent}
    , _type(Type::NONE)
    , _budget(0)
    , _amount(0)
{

}

Category::Category(Category::Type type, QObject *parent)
    : QObject{parent}
    , _type(type)
    , _budget(0)
    , _amount(0)
{

}

double Category::amount()
{
    return _amount;
}

void Category::addOperation(double amount)
{
    _amount += amount;
}
