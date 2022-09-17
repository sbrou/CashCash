#ifndef DEFINES_H
#define DEFINES_H

#define DEFAULT_GROUP 1
#define COND_SEP " AND "

#define SELECT_OPERATIONS "SELECT * FROM operations"
#define SELECT_CATEGORIES "SELECT * FROM categories"
#define SELECT_TAGS "SELECT * FROM tags"
#define SELECT_SUM "SELECT SUM (amount) FROM operations"

#define ADD_ICON QIcon(":/images/images/add_48px.png")
#define EDIT_ICON QIcon(":/images/images/edit_48px.png")
#define REMOVE_ICON QIcon(":/images/images/Remove_48px.png")

#include <QMetaType>

enum AccountState {
    Empty,
    UpToDate,
    Modified
};

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
    float max;
};

Q_DECLARE_METATYPE(Goal)

enum TimePeriod {
    CurrentMonth,
    PreviousMonth,
    ThreePastMonths,
    CurrentYear,
    PreviousYear
};

enum Action
{
    AddAction,
    EditAction,
    RemoveAction
};

#endif // DEFINES_H
