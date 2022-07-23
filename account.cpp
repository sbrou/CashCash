#include "account.h"

#include <QFileDialog>
#include <QTemporaryFile>
#include <QProcess>
#include <QPushButton>
#include <QDomDocument>
#include <QDebug>

#include <QPieSeries>

#include "newaccountdialog.h"
#include <addopdialog.h>
#include "catslist.h"

#include "initdb.h"

Account::Account(QWidget *parent)
    : QSplitter{parent}
    , _locale(QLocale::German)
    , _title("")
    , _balance(0)
    , _future_balance(0)
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
        updateBalance();
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

QDomElement getElement(const QDomElement &parent, const QString& name)
{
    if (!parent.isNull())
    {
        QDomNodeList list = parent.elementsByTagName(name);
        if (list.isEmpty())
            return QDomElement();
        else
            return list.at(0).toElement();
    }

    return QDomElement();
}

void Account::importFile()
{
    QString filename = QFileDialog::getOpenFileName(nullptr,tr("Importer des operations"), QDir::home().dirName(), tr("ofx files (*.ofx)"));

    QFile inFile(filename);
    if(!inFile.open(QIODevice::ReadOnly)) {
        qDebug() << "can't open input file";
        return;
    }

    QString newFileData;
    QTextStream in(&inFile);

    //Read file line by line until it reaches the end
    while(!in.atEnd())
    {
        QString line = in.readLine();
        if (!line.contains("<OFX>"))
            continue;

        newFileData.append(line);
    }

    inFile.close();

    QFile outFile("trimmed_ofx.ofx");
    if(!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "can't open trimmed file";
        return;
    }

    //Write the data to the output file
    QTextStream out(&outFile);
    out << newFileData;
    outFile.close();

    QString program = "D:\\sopie\\Qt_Projects\\budget\\3rdparty\\OpenSP-1.5.2\\osx.exe";
    QStringList arguments;
    arguments << "-ldtd_file" <<  "D:\\sopie\\Qt_Projects\\budget\\3rdparty\\OpenSP-1.5.2\\ofx160.dtd" << outFile.fileName();

    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
    if (!myProcess->waitForFinished()) {
        qDebug() << "process not finished";
        qDebug() << myProcess->readAllStandardError();
        return;
    }

    QByteArray result = myProcess->readAllStandardOutput();
    QFile xmlFile("ops.ofx");
    if(!xmlFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "can't create xml file";
        return;
    }

    //Write the data to the output file
    QTextStream xml(&xmlFile);
    xml << result;
    xmlFile.close();

    QDomDocument doc("ops");
    if (!xmlFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "can't open xml file";
        return;
    }
    if (!doc.setContent(&xmlFile)) {
        xmlFile.close();
        qDebug() << "problem at reading";
        return;
    }
    xmlFile.close();

    QDomNodeList ofx = doc.elementsByTagName("OFX");
    if (!ofx.isEmpty())
    {
        qDebug() << "ofx is not empty";
        QDomElement ofx_elt = ofx.at(0).toElement();
        QDomElement BANKMSGSRSV1 = getElement(ofx_elt, "BANKMSGSRSV1");
        QDomElement STMTTRNRS = getElement(BANKMSGSRSV1, "STMTTRNRS");
        QDomElement STMTRS = getElement(STMTTRNRS, "STMTRS");
        QDomElement BANKTRANLIST = getElement(STMTRS, "BANKTRANLIST");
        QDomNodeList ops_list = BANKTRANLIST.elementsByTagName("STMTTRN");
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
                QDate date = QDate::fromString(date_elt.text(),"yyyyMMdd");

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
    outFile.remove();
    xmlFile.remove();
}

void Account::updateBalance()
{
    QSqlQuery query;
    query.exec(QString("SELECT SUM (amount) FROM operations WHERE op_date<='%2'").arg(QDate::currentDate().toString(Qt::ISODateWithMs)));
    while (query.next()) {
        _balance = query.value(0).toDouble();
    }

    query.exec("SELECT SUM (amount) FROM operations");
    while (query.next()) {
        _future_balance = query.value(0).toDouble();
    }
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

    ///// Write Account caracteristics /////

    accObject["title"] = _title;
    accObject["balance"] = _balance;
    accObject["future_balance"] = _future_balance;

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

    //    write(accObject);

    ///// Write Account caracteristics /////

    if (loadObject.contains("title") && loadObject["title"].isString())
        _title = loadDoc["title"].toString();

    if (loadObject.contains("balance") && loadObject["balance"].isDouble())
        _balance = loadObject["balance"].toDouble();

    if (loadObject.contains("future_balance") && loadObject["future_balance"].isDouble())
        _future_balance = loadObject["future_balance"].toDouble();

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

QSqlError Account::createFile()
{
    NewAccountDialog newAcc(this);

    if (newAcc.exec())
    {
        _title = newAcc.title();
        _balance = newAcc.balance();
        _future_balance = _balance;

        QSqlQuery q;

        if (!q.prepare(INSERT_CATEGORY_SQL))
            return q.lastError();

        addCategoryInDB(q, "-NONE-", "#000000" , -1);

        if (!q.prepare(INSERT_TAG_SQL))
            return q.lastError();

        addTagInDB(q, "-NONE-", "#000000" , -1);

        initAccount();
    }

    return QSqlError();
}
