#ifndef UTILITIES_H
#define UTILITIES_H

#include <QStringList>

#include "defines.h"

namespace Utilities
{
    // Date functions
    int daysInMonth(int month, int year);
    QString monthName(int month);
    void getTimePeriod(TimePeriod timePeriod, QDate & beginDate, QDate & endDate);

    // Group functions
    QString groupIconByType(GroupType type);
    QString groupNameByType(GroupType type);
    QString groupTableByType(GroupType type);
    QString groupCondition(GroupType, int id);
    QString selectGroupCmd(GroupType);

    // Query statements functions
    QString lowerDateCondition(QDate date);
    QString upperDateCondition(QDate date);
    QString categoryCondition(int id);
    QString tagCondition(int id);
    QString lowerAmountCondition(float amount);
    QString upperAmountCondition(float amount);
    QString descriptionCondition(const QString &str);
    QString typeCondition(OpType type);
    QString idCondition(int id);

    class QueryStatement
    {
    public:
        QueryStatement(const QString & sqlCommand, const QString & condition = "");
        void setCommand(const QString & condition, bool clearAllConditions = true);
        void addCondition(const QString & condition);
        QString get();

        // if i equals -1, removes all conditions
        // else, removes n conditions starting at index position i.
        void clearConditions(int i = -1, qsizetype n = 1);

    private:
        QString command;
        QStringList conditions;
    };
}

#endif // UTILITIES_H
