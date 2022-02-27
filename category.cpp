#include "category.h"

//namespace category
//{
//    enum type
//    {
//        SPENDING=0x1,
//        INCOME=0x2,
//        SAVING=0x4
//    };
//}

//Category::Category(const QString& title, QObject *parent)
//    : QObject{parent},
//      _title(title)
//{

//}

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

//QString Category::title()
//{
//    return _title;
//}

double Category::amount()
{
    return _amount;
}

void Category::addOperation(double amount)
{
    _amount += amount;
}
