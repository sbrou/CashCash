#ifndef INITDB_H
#define INITDB_H

#include <QtSql>

void addOperation(QSqlQuery &q, QDate date, const QVariant &categoryId,
             double amount, const QVariant &tagId, const QString &description)
{
    q.addBindValue(date);
    q.addBindValue(categoryId);
    q.addBindValue(amount);
    q.addBindValue(tagId);
    q.addBindValue(description);
    q.exec();
}

QVariant addCategory(QSqlQuery &q, const QString &name)
{
    q.addBindValue(name);
    q.exec();
    return q.lastInsertId();
}

QVariant addTag(QSqlQuery &q, const QString &name)
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
    QVariant essId = addTag(q, QLatin1String("ESSENTIAL"));
    QVariant wanId = addTag(q, QLatin1String("WANTING"));
    QVariant savId = addTag(q, QLatin1String("SAVING"));
    QVariant excId = addTag(q, QLatin1String("EXCEPTIONAL"));
    QVariant incId = addTag(q, QLatin1String("INCOME"));

    if (!q.prepare(INSERT_CATEGORY_SQL))
        return q.lastError();
    QVariant foodId = addCategory(q, QLatin1String("FOOD"));
    QVariant houseId = addCategory(q, QLatin1String("HOUSE"));
    QVariant hobbiesId = addCategory(q, QLatin1String("HOBBIES"));
    QVariant salaryId = addCategory(q, QLatin1String("SALARY"));
    QVariant livretId = addCategory(q, QLatin1String("LIVRET"));

    if (!q.prepare(INSERT_OPERATION_SQL))
        return q.lastError();
    addOperation(q, QDate(2022, 1, 31), salaryId, 1857.29, incId, QLatin1String("VIREMENT DE ASTEK"));
    addOperation(q, QDate(2022, 1, 31), hobbiesId, -10.99, wanId, QLatin1String("ACHAT CB Deezer"));
    addOperation(q, QDate(2022, 1, 31), hobbiesId, -7.8, wanId, QLatin1String("ACHAT CB BARAKA JEUX"));
    addOperation(q, QDate(2022, 2, 7), houseId, -550, essId, QLatin1String("PRELEVEMENT DE SARL CHANTURGUE"));
    addOperation(q, QDate(2022, 2, 7), houseId, -30, essId, QLatin1String("PRELEVEMENT DE TotalEnergies"));
    addOperation(q, QDate(2022, 2, 7), houseId, -138, essId, QLatin1String("PRELEVEMENT DE TotalEnergies"));
    addOperation(q, QDate(2022, 2, 21), foodId, -35.30, essId, QLatin1String("ACHAT CB CARREFOUR"));
    addOperation(q, QDate(2022, 2, 3), foodId, -23.76, essId, QLatin1String("ACHAT CB CARREFOUR"));
    addOperation(q, QDate(2022, 2, 1), livretId, -100, savId, QLatin1String("VIREMENT PERMANENT POUR MLE BROU SOPIE"));
    addOperation(q, QDate(2022, 2, 3), houseId, -300, excId, QLatin1String("VIREMENT POUR COMPTE JOINT"));

    return QSqlError();
}

#endif // INITDB_H
