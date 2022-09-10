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
}



