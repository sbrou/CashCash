#include "account.h"
#include "ui_account.h"

#include <QFileDialog>
#include <QPushButton>
#include <QDomDocument>
#include <QDebug>

#include <QPieSeries>

#include <addopdialog.h>

#include "catslist.h"

#include "initdb.h"

Account::Account(QString title, QWidget *parent)
    : QSplitter{parent}
    , _locale(QLocale::German)
    , _title(title)
    , _filepath("")
    , _nbOperations(0)
{
    setOrientation(Qt::Vertical);  
    splitter = new QSplitter(Qt::Horizontal, this);

    qDebug() << splitter->sizes();
//    accLayout = new QGridLayout(this);


    if (!QSqlDatabase::drivers().contains("QSQLITE"))
        QMessageBox::critical(
                    this,
                    "Unable to load database",
                    "This demo needs the SQLITE driver"
                    );

    // Initialize the database:
    QSqlError err = initDb();

    if (err.type() != QSqlError::NoError) {
        showError(err);
        return;
    }

//    initTabFilters();

//    createToolBar();

//    connect(ui->qpbSaveFile, SIGNAL(clicked()), this, SLOT(saveFile()));
}

bool Account::commitOnDatabase()
{
    model->database().transaction();
    bool st = model->submitAll();
    qDebug() << "in commit on database";
    if (st)
    {
        qDebug() << "commiting on database";
        model->database().commit();
        model->select();
//        updatePie();
    }

    return st;
}

void Account::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                    "Error initializing database: " + err.text());
}

void Account::initAccount()
{
    readSettings();

    // Create the data model:
    opsView = new OperationsView;
    model = new QSqlRelationalTableModel(opsView->table());
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("operations");
    model->setSort(1, Qt::DescendingOrder);

    // Remember the indexes of the columns:
    categoryIdx = model->fieldIndex("category");
    tagIdx = model->fieldIndex("tag");

    // Set the relations to the other database tables:
    model->setRelation(categoryIdx, QSqlRelation("categories", "id", "name"));
    model->setRelation(tagIdx, QSqlRelation("tags", "id", "name"));

    cats_model = model->relationModel(categoryIdx);
    tags_model = model->relationModel(tagIdx);

    // Set the localized header captions:
    model->setHeaderData(categoryIdx, Qt::Horizontal, tr("Category"));
    model->setHeaderData(tagIdx, Qt::Horizontal, tr("Tag"));
    model->setHeaderData(model->fieldIndex("op_date"),
                         Qt::Horizontal, tr("Date"));
    model->setHeaderData(model->fieldIndex("amount"), Qt::Horizontal, tr("Amount"));
    model->setHeaderData(model->fieldIndex("description"),
                         Qt::Horizontal, tr("Description"));

    // Populate the model:
    if (!model->select()) {
        showError(model->lastError());
        return;
    }

    _nbOperations = model->rowCount();
    qDebug() << "number of operations : " << _nbOperations;

    opsView->loadModel(model);
    connect(opsView->table()->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SIGNAL(selectionChanged(QItemSelection)));
//    accLayout->addWidget(opsView, 0, 0);
    splitter->addWidget(opsView);

    chartView = new ChartsView(model, this);
    connect(this, SIGNAL(operationsChanged(QSqlRecord)), chartView, SLOT(updateChart(QSqlRecord)));
//    accLayout->addWidget(chartView, 0, 1);
    splitter->addWidget(chartView);

    addWidget(splitter);

    goalsView = new GoalsView(this);
//    accLayout->addWidget(goalsView, 1, 0, 1, 2);
    addWidget(goalsView);

    catsWidget = new CatsList(cats_model, this);
    connect(catsWidget, SIGNAL(commit()), this, SLOT(commitOnDatabase()));

    tagsWidget = new TagsList(tags_model, this);
    connect(tagsWidget, SIGNAL(commit()), this, SLOT(commitOnDatabase()));

    emit accountReady();
}

void Account::initTabFilters()
{
//    QDate today = QDate::currentDate();
////    dateFrom = QDate(today.year(), today.month(), 1);
////    dateTo = QDate(today.year(), today.month(), today.daysInMonth());
//    dateFrom = QDate(today.year(), 2, 1);
//    dateTo = QDate(today.year(), 2, 28);
//    ui->qdeFilterFrom->setDate(dateFrom);
//    ui->qdeFilterTo->setDate(dateTo);

//    ui->qcbFilterCat->setModel(model->relationModel(categoryIdx));
//    ui->qcbFilterCat->setModelColumn(1);
//    ui->qcbFilterTag->setModel(model->relationModel(tagIdx));
//    ui->qcbFilterTag->setModelColumn(1);

//    filterMapper = new QDataWidgetMapper(this);
//    filterMapper->setModel(model);
//    filterMapper->addMapping(ui->qcbFilterCat, 2, "currentIndex");
//    filterMapper->addMapping(ui->qcbFilterTag, 4, "currentIndex");
//    filterMapper->toFirst();

//    setDateFilter();

//    connect(ui->qgbFilterDate, SIGNAL(toggled(bool)), this, SLOT(activateDateFilter(bool)));
//    connect(ui->qdeFilterFrom, SIGNAL(dateChanged(QDate)), this, SLOT(applyFromDateFilter(QDate)));
//    connect(ui->qdeFilterTo, SIGNAL(dateChanged(QDate)), this, SLOT(applyToDateFilter(QDate)));

//    connect(ui->qgbFilterCat, SIGNAL(toggled(bool)), this, SLOT(activateCatFilter(bool)));
//    connect(ui->qcbFilterCat, SIGNAL(currentIndexChanged(int)), this, SLOT(applyCatFilter(int)));

//    connect(ui->qgbFilterTag, SIGNAL(toggled(bool)), this, SLOT(activateTagFilter(bool)));
//    connect(ui->qcbFilterTag, SIGNAL(currentIndexChanged(int)), this, SLOT(applyTagFilter(int)));
}

void Account::importFile()
{
    QString filename = QFileDialog::getOpenFileName(nullptr,tr("Importer des operations"), QDir::home().dirName(), tr("ofx files (*.ofx)"));
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    QDomNodeList ops_node = doc.elementsByTagName("BANKTRANLIST");
    if (!ops_node.isEmpty())
    {
        QDomElement ops_elt = ops_node.at(0).toElement();
        QDomNodeList ops_list = ops_elt.elementsByTagName("STMTTRN");
        if (!ops_list.isEmpty())
        {
            QSqlQuery q;
            if (!q.prepare(INSERT_OPERATION_SQL))
            {
                qDebug() << q.lastError();
                return;
            }

            for (int i = 0; i < ops_list.count(); ++i)
            {
                QDomElement op = ops_list.at(i).toElement();

                QDomElement date_elt = op.elementsByTagName("DTPOSTED").at(0).toElement();
                QDate date = QDate::fromString(date_elt.text(),"yyyMMdd");

                QDomElement amt_elt = op.elementsByTagName("TRNAMT").at(0).toElement();
                double amount = amt_elt.text().toDouble();

                QDomNodeList des_node = op.elementsByTagName("NAME");
                QString description;
                if (!des_node.isEmpty())
                    description = des_node.at(0).toElement().text();
                else
                    description = op.elementsByTagName("PAYEE").at(0).toElement().text();

                qDebug() << date << amount << description;
                addOperationInDB(q, date, 1, amount, 1, description, (amount>0));

            }
        }
    }
//    model->select();
    commitOnDatabase();
}

void Account::editOperation()
{
    int idx = (int) opsView->table()->currentIndex().row();

    AddOpDialog aoDiag(idx, model);
    aoDiag.setWindowTitle(tr("Edit an operation"));
    aoDiag.setWindowIcon(QIcon(":/images/images/edit_48px.png"));

    if (aoDiag.exec())
        commitOnDatabase();
}

void Account::addOperation()
{
    AddOpDialog aoDiag(cats_model, tags_model);
    aoDiag.setWindowTitle(tr("Add an operation"));
    aoDiag.setWindowIcon(QIcon(":/images/images/add_48px.png"));
    qDebug() << model->rowCount();
    if (aoDiag.exec()) {

        QSqlRecord new_record = model->record();
        new_record.setValue(1, QVariant(aoDiag.date()));
        new_record.setValue(2, QVariant(aoDiag.category()));
        new_record.setValue(3, QVariant(aoDiag.amount()));
        new_record.setValue(4, QVariant(aoDiag.tag()));
        new_record.setValue(5, QVariant(aoDiag.description()));
        if (aoDiag.amount() < 0)
            new_record.setValue(6, QVariant(0));
        else
            new_record.setValue(6, QVariant(1));

        if (model->insertRecord(-1, new_record))
        {
            if (commitOnDatabase())
                emit operationsChanged(new_record);

        }
        qDebug() << model->rowCount();
    }
}

void Account::removeOperation()
{
    int ok = QMessageBox::warning(this,QStringLiteral("Supprimer les opérations sélectionnées"),
                                  QStringLiteral("Etes-vous sûr de vouloir supprimer les opérations sélectionnées?"),
                                  QMessageBox::Yes,QMessageBox::No);
    if(ok == QMessageBox::No)
        return;

    QItemSelectionModel *selectionModel = opsView->table()->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();
    QModelIndex index;

    foreach (index, indexes) {
        int row = index.row();
        model->removeRows(row,1);
    }
    commitOnDatabase();
}

void Account::setStandardCategories()
{

}

void Account::setStandardRules()
{
    _rules.insert("CARREFOUR","FOOD");
    _rules.insert("HOMESERVE","HOUSE");
    _rules.insert("PHARMACIE","HEALTH");
    _rules.insert("FNAC","HOBBIES");
    _rules.insert("Maxi Zoo","MAIKO");
    _rules.insert("CHANTURGUE","HOUSE");
    _rules.insert("TotalEnergies","HOUSE");
    _rules.insert("PROXISERV","HOUSE");
    _rules.insert("AVANSSUR","HOUSE");
    _rules.insert("JOINT","JOINT");
    _rules.insert("T2C","TRANSPORT");
    _rules.insert("FR5110011000207555808944J","SAVING");
    _rules.insert("BOUYGUES","SUBS");
    _rules.insert("Deezer","SUBS");
    _rules.insert("ASTEK","SALARY");
}

void Account::selectTest()
{
//    qDebug() << model->filter();
//    model->setFilter(QString("category='1'"));
//    QSqlQuery query = model->query();
//    while (query.next()) {
//        qDebug() << query.value(0);
//    }

}

void Account::setDateFilter()
{
//    qDebug() << "From " << dateFrom.toString(Qt::ISODateWithMs) << " To " << dateTo.toString(Qt::ISODateWithMs);
    model->setFilter(QString("(op_date>='%1' AND op_date<='%2')").arg(dateFrom.toString(Qt::ISODateWithMs)).arg(dateTo.toString(Qt::ISODateWithMs)));
}

void Account::activateDateFilter(bool on)
{
//    if (on)
//    {
//        dateFrom = ui->qdeFilterFrom->date();
//        dateTo = ui->qdeFilterTo->date();
//        setDateFilter();
//    }
//    else
//    {
//        model->setFilter("");
//    }
}

void Account::applyFromDateFilter(QDate from)
{
    dateFrom = from;
    setDateFilter();
}

void Account::applyToDateFilter(QDate to)
{
    dateTo = to;
    setDateFilter();
}

void Account::activateCatFilter(bool on)
{
//    if (on)
//    {
//        int cat = ui->qcbFilterCat->currentIndex() + 1;
//        model->setFilter(QString("category='%1'").arg(cat));
//    }
//    else
//    {
//        model->setFilter("");
////        qDebug() << model->filter();
//    }
}

void Account::applyCatFilter(int cat)
{
    model->setFilter(QString("category='%1'").arg(cat+1));
}

void Account::activateTagFilter(bool on)
{
//    if (on)
//    {
//        int tag = ui->qcbFilterTag->currentIndex() + 1;
//        model->setFilter(QString("tag='%1'").arg(tag));
//    }
//    else
//    {
//        model->setFilter("");
//    }
}

void Account::applyTagFilter(int tag)
{
    model->setFilter(QString("tag='%1'").arg(tag+1));
}

void Account::saveFile()
{
    QString saveFilename = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    "D:/sopie/Documents/untitled.bsx",
                                                    tr("BSX files (*.bsx)"));
    QFile saveFile(saveFilename);

    commitOnDatabase();

    if (!saveFile.open(QIODevice::WriteOnly)) {
            qWarning("Couldn't open save file.");
            return;
        }

    QJsonObject accObject;
    QSqlQuery q;

//    write(accObject);

    ///// Write Cats /////

    QJsonArray catsArray;

    q.exec("SELECT * FROM categories");
    while (q.next()) {
        QJsonObject cat;
        cat["name"] = q.value(1).toString();
        cat["color"] = q.value(2).toString();
        cat["type"] = q.value(3).toInt();

        catsArray.append(cat);
    }
    accObject["categories"] = catsArray;

    ///// Write Tags /////

    QJsonArray tagsArray;

    q.exec("SELECT * FROM tags");
    while (q.next()) {
        QJsonObject tag;
        tag["name"] = q.value(1).toString();
        tag["color"] = q.value(2).toString();
        tag["type"] = q.value(3).toInt();

        tagsArray.append(tag);
    }
    accObject["tags"] = tagsArray;

    ///// Write Operations /////

    QJsonArray opsArray;

    q.exec("SELECT * FROM operations");
    while (q.next()) {
        QJsonObject op;
        op["date"] = q.value(1).toString();
        op["category"] = q.value(2).toInt();
        op["amount"] = q.value(3).toDouble();
        op["tag"] = q.value(4).toInt();
        op["description"] = q.value(5).toString();
        op["type"] = q.value(6).toInt();

        opsArray.append(op);
    }
    accObject["operations"] = opsArray;

    saveFile.write(QCborValue::fromJsonValue(accObject).toCbor());
}

void Account::saveSettings()
{
    qDebug() << splitter->sizes();
    QSettings settings;
    settings.setValue("subSplitterSizes", splitter->saveState());
    settings.setValue("mainSplitterSizes", saveState());
    settings.setValue("lastFile", QVariant(_filepath));
}

void Account::readSettings()
{
    QSettings settings;
    splitter->restoreState(settings.value("subSplitterSizes").toByteArray());
    restoreState(settings.value("mainSplitterSizes").toByteArray());
}

QSqlError Account::loadFile(const QString& filename)
{
    QSqlError err;

    QString loadFilename = filename.isEmpty() ? QFileDialog::getOpenFileName(this, tr("Open File"),
                                                                             "D:/sopie/Documents",
                                                                             tr("BSX files (*.bsx)")) : filename;

    QFile loadFile(loadFilename);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return QSqlError("Couldn't open save file.","",QSqlError::UnknownError);
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument(QCborValue::fromCbor(saveData).toMap().toJsonObject()));
    QJsonObject loadObject = loadDoc.object();
    QSqlQuery q;

    ///// Read Cats /////

    if (loadObject.contains("categories") && loadObject["categories"].isArray()) {
        q.exec("TRUNCATE TABLE categories");
        QJsonArray catsArray = loadObject["categories"].toArray();
        if (readCategories(INSERT_CATEGORY_SQL, catsArray).type() != QSqlError::NoError)
            return q.lastError();
    }

    ///// Read Tags /////

    if (loadObject.contains("tags") && loadObject["tags"].isArray()) {
        q.exec("TRUNCATE TABLE tags");
        QJsonArray tagsArray = loadObject["tags"].toArray();
        if (readCategories(INSERT_TAG_SQL, tagsArray).type() != QSqlError::NoError)
            return q.lastError();
    }

    ///// Read Operations /////
    if (loadObject.contains("operations") && loadObject["operations"].isArray()) {
        q.exec("TRUNCATE TABLE operations");
        QJsonArray opsArray = loadObject["operations"].toArray();
        if (readOperations(opsArray).type() != QSqlError::NoError)
            return q.lastError();
    }

    _filepath = loadFilename;
    initAccount();

    return err;
}

QSqlError Account::readCategories(const QString& query, const QJsonArray &catsArray)
{
    QSqlQuery q;
    if (!q.prepare(query))
        return q.lastError();

    for (int catIndex = 0; catIndex < catsArray.size(); ++catIndex) {
        QJsonObject catObject = catsArray[catIndex].toObject();

        QString name;
        QString color;
        int type;

        if (catObject.contains("name") && catObject["name"].isString())
            name = catObject["name"].toString();
        else
            continue;

        if (catObject.contains("color") && catObject["color"].isString())
            color = catObject["color"].toString();
        else
            continue;

        if (catObject.contains("type") && catObject["type"].isDouble())
            type = catObject["type"].toInt();
        else
            continue;

        addCategoryInDB(q, name, color, type);
    }

    return q.lastError();
}

QSqlError Account::readOperations(const QJsonArray &opsArray)
{
    QSqlQuery q;
    if (!q.prepare(INSERT_OPERATION_SQL))
        return q.lastError();

    for (int opIndex = 0; opIndex < opsArray.size(); ++opIndex)
    {
        QJsonObject opObject = opsArray[opIndex].toObject();

        QDate date;
        int category;
        double amount;
        int tag;
        QString description;
        int type;

        if (opObject.contains("date") && opObject["date"].isString())
            date = QDate::fromString(opObject["date"].toString(), "yyyy-MM-dd");
        else
            continue;

        if (opObject.contains("category") && opObject["category"].isDouble())
            category = opObject["category"].toInt();
        else
            continue;

        if (opObject.contains("amount") && (opObject["amount"].isDouble()))
            amount = opObject["amount"].toDouble();
        else
            continue;

        if (opObject.contains("tag") && (opObject["tag"].isDouble()))
            tag = opObject["tag"].toInt();
        else
            continue;

        if (opObject.contains("description") && (opObject["description"].isString()))
            description = opObject["description"].toString();
        else
            continue;

        if (opObject.contains("type") && (opObject["type"].isDouble()))
            type = opObject["type"].toInt();
        else
            continue;

        addOperationInDB(q, date, category, amount, tag, description, type);
    }

    return q.lastError();
}

void Account::showCategories()
{
    catsWidget->show();
}

void Account::showTags()
{
    tagsWidget->show();
}
