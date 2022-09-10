#ifndef DEFINES_H
#define DEFINES_H

#define DEFAULT_GROUP 1
#define COND_SEP " AND "

#include <QString>
#include <QMetaType>

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
    int typeId;
    double max;
};

Q_DECLARE_METATYPE(Goal)

#endif // DEFINES_H
