#ifndef DEFINES_H
#define DEFINES_H

#define DEFAULT_GROUP 1

#include <QString>

enum OpType {
    Expense = 0,
    Earning
};

enum GroupType {
    CatType,
    TagType
};

enum IndexInTable {
    DateIndex = 1,
    CatIndex,
    AmountIndex,
    TagIndex,
    DesIndex,
    OpTypeIndex
};

struct Goal {
    GroupType type;
    QString typeName;
    double max;
};

#endif // DEFINES_H
