#ifndef UTILITIES_H
#define UTILITIES_H

#include <QStringList>
#include <QWidget>
#include <QToolBar>
#include <QItemSelection>

#include "defines.h"

namespace utilities
{
    void capitalize(QString &);

    // Date functions
    int daysInMonth(int month, int year);
    QString monthName(int month);
    void getTimePeriod(TimePeriod timePeriod, QDate & beginDate, QDate & endDate);

    // Group functions
    QString groupIcon(GroupType type);
    QString groupName(GroupType type);
    QString groupTable(GroupType type);
    QString groupCondition(GroupType, int id);
    QString selectGroupCmd(GroupType);
    QString removeAGroup(GroupType);
    QString removeTheGroup(GroupType);

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

    class ToolBar : public QToolBar
    {
        Q_OBJECT
    public:
        ToolBar(QWidget* parent = 0);

    public slots:
        void updateActions(const QItemSelection &selected);

    signals:
        void actTriggered(Action);

    private slots:
        void addTriggered();
        void editTriggered();
        void removeTriggered();

    private:
        QAction* editAct;
        QAction* removeAct;
    };
}

#endif // UTILITIES_H
