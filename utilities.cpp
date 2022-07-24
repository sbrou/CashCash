#include "utilities.h"

#include <QDate>

int daysInMonth(int month, int year)
{
    if (month == 2)
        return QDate::isLeapYear(year) ? 29 : 28;
    else if (month <= 7)
        return month % 2 ? 31 : 30;
    else
        return month % 2 ? 30 : 31;
}

