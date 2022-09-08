#ifndef DEFINES_H
#define DEFINES_H

#define EXPENSES_TYPE 0
#define EARNINGS_TYPE 1

#include <QString>

enum GroupType {
    CatType,
    TagType
};

struct Goal {
    GroupType type;
    QString typeName;
    double max;
};

#endif // DEFINES_H
