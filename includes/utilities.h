#ifndef UTILITIES_H
#define UTILITIES_H

#include "defines.h"

namespace Utilities
{
    int daysInMonth(int month, int year);

    QString groupIconByType(GroupType type);
    QString groupNameByType(GroupType type);
    QString groupTableByType(GroupType type);
}

#endif // UTILITIES_H
