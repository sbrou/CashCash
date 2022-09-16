#ifndef INITDB_H
#define INITDB_H

#include <defines.h>
#include <QtSql>

// les revenus ont comme type 1
void addOperationInDB(QSqlQuery &q, QDate date, const QVariant &categoryId,
             float amount, const QVariant &tagId, const QString &description, int type=Expense)
{
    q.addBindValue(date);
    q.addBindValue(categoryId);
    q.addBindValue(amount);
    q.addBindValue(tagId);
    q.addBindValue(description);
    q.addBindValue(type);
    q.exec();
}

QVariant addGroupInDB(QSqlQuery &q, const QString &name, const QString& color, int type=Expense)
{
    q.addBindValue(name);
    q.addBindValue(color);
    q.addBindValue(type);
    q.exec();
    return q.lastInsertId();
}

const auto OPERATIONS_SQL = QLatin1String(R"(
    create table operations(id integer primary key, op_date date, category integer, amount decimal(10,2),
                       tag integer, description varchar, type integer)
    )");

const auto CATEGORIES_SQL =  QLatin1String(R"(
    create table categories(id integer primary key, name varchar, color varchar, type integer)
    )");

const auto TAGS_SQL = QLatin1String(R"(
    create table tags(id integer primary key, name varchar, color varchar, type integer)
    )");

const auto INSERT_CATEGORY_SQL = QLatin1String(R"(
    insert into categories(name, color, type) values(?, ?, ?)
    )");

const auto INSERT_OPERATION_SQL = QLatin1String(R"(
    insert into operations(op_date, category, amount, tag, description, type)
                      values(?, ?, ?, ?, ?, ?)
    )");

const auto INSERT_TAG_SQL = QLatin1String(R"(
    insert into tags(name, color, type) values(?, ?, ?)
    )");

QSqlError initDb(const QString & title)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", title);
    db.setDatabaseName(":memory:");

    if (!db.open())
        return db.lastError();

    QStringList tables = db.tables();
    if (tables.contains("operations", Qt::CaseInsensitive)
        && tables.contains("categories", Qt::CaseInsensitive)
        && tables.contains("tags", Qt::CaseInsensitive))
        return QSqlError();

    QSqlQuery q(db);
    if (!q.exec(OPERATIONS_SQL))
        return q.lastError();
    if (!q.exec(CATEGORIES_SQL))
        return q.lastError();
    if (!q.exec(TAGS_SQL))
        return q.lastError();

    return QSqlError();
}

#endif // INITDB_H
