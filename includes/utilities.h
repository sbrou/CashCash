#ifndef UTILITIES_H
#define UTILITIES_H

#include "defines.h"

namespace Utilities
{
    // Date functions
    int daysInMonth(int month, int year);

    // Group functions
    QString groupIconByType(GroupType type);
    QString groupNameByType(GroupType type);
    QString groupTableByType(GroupType type);
    QString groupCondition(GroupType, int id);

    // Query statements functions
    QString lowerDateCondition(QDate date);
    QString upperDateCondition(QDate date);
    QString categoryCondition(int id);
    QString tagCondition(int id);
    QString lowerAmountCondition(double amount);
    QString upperAmountCondition(double amount);
    QString descriptionCondition(QString str);
    QString typeCondition(OpType type);
    QString idCondition(int id);

}

#endif // UTILITIES_H
