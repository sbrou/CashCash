#ifndef INITDB_H
#define INITDB_H

#include <defines.h>
#include <QtSql>

// les revenus ont comme type 1
void addOperationInDB(QSqlQuery &q, QDate date, const QVariant &categoryId,
             double amount, const QVariant &tagId, const QString &description, int type=Expense)
{
    q.addBindValue(date);
    q.addBindValue(categoryId);
    q.addBindValue(amount);
    q.addBindValue(tagId);
    q.addBindValue(description);
    q.addBindValue(type);
    q.exec();
}

QVariant addCategoryInDB(QSqlQuery &q, const QString &name, const QString& color, int type=Expense)
{
    q.addBindValue(name);
    q.addBindValue(color);
    q.addBindValue(type);
    q.exec();
    return q.lastInsertId();
}

QVariant addTagInDB(QSqlQuery &q, const QString &name, const QString& color, int type=Expense)
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

QSqlError manualInit(QSqlQuery &q);

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

QSqlError manualInit(QSqlQuery &q)
{
    if (!q.prepare(INSERT_TAG_SQL))
        return q.lastError();
    QVariant essId = addTagInDB(q, QLatin1String("ESSENTIAL"), QLatin1String("#BA4A00"));
    QVariant wanId = addTagInDB(q, QLatin1String("WANTING"), QLatin1String("#D68910"));
    QVariant savId = addTagInDB(q, QLatin1String("SAVING"), QLatin1String("#28B463"));
    QVariant excId = addTagInDB(q, QLatin1String("EXCEPTIONAL"), QLatin1String("#BA4A00"));
    QVariant incId = addTagInDB(q, QLatin1String("INCOME"), QLatin1String("#2E86C1"), 1);

    if (!q.prepare(INSERT_CATEGORY_SQL))
        return q.lastError();
    QVariant foodId = addCategoryInDB(q, QLatin1String("FOOD"), QLatin1String("#CA6F1E"));
    QVariant houseId = addCategoryInDB(q, QLatin1String("HOUSE"), QLatin1String("#D4AC0D"));
    QVariant hobbiesId = addCategoryInDB(q, QLatin1String("HOBBIES"), QLatin1String("#229954"));
    QVariant salaryId = addCategoryInDB(q, QLatin1String("SALARY"), QLatin1String("#17A589"), 1);
    QVariant livretId = addCategoryInDB(q, QLatin1String("LIVRET"), QLatin1String("#2471A3"));

    if (!q.prepare(INSERT_OPERATION_SQL))
        return q.lastError();
    addOperationInDB(q, QDate(2022, 1, 31), salaryId, 1857.29, incId, QLatin1String("VIREMENT DE ASTEK"), 1);
    addOperationInDB(q, QDate(2022, 1, 31), hobbiesId, -10.99, wanId, QLatin1String("ACHAT CB Deezer"));
    addOperationInDB(q, QDate(2022, 1, 31), hobbiesId, -7.8, wanId, QLatin1String("ACHAT CB BARAKA JEUX"));
    addOperationInDB(q, QDate(2022, 2, 7), houseId, -550, essId, QLatin1String("PRELEVEMENT DE SARL CHANTURGUE"));
    addOperationInDB(q, QDate(2022, 2, 7), houseId, -30, essId, QLatin1String("PRELEVEMENT DE TotalEnergies"));
    addOperationInDB(q, QDate(2022, 2, 7), houseId, -138, essId, QLatin1String("PRELEVEMENT DE TotalEnergies"));
    addOperationInDB(q, QDate(2022, 2, 21), foodId, -35.30, essId, QLatin1String("ACHAT CB CARREFOUR"));
    addOperationInDB(q, QDate(2022, 2, 3), foodId, -23.76, essId, QLatin1String("ACHAT CB CARREFOUR"));
    addOperationInDB(q, QDate(2022, 2, 1), livretId, -100, savId, QLatin1String("VIREMENT PERMANENT POUR MLE BROU SOPIE"));
    addOperationInDB(q, QDate(2022, 2, 3), houseId, -300, excId, QLatin1String("VIREMENT POUR COMPTE JOINT"));

    return q.lastError();
}
#endif // INITDB_H
