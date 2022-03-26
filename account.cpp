#include "account.h"
#include "ui_account.h"

#include <QFileDialog>
#include <QPushButton>
#include <QDebug>

#include <QPieSeries>

#include <addopdialog.h>
#include <addcatdialog.h>

#include "initdb.h"

Account::Account(QString title, QWidget *parent)
    : QWidget{parent}
    , ui(new Ui::Account)
    , _locale(QLocale::German)
    , _title(title)
    , _nbOperations(0)
{
    ui->setupUi(this);
    connect(ui->qpbTest, &QPushButton::clicked, this, &Account::selectTest);

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

    //// **** Temporaire **** ////
    err = loadFile();
    if (err.type() != QSqlError::NoError) {
        showError(err);
        return;
    }
    //// **** Temporaire **** ////


    // Create the data model:
    model = new QSqlRelationalTableModel(ui->opsView);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("operations");
    model->setSort(1, Qt::DescendingOrder);

    // Remember the indexes of the columns:
    categoryIdx = model->fieldIndex("category");
    tagIdx = model->fieldIndex("tag");

    // Set the relations to the other database tables:
    model->setRelation(categoryIdx, QSqlRelation("categories", "id", "name"));
    model->setRelation(tagIdx, QSqlRelation("tags", "id", "name"));

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

    // Set the model and hide the ID column:
    ui->opsView->setModel(model);
    ui->opsView->setItemDelegate(new QSqlRelationalDelegate(ui->opsView));
    //        ui->opsView->setColumnHidden(model->fieldIndex("id"), true);
    ui->opsView->setColumnHidden(model->fieldIndex("type"), true);
    ui->opsView->setCurrentIndex(model->index(0, 0));
    ui->opsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->opsView->horizontalHeader()->setStretchLastSection(true);
    ui->opsView->verticalHeader()->hide();
    ui->opsView->clearSpans();
    ui->opsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->opsView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->opsView->resizeColumnsToContents();

    //    connect(ui->qpbImportFile, SIGNAL(clicked()), this, SLOT(importFile()));

    //    setStandardCategories();
    //    setStandardRules();

    chart = new DrilldownChart();
    chart->setTheme(QChart::ChartThemeLight);
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    updatePie();

    chartView = new QChartView(chart, ui->catsWidget);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(380,260);

    initTabFilters();

    createToolBar();

    connect(ui->qpbSaveFile, SIGNAL(clicked()), this, SLOT(saveFile()));
}

bool Account::commitOnDatabase()
{
    model->database().transaction();
    bool st = model->submitAll();
    if (st)
    {
        qDebug() << "commiting on database";
        model->database().commit();
        model->select();
        updatePie();
    }

    return st;
}

void Account::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                    "Error initializing database: " + err.text());
}

void Account::updatePie()
{
    QPieSeries *tagsSeries = new QPieSeries(ui->catsWidget);
    tagsSeries->setName("Tags");


    model->select();
    QSqlQuery qu("SELECT * FROM operations WHERE category=1");
    while (qu.next()) {
//        qDebug() << "tag : " << qu.value(0);
//        qDebug() << "      " << qu.value(1);
//        qDebug() << "      " << qu.value(2);
//        qDebug() << "      " << qu.value(3);
//        qDebug() << "      " << qu.value(4);
//        qDebug() << "      " << qu.value(5);
    }

    QMap<int,QString> map_tags;
    QMap<int,QColor> tags_colors;
    QSqlQuery q("SELECT * FROM tags WHERE type=0");
    while (q.next()) {
//        qDebug() << "tag : " << q.value(0);
//        qDebug() << "tag : " << q.value(1);
//        qDebug() << "tag : " << q.value(2);
//        qDebug() << "tag : " << q.value(3);
        map_tags.insert(q.value(0).toInt(), q.value(1).toString());
        QString color = QString("#%1").arg(q.value(2).toString());
//        qDebug() << color << QColor(color);
        tags_colors.insert(q.value(0).toInt(), QColor(color));
    }

    QMap<int,QString> map_cats;
    QMap<int,QColor> cats_colors;
    q.exec("SELECT * FROM categories WHERE type=0");
    while (q.next()) {
        //        qDebug() << "cat : " << q.value(0);
        //        qDebug() << "cat " << q.value(1);
        map_cats.insert(q.value(0).toInt(), q.value(1).toString());
        QString color = QString("#%1").arg(q.value(2).toString());
        //        qDebug() << color << QColor(color);
        cats_colors.insert(q.value(0).toInt(), QColor(color));
    }

    for (QMap<int,QString>::const_iterator tag = map_tags.constBegin(); tag != map_tags.constEnd(); ++tag)
    {
        QPieSeries *series = new QPieSeries(ui->catsWidget);
        series->setName("Categories in " + tag.value());

        q.exec(QString("SELECT category, SUM (amount) FROM operations WHERE (tag=%1 AND type=0) GROUP BY category").arg(tag.key()));
//        qDebug() << "BOUCLE : " << tag.key() << " : " << tag.value();
        while (q.next()) {
//            qDebug() << q.value(0);
//            qDebug() << q.value(1);
            int id = q.value(0).toInt();
            *series << new DrilldownSlice(q.value(1).toDouble(), map_cats[id], cats_colors[id], tagsSeries);
        }

        QObject::connect(series, &QPieSeries::clicked, chart, &DrilldownChart::handleSliceClicked);

        *tagsSeries << new DrilldownSlice(series->sum(), tag.value(), cats_colors[tag.key()], series);
    }

    QObject::connect(tagsSeries, &QPieSeries::clicked, chart, &DrilldownChart::handleSliceClicked);

    chart->changeSeries(tagsSeries);
}

void Account::initTabFilters()
{
    QDate today = QDate::currentDate();
//    dateFrom = QDate(today.year(), today.month(), 1);
//    dateTo = QDate(today.year(), today.month(), today.daysInMonth());
    dateFrom = QDate(today.year(), 2, 1);
    dateTo = QDate(today.year(), 2, 28);
    ui->qdeFilterFrom->setDate(dateFrom);
    ui->qdeFilterTo->setDate(dateTo);

    ui->qcbFilterCat->setModel(model->relationModel(categoryIdx));
    ui->qcbFilterCat->setModelColumn(1);
    ui->qcbFilterTag->setModel(model->relationModel(tagIdx));
    ui->qcbFilterTag->setModelColumn(1);

    filterMapper = new QDataWidgetMapper(this);
    filterMapper->setModel(model);
    filterMapper->addMapping(ui->qcbFilterCat, 2, "currentIndex");
    filterMapper->addMapping(ui->qcbFilterTag, 4, "currentIndex");
    filterMapper->toFirst();

    setDateFilter();

    connect(ui->qgbFilterDate, SIGNAL(toggled(bool)), this, SLOT(activateDateFilter(bool)));
    connect(ui->qdeFilterFrom, SIGNAL(dateChanged(QDate)), this, SLOT(applyFromDateFilter(QDate)));
    connect(ui->qdeFilterTo, SIGNAL(dateChanged(QDate)), this, SLOT(applyToDateFilter(QDate)));

    connect(ui->qgbFilterCat, SIGNAL(toggled(bool)), this, SLOT(activateCatFilter(bool)));
    connect(ui->qcbFilterCat, SIGNAL(currentIndexChanged(int)), this, SLOT(applyCatFilter(int)));

    connect(ui->qgbFilterTag, SIGNAL(toggled(bool)), this, SLOT(activateTagFilter(bool)));
    connect(ui->qcbFilterTag, SIGNAL(currentIndexChanged(int)), this, SLOT(applyTagFilter(int)));
}

void Account::importFile()
{
    QString filename = QFileDialog::getOpenFileName(nullptr,tr("Importer fichier"), QDir::home().dirName(), tr("csv files (*.csv)"));
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
//        process_line(line);
    }
}

void Account::editOperation()
{
    int idx = (int) ui->opsView->currentIndex().row();

    AddOpDialog aoDiag(idx, model);
    aoDiag.setWindowTitle(tr("Editer une opération"));
    QString cat = model->record(idx).value(2).toString();
    QString tag = model->record(idx).value(4).toString();
    aoDiag.fillCategories(model->relationModel(categoryIdx), model->relationModel(categoryIdx)->fieldIndex("name"),cat);
    aoDiag.fillTags(model->relationModel(tagIdx), model->relationModel(tagIdx)->fieldIndex("name"),tag);

    aoDiag.exec();
    commitOnDatabase();
}

void Account::addOperation()
{
    AddOpDialog aoDiag;
    aoDiag.setWindowTitle(tr("Ajouter une opération"));
    aoDiag.fillCategories(model->relationModel(categoryIdx), model->relationModel(categoryIdx)->fieldIndex("name"));
    aoDiag.fillTags(model->relationModel(tagIdx), model->relationModel(tagIdx)->fieldIndex("name"));

    if (aoDiag.exec()) {
        ++_nbOperations;
        int row = 0;
        model->insertRows(row, 1);
        model->setData(model->index(row, 0), _nbOperations);
        model->setData(model->index(row, 1), aoDiag.date());
        model->setData(model->index(row, 2), aoDiag.category());
        model->setData(model->index(row, 3), aoDiag.amount());
        model->setData(model->index(row, 4), aoDiag.tag());
        model->setData(model->index(row, 5), aoDiag.description());
        commitOnDatabase();
    }
}

void Account::removeOperation()
{
    int ok = QMessageBox::warning(this,QStringLiteral("Supprimer les opérations sélectionnées"),
                                  QStringLiteral("Etes-vous sûr de vouloir supprimer les opérations sélectionnées?"),
                                  QMessageBox::Yes,QMessageBox::No);
    if(ok == QMessageBox::No)
        return;

    QItemSelectionModel *selectionModel = ui->opsView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();
    QModelIndex index;

    foreach (index, indexes) {
        int row = index.row();
        model->removeRows(row,1);
    }
    commitOnDatabase();
}

void Account::addCategory()
{
    addCatDialog acDiag;
    acDiag.setWindowTitle(tr("Ajouter une catégorie"));

    if (acDiag.exec())
    {
        model->database().transaction();

        QSqlQuery q;
        if (!q.prepare(INSERT_CATEGORY_SQL))
            qDebug() << q.lastError();

        addCategoryInDB(q, acDiag.title(), QLatin1String("CA6F1E"));
        model->database().commit();
    }
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

void Account::createToolBar()
{
    toolBar = new QToolBar(this);

    importOpAct = new QAction(tr("&Import..."), this);
    toolBar->addAction(importOpAct);
    connect(importOpAct, SIGNAL(triggered()),
            this, SLOT(importFile()));

    toolBar->addSeparator();

    addOpAct = new QAction(tr("&Add..."), this);
    toolBar->addAction(addOpAct);
    connect(addOpAct, SIGNAL(triggered()),
            this, SLOT(addOperation()));

    editOpAct = new QAction(tr("&Edit..."), this);
//    editOpAct->setEnabled(false);
    toolBar->addAction(editOpAct);
    connect(editOpAct, SIGNAL(triggered()),
            this, SLOT(editOperation()));

    removeOpAct = new QAction(tr("&Remove"), this);
    removeOpAct->setEnabled(false);
    toolBar->addAction(removeOpAct);
    connect(removeOpAct, SIGNAL(triggered()),
            this, SLOT(removeOperation()));

    toolBar->addSeparator();

    addCatAct = new QAction(tr("&Add Category..."), this);
    toolBar->addAction(addCatAct);
    connect(addCatAct, SIGNAL(triggered()),
            this, SLOT(addCategory()));

    manBudgetAct = new QAction(tr("&Budget..."), this);
    toolBar->addAction(manBudgetAct);
    //     connect(manBudgetAct, SIGNAL(triggered()),
    //         this, SLOT(manageBudget()));

    connect(ui->opsView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(update_actions(QItemSelection)));

}

void Account::update_actions(const QItemSelection& selected)
{
    QModelIndexList indexes = selected.indexes();

    if (!indexes.isEmpty()) {
        removeOpAct->setEnabled(true);
        if (indexes.size() == 4)
            editOpAct->setEnabled(true);
    } else {
        removeOpAct->setEnabled(false);
        editOpAct->setEnabled(false);
    }
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
    if (on)
    {
        dateFrom = ui->qdeFilterFrom->date();
        dateTo = ui->qdeFilterTo->date();
        setDateFilter();
    }
    else
    {
        model->setFilter("");
    }
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
    if (on)
    {
        int cat = ui->qcbFilterCat->currentIndex() + 1;
        model->setFilter(QString("category='%1'").arg(cat));
    }
    else
    {
        model->setFilter("");
//        qDebug() << model->filter();
    }
}

void Account::applyCatFilter(int cat)
{
    model->setFilter(QString("category='%1'").arg(cat+1));
}

void Account::activateTagFilter(bool on)
{
    if (on)
    {
        int tag = ui->qcbFilterTag->currentIndex() + 1;
        model->setFilter(QString("tag='%1'").arg(tag));
    }
    else
    {
        model->setFilter("");
    }
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

QSqlError Account::loadFile()
{
    QSqlError err;

    QString loadFilename = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                        "D:/sopie/Documents",
                                                        tr("BSX files (*.bsx)"));
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
        QSqlError err = readCategories(INSERT_CATEGORY_SQL, catsArray);
    }

    ///// Read Tags /////

    if (loadObject.contains("tags") && loadObject["tags"].isArray()) {
        q.exec("TRUNCATE TABLE tags");
        QJsonArray tagsArray = loadObject["tags"].toArray();
        QSqlError err = readCategories(INSERT_TAG_SQL, tagsArray);
    }

    ///// Read Operations /////
    if (loadObject.contains("operations") && loadObject["operations"].isArray()) {
        q.exec("TRUNCATE TABLE operations");
        QJsonArray opsArray = loadObject["operations"].toArray();
        QSqlError err = readOperations(opsArray);
    }

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
