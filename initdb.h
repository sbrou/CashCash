#ifndef INITDB_H
#define INITDB_H

#include <QtSql>

void addOperationInDB(QSqlQuery &q, QDate date, const QVariant &categoryId,
             double amount, const QVariant &tagId, const QString &description)
{
    q.addBindValue(date);
    q.addBindValue(categoryId);
    q.addBindValue(amount);
    q.addBindValue(tagId);
    q.addBindValue(description);
    q.exec();
}

QVariant addCategoryInDB(QSqlQuery &q, const QString &name)
{
    q.addBindValue(name);
    q.exec();
    return q.lastInsertId();
}

QVariant addTagInDB(QSqlQuery &q, const QString &name)
{
    q.addBindValue(name);
    q.exec();
    return q.lastInsertId();
}

const auto OPERATIONS_SQL = QLatin1String(R"(
    create table operations(id integer primary key, op_date date, category integer, amount decimal(10,2),
                       tag integer, description varchar)
    )");

const auto CATEGORIES_SQL =  QLatin1String(R"(
    create table categories(id integer primary key, name varchar)
    )");

const auto TAGS_SQL = QLatin1String(R"(
    create table tags(id integer primary key, name varchar)
    )");

const auto INSERT_CATEGORY_SQL = QLatin1String(R"(
    insert into categories(name) values(?)
    )");

const auto INSERT_OPERATION_SQL = QLatin1String(R"(
    insert into operations(op_date, category, amount, tag, description)
                      values(?, ?, ?, ?, ?)
    )");

const auto INSERT_TAG_SQL = QLatin1String(R"(
    insert into tags(name) values(?)
    )");

QSqlError initDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");

    if (!db.open())
        return db.lastError();

    QStringList tables = db.tables();
    if (tables.contains("books", Qt::CaseInsensitive)
        && tables.contains("authors", Qt::CaseInsensitive))
        return QSqlError();

    QSqlQuery q;
    if (!q.exec(OPERATIONS_SQL))
        return q.lastError();
    if (!q.exec(CATEGORIES_SQL))
        return q.lastError();
    if (!q.exec(TAGS_SQL))
        return q.lastError();

    if (!q.prepare(INSERT_TAG_SQL))
        return q.lastError();
    QVariant essId = addTagInDB(q, QLatin1String("ESSENTIAL"));
    QVariant wanId = addTagInDB(q, QLatin1String("WANTING"));
    QVariant savId = addTagInDB(q, QLatin1String("SAVING"));
    QVariant excId = addTagInDB(q, QLatin1String("EXCEPTIONAL"));
    QVariant incId = addTagInDB(q, QLatin1String("INCOME"));

    if (!q.prepare(INSERT_CATEGORY_SQL))
        return q.lastError();
    QVariant foodId = addCategoryInDB(q, QLatin1String("FOOD"));
    QVariant houseId = addCategoryInDB(q, QLatin1String("HOUSE"));
    QVariant hobbiesId = addCategoryInDB(q, QLatin1String("HOBBIES"));
    QVariant salaryId = addCategoryInDB(q, QLatin1String("SALARY"));
    QVariant livretId = addCategoryInDB(q, QLatin1String("LIVRET"));

    if (!q.prepare(INSERT_OPERATION_SQL))
        return q.lastError();
    addOperationInDB(q, QDate(2022, 1, 31), salaryId, 1857.29, incId, QLatin1String("VIREMENT DE ASTEK"));
    addOperationInDB(q, QDate(2022, 1, 31), hobbiesId, -10.99, wanId, QLatin1String("ACHAT CB Deezer"));
    addOperationInDB(q, QDate(2022, 1, 31), hobbiesId, -7.8, wanId, QLatin1String("ACHAT CB BARAKA JEUX"));
    addOperationInDB(q, QDate(2022, 2, 7), houseId, -550, essId, QLatin1String("PRELEVEMENT DE SARL CHANTURGUE"));
    addOperationInDB(q, QDate(2022, 2, 7), houseId, -30, essId, QLatin1String("PRELEVEMENT DE TotalEnergies"));
    addOperationInDB(q, QDate(2022, 2, 7), houseId, -138, essId, QLatin1String("PRELEVEMENT DE TotalEnergies"));
    addOperationInDB(q, QDate(2022, 2, 21), foodId, -35.30, essId, QLatin1String("ACHAT CB CARREFOUR"));
    addOperationInDB(q, QDate(2022, 2, 3), foodId, -23.76, essId, QLatin1String("ACHAT CB CARREFOUR"));
    addOperationInDB(q, QDate(2022, 2, 1), livretId, -100, savId, QLatin1String("VIREMENT PERMANENT POUR MLE BROU SOPIE"));
    addOperationInDB(q, QDate(2022, 2, 3), houseId, -300, excId, QLatin1String("VIREMENT POUR COMPTE JOINT"));

    return QSqlError();
}

#endif // INITDB_H
