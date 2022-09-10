#include "utilities.h"

#include <QDate>

namespace Utilities
{
    int daysInMonth(int month, int year)
    {
        if (month == 2)
            return QDate::isLeapYear(year) ? 29 : 28;
        else if (month <= 7)
            return month % 2 ? 31 : 30;
        else
            return month % 2 ? 30 : 31;
    }

    QString groupIconByType(GroupType type)
    {
        switch (type)
        {
        case CatType:
            return ":/images/images/category_48px.png";
        case TagType:
            return ":/images/images/tag_window_48px.png";
        default:
            return "";
        }
    }

    QString groupNameByType(GroupType type)
    {
        switch (type)
        {
        case CatType:
            return "category";
        case TagType:
            return "tag";
        default:
            return "";
        }
    }

    QString groupTableByType(GroupType type)
    {
        switch (type)
        {
        case CatType:
            return "categories";
        case TagType:
            return "tags";
        default:
            return "";
        }
    }

    QString groupCondition(GroupType type, int id)
    {
        switch(type)
        {
        case CatType:
            return categoryCondition(id);
        case TagType:
            return tagCondition(id);
        default:
            return "";
        }
    }

    QString lowerDateCondition(QDate date)
    {
        return QString("op_date>='%1'").arg(date.toString(Qt::ISODateWithMs));
    }

    QString upperDateCondition(QDate date)
    {
        return QString("op_date<='%1'").arg(date.toString(Qt::ISODateWithMs));
    }

    QString categoryCondition(int id)
    {
        return QString("category=%1").arg(id);
    }

    QString tagCondition(int id)
    {
        return QString("tag=%1").arg(id);
    }

    QString lowerAmountCondition(double amount)
    {
        return QString("amount>=%1").arg(amount);
    }

    QString upperAmountCondition(double amount)
    {
        return QString("amount<=%1").arg(amount);
    }

    QString descriptionCondition(QString str)
    {
        return QString("description LIKE '%%1%'").arg(str);
    }

    QString typeCondition(OpType type)
    {
        return QString("type=%1").arg(type);
    }

    QString idCondition(int id)
    {
        return QString("id=%1").arg(id);
    }
}



