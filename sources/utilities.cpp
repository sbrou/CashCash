#include "utilities.h"

#include <QDate>

namespace utilities
{
    void capitalize(QString & string)
    {
        string.replace(0, 1, string[0].toUpper());
    }

    int daysInMonth(int month, int year)
    {
        if (month == 2)
            return QDate::isLeapYear(year) ? 29 : 28;
        else if (month <= 7)
            return month % 2 ? 31 : 30;
        else
            return month % 2 ? 30 : 31;
    }

    QString monthName(int month)
    {
        switch(month) {
        case 1:
            return QObject::tr("Janvier");
        case 2:
            return QObject::tr("Février");
        case 3:
            return QObject::tr("Mars");
        case 4:
            return QObject::tr("Avril");
        case 5:
            return QObject::tr("Mai");
        case 6:
            return QObject::tr("Juin");
        case 7:
            return QObject::tr("Juillet");
        case 8:
            return QObject::tr("Août");
        case 9:
            return QObject::tr("Septembre");
        case 10:
            return QObject::tr("Octobre");
        case 11:
            return QObject::tr("Novembre");
        case 12:
        case 0:
            return QObject::tr("Décembre");
        default:
            return "";
        }
    }

    void getTimePeriod(TimePeriod timePeriod, QDate & beginDate, QDate & endDate)
    {
        QDate today = QDate::currentDate();
        int year = today.year();
        int yearFrom = year;
        int yearTo = year;
        int month = today.month();

        int pre_month = month - 1;
        if (pre_month == 0) {
            pre_month = 12;
            --yearFrom;
        }

        int three_months_before = month - 2;
        if (three_months_before <= 0)
            three_months_before += 12;


        switch (timePeriod)
        {
        case PreviousMonth:
            beginDate = QDate(yearFrom, pre_month , 1);
            endDate = QDate(yearFrom, pre_month, daysInMonth(pre_month, yearFrom));
            break;
        case ThreePastMonths:
            beginDate = QDate(yearFrom, three_months_before, 1);
            endDate = QDate(yearTo, month, today.daysInMonth());
            break;
        case CurrentYear:
            beginDate = QDate(year, 1, 1);
            endDate = QDate(year, 12, 31);
            break;
        case PreviousYear:
            beginDate = QDate(year-1, 1, 1);
            endDate = QDate(year-1, 12, 31);
            break;
        default:
            beginDate = QDate(year, month, 1);
            endDate = QDate(year, month, today.daysInMonth());
            break;
        }
    }

    QString groupIcon(GroupType type)
    {
        switch (type)
        {
        case CatType:
            return ":/images/images/category.png";
        case TagType:
            return ":/images/images/tag.png";
        default:
            return "";
        }
    }

    QString groupName(GroupType type)
    {
        switch (type)
        {
        case CatType:
            return QObject::tr("Catégorie");
        case TagType:
            return QObject::tr("Tag");
        default:
            return "";
        }
    }

    QString groupTable(GroupType type)
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

    QString groupCondition(GroupType type, int id)
    {
        switch(type)
        {
        case CatType:
            return categoryCondition(id);
        case TagType:
            return tagCondition(id);
        default:
            return "";
        }
    }

    QString selectGroupCmd(GroupType type)
    {
        switch(type)
        {
        case CatType:
            return SELECT_CATEGORIES;
        case TagType:
            return SELECT_TAGS;
        default:
            return "";
        }
    }

    QString removeAGroup(GroupType type)
    {
        switch(type)
        {
        case CatType:
            return QObject::tr("Supprimer une catégorie");
        case TagType:
            return QObject::tr("Supprimer un tag");
        default:
            return "";
        }
    }

    QString removeTheGroup(GroupType type)
    {
        switch(type)
        {
        case CatType:
            return QObject::tr("supprimer la catégorie");
        case TagType:
            return QObject::tr("supprimer le tag");
        default:
            return "";
        }
    }

    QString lowerDateCondition(QDate date)
    {
        return QString("op_date>='%1'").arg(date.toString(Qt::ISODateWithMs));
    }

    QString upperDateCondition(QDate date)
    {
        return QString("op_date<='%1'").arg(date.toString(Qt::ISODateWithMs));
    }

    QString categoryCondition(int id)
    {
        return QString("category=%1").arg(id);
    }

    QString tagCondition(int id)
    {
        return QString("tag=%1").arg(id);
    }

    QString lowerAmountCondition(float amount)
    {
        return QString("amount>=%1").arg(amount);
    }

    QString upperAmountCondition(float amount)
    {
        return QString("amount<=%1").arg(amount);
    }

    QString descriptionCondition(const QString &str)
    {
        return QString("description LIKE '%%1%'").arg(str);
    }

    QString typeCondition(OpType type)
    {
        return QString("type=%1").arg(type);
    }

    QString idCondition(int id)
    {
        return QString("id=%1").arg(id);
    }


    // QueryStatement
    QueryStatement::QueryStatement(const QString & sqlCommand, const QString & condition)
        : command(sqlCommand)
    {
        if (!condition.isEmpty())
            conditions << condition;
    }

    void QueryStatement::setCommand(const QString & sqlCommand, bool clearAllConditions)
    {
        command = sqlCommand;
        if (clearAllConditions)
            conditions.clear();
    }

    void QueryStatement::addCondition(const QString & condition)
    {
        conditions << condition;
    }

    QString QueryStatement::get()
    {
        QString finalStatement(command);
        if (conditions.size() > 0) {
            finalStatement += " WHERE ";
            finalStatement += conditions.join(COND_SEP);
        }
        return finalStatement;
    }

    void QueryStatement::clearConditions(int i, qsizetype n)
    {
        if (i < 0)
            conditions.clear();
        else
            conditions.remove(i,n);
    }


    // class ToolBar
    ToolBar::ToolBar(QWidget *parent)
        : QToolBar{parent}
    {
        QAction* addAct = new QAction(ADD_ICON, tr("A&jouter"), this);
        connect(addAct, SIGNAL(triggered()), this, SLOT(addTriggered()));

        editAct = new QAction(EDIT_ICON, tr("É&diter"), this);
        connect(editAct, SIGNAL(triggered()), this, SLOT(editTriggered()));

        removeAct = new QAction(REMOVE_ICON, tr("S&upprimer"), this);
        connect(removeAct, SIGNAL(triggered()), this, SLOT(removeTriggered()));

        addAction(addAct);
        addAction(editAct);
        addAction(removeAct);

        removeAct->setEnabled(false);
        editAct->setEnabled(false);

        setIconSize(QSize(18,18));
    }

    void ToolBar::addTriggered()
    {
        emit actTriggered(AddAction);
    }

    void ToolBar::editTriggered()
    {
        emit actTriggered(EditAction);
    }

    void ToolBar::removeTriggered()
    {
        emit actTriggered(RemoveAction);
    }

    void ToolBar::updateActions(const QItemSelection &selected)
    {
        QModelIndexList indexes = selected.indexes();
        bool enabled = !indexes.isEmpty();
        removeAct->setEnabled(enabled);
        editAct->setEnabled(enabled);
    }
}



