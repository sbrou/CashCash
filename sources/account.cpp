#include "account.h"

#include <QFileDialog>
#include <QTemporaryFile>
#include <QProcess>
#include <QPushButton>
#include <QDomDocument>
#include <QDebug>

#include <QPieSeries>

#include "addopdialog.h"
#include "catslist.h"
#include "goaldialog.h"
#include "statswidget.h"
#include "csvimporterwizard.h"

#include "initdb.h"
#include "utilities.h"

using namespace Utilities;

Account::Account(QWidget *parent)
    : QSplitter{parent}
    , _locale(QLocale::German)
    , _title("")
    , _init_balance(0)
    , _balance(0)
    , _future_balance(0)
    , _filepath("")
    , _state(Empty)
    , _nbOperations(0)
{
    setOrientation(Qt::Vertical);  
    splitter = new QSplitter(Qt::Horizontal, this);
}

QSqlError Account::initDatabase()
{
    QSqlError err = initDb(_title);

    if (err.type() != QSqlError::NoError) {
        showError(err);
    }

    return err;
}

bool Account::commitOnDatabase()
{
    model->database().transaction();
    bool st = model->submitAll();
    if (st)
    {
        model->database().commit();
        model->select();
        updateBalance();
        chartView->updatePie();
        opsView->resizeView();
        goalsView->updateGoals();
        if (statsWidget->isVisible())
            statsWidget->populateTable();

        changeState(Modified);
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
    // Create the data model:
    opsView = new OperationsView;
    model = new QSqlRelationalTableModel(opsView->table(), QSqlDatabase::database(_title));
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("operations");
    model->setSort(1, Qt::DescendingOrder);

    // Set the relations to the other database tables:
    model->setRelation(CatIndex, QSqlRelation("categories", "id", "name"));
    model->setRelation(TagIndex, QSqlRelation("tags", "id", "name"));

    cats_model = model->relationModel(CatIndex);
    tags_model = model->relationModel(TagIndex);

    // Set the localized header captions:
    model->setHeaderData(CatIndex, Qt::Horizontal, tr("Category"));
    model->setHeaderData(TagIndex, Qt::Horizontal, tr("Tag"));
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

    opsView->loadModel(model);
    opsView->setBalance(_balance, _future_balance);
    connect(opsView->table()->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SIGNAL(selectionChanged(QItemSelection)));
    connect(opsView->table(), SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editOperation()));
    connect(this, SIGNAL(balanceChanged(float, float)), opsView, SLOT(setBalance(float, float)));
    splitter->addWidget(opsView);

    chartView = new ChartsView(model, this);
    splitter->addWidget(chartView);

    addWidget(splitter);

    goalsView = new GoalsView(_title, this);
    connect(goalsView, SIGNAL(changeState(AccountState)), this, SLOT(changeState(AccountState)));
    addWidget(goalsView);

    catsWidget = new CatsList(cats_model, this);
    connect(catsWidget, SIGNAL(commit()), this, SLOT(commitOnDatabase()));

    tagsWidget = new TagsList(tags_model, this);
    connect(tagsWidget, SIGNAL(commit()), this, SLOT(commitOnDatabase()));

    rulesWidget = new RulesList(cats_model, tags_model, this);
    connect(rulesWidget, SIGNAL(changeState(AccountState)), this, SLOT(changeState(AccountState)));

    statsWidget = new StatsWidget(_init_balance, _title);

    readSettings();

    changeState(Modified);
    emit accountReady(_title);
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

void Account::importCSV(const QString & filename)
{
    CSVImporterWizard csvWizard(model, filename);
    if (csvWizard.exec())
    {
        OperationsVector *ops = csvWizard.getOperations();

        QSqlQuery q(QSqlDatabase::database(_title));
        if (!q.prepare(INSERT_OPERATION_SQL))
        {
            qDebug() << q.lastError();
            return;
        }

        for (int r = 0; r < ops->size(); ++r )
        {
            Operation op = ops->at(r);
            rulesWidget->assignDescriptionToGroups(op.description, op.cat, op.tag);
            addOperationInDB(q, op.date, op.cat, op.amount, op.tag, op.description, (op.amount>0));
        }
    }
    commitOnDatabase();
}

void Account::importOFX(const QString & filename)
{
    QMessageBox qmbErr(QMessageBox::Critical, tr("Erreur"), QString(tr("Problème à l'importation du fichier") + " %1")
                       .arg(filename), QMessageBox::Ok, this);


    QFile inFile(filename);
    if(!inFile.open(QIODevice::ReadOnly)) {
        qmbErr.setDetailedText("can't open input file");
        qmbErr.exec();
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
        qmbErr.setDetailedText("can't open trimmed file");
        qmbErr.exec();
        return;
    }

    //Write the data to the output file
    QTextStream out(&outFile);
    out << newFileData;
    outFile.close();

    QFile osx_exe(QString("%1/extern/osx.exe").arg(QCoreApplication::applicationDirPath()));
    QFile dtd_file(QString("%1/extern/ofx160.dtd").arg(QCoreApplication::applicationDirPath()));
    if (!osx_exe.exists() || !dtd_file.exists()) {
        QString details;
        if (!osx_exe.exists()) details += QString("can't find the file %1 \n").arg(osx_exe.fileName());
        if (!dtd_file.exists()) details += QString("can't find the file %1 \n").arg(dtd_file.fileName());
        qmbErr.setDetailedText(details);
        qmbErr.exec();
        return;
    }

    QString program = osx_exe.fileName();
    QStringList arguments;
    arguments << "-ldtd_file" <<  dtd_file.fileName() << outFile.fileName();

    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
    if (!myProcess->waitForFinished()) {
        qmbErr.setDetailedText("process not finished : \n" + myProcess->readAllStandardError());
        qmbErr.exec();
        return;
    }

    QByteArray result = myProcess->readAllStandardOutput();
    QFile xmlFile("ops.ofx");
    if(!xmlFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qmbErr.setDetailedText("can't create xml file");
        qmbErr.exec();
        return;
    }

    //Write the data to the output file
    QTextStream xml(&xmlFile);
    xml << result;
    xmlFile.close();

    QDomDocument doc("ops");
    if (!xmlFile.open(QIODevice::ReadOnly))
    {
        qmbErr.setDetailedText("can't open xml file");
        qmbErr.exec();
        return;
    }
    if (!doc.setContent(&xmlFile)) {
        xmlFile.close();
        qmbErr.setDetailedText("problem at reading");
        qmbErr.exec();
        return;
    }
    xmlFile.close();

    QDomNodeList ofx = doc.elementsByTagName("OFX");
    if (!ofx.isEmpty())
    {
        QDomElement ofx_elt = ofx.at(0).toElement();
        QDomElement BANKMSGSRSV1 = getElement(ofx_elt, "BANKMSGSRSV1");
        QDomElement STMTTRNRS = getElement(BANKMSGSRSV1, "STMTTRNRS");
        QDomElement STMTRS = getElement(STMTTRNRS, "STMTRS");
        QDomElement BANKTRANLIST = getElement(STMTRS, "BANKTRANLIST");
        QDomNodeList ops_list = BANKTRANLIST.elementsByTagName("STMTTRN");
        if (!ops_list.isEmpty())
        {
            QSqlQuery q(QSqlDatabase::database(_title));
            if (!q.prepare(INSERT_OPERATION_SQL))
            {
                qmbErr.setDetailedText(q.lastError().text());
                qmbErr.exec();
                return;
            }

            for (int i = 0; i < ops_list.count(); ++i)
            {
                QDomElement op = ops_list.at(i).toElement();

                QDomElement date_elt = op.elementsByTagName("DTPOSTED").at(0).toElement();
                QDate date = QDate::fromString(date_elt.text(),"yyyyMMdd");

                QDomElement amt_elt = op.elementsByTagName("TRNAMT").at(0).toElement();
                float amount = amt_elt.text().toFloat();

                QDomNodeList des_node = op.elementsByTagName("NAME");
                QString description;
                if (!des_node.isEmpty())
                    description = des_node.at(0).toElement().text();
                else
                    description = op.elementsByTagName("PAYEE").at(0).toElement().text();

                int cat, tag;
                rulesWidget->assignDescriptionToGroups(description, cat, tag);
                addOperationInDB(q, date, cat, amount, tag, description, (amount>0));

            }
        }
    }
    commitOnDatabase();
    outFile.remove();
    xmlFile.remove();
}

void Account::importFile()
{
    QString filename = QFileDialog::getOpenFileName(nullptr,tr("Importer des operations"), QDir::home().dirName(),
                                                    tr("Fichiers (*.ofx *.csv)"));
    if (filename.endsWith(".csv"))
        importCSV(filename);
    else if (filename.endsWith(".ofx"))
        importOFX(filename);
}

void Account::updateBalance()
{
    QSqlQuery query(QSqlDatabase::database(_title));
    QueryStatement statement(SELECT_SUM, upperDateCondition(QDate::currentDate()));
    query.exec(statement.get());
    while (query.next()) {
        _balance = _init_balance + query.value(0).toFloat();
    }

    statement.clearConditions();
    query.exec(statement.get());
    while (query.next()) {
        _future_balance = _init_balance + query.value(0).toFloat();
    }

    emit balanceChanged(_balance, _future_balance);
}

void Account::editOperation()
{
    int idx = (int) opsView->table()->currentIndex().row();

    AddOpDialog aoDiag(idx, model);
    aoDiag.setWindowTitle(tr("Edit an operation"));
    aoDiag.setWindowIcon(EDIT_ICON);

    if (aoDiag.exec())
        commitOnDatabase();
}

void Account::addOperation()
{
    AddOpDialog aoDiag(cats_model, tags_model);
    aoDiag.setWindowTitle(tr("Add an operation"));
    aoDiag.setWindowIcon(ADD_ICON);
    if (aoDiag.exec()) {

        QSqlRecord new_record = model->record();
        new_record.setValue(DateIndex, QVariant(aoDiag.date()));
        new_record.setValue(CatIndex, QVariant(aoDiag.category()));
        new_record.setValue(AmountIndex, QVariant(aoDiag.amount()));
        new_record.setValue(TagIndex, QVariant(aoDiag.tag()));
        new_record.setValue(DesIndex, QVariant(aoDiag.description()));
        if (aoDiag.amount() < 0)
            new_record.setValue(OpTypeIndex, QVariant(0));
        else
            new_record.setValue(OpTypeIndex, QVariant(1));

        if (model->insertRecord(-1, new_record))
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

    QItemSelectionModel *selectionModel = opsView->table()->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();
    QModelIndex index;

    foreach (index, indexes) {
        int row = index.row();
        model->removeRows(row,1);
    }
    commitOnDatabase();
}

AccountState Account::state()
{
    return _state;
}

QString Account::title()
{
    return _title;
}

void Account::changeState(AccountState newState)
{
    _state = newState;
    emit stateChanged( _state == Modified);
}

void Account::saveFile(bool isNewFile)
{
    QString saveFilename = _filepath;

    if (_filepath.isEmpty() || isNewFile)
    {
        QString title = isNewFile ? tr("Save file as") : tr("Save file");
        saveFilename = QFileDialog::getSaveFileName(this, title, "D:/sopie/Documents/untitled.bsx",
                                                    tr("BSX files (*.bsx)"));

        if (saveFilename.isEmpty())
            return;
        else
            _filepath = saveFilename;
    }

    QFile saveFile(saveFilename);

    commitOnDatabase();

    if (!saveFile.open(QIODevice::WriteOnly)) {
            qWarning("Couldn't open save file.");
            return;
        }

    QJsonObject accObject;
    QSqlQuery q(QSqlDatabase::database(_title));

    ///// Write Account caracteristics /////

    accObject["title"] = _title;
    accObject["init_balance"] = _init_balance;

    ///// Write Cats /////

    QJsonArray catsArray;

    q.exec(SELECT_CATEGORIES);
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

    q.exec(SELECT_TAGS);
    while (q.next()) {
        QJsonObject tag;
        tag["name"] = q.value(1).toString();
        tag["color"] = q.value(2).toString();
        tag["type"] = q.value(3).toInt();

        tagsArray.append(tag);
    }
    accObject["tags"] = tagsArray;

    ///// Write Goals /////

    QJsonArray goalsArray;
    QStandardItemModel* goals = goalsView->model();
    for (int row = 0; row < goals->rowCount(); ++row) {
        Goal goal = goals->item(row,0)->data(Qt::UserRole).value<Goal>();
        QJsonObject goalObj;
        goalObj["group_type"] = goal.type;
        goalObj["type_id"] = goal.typeId;
        goalObj["amount"] = goal.max;

        goalsArray.append(goalObj);
    }
    accObject["goals"] = goalsArray;

    ///// Write Operations /////

    QJsonArray opsArray;

    q.exec(SELECT_OPERATIONS);
    while (q.next()) {
        QJsonObject op;
        op["date"] = q.value(DateIndex).toString();
        op["category"] = q.value(CatIndex).toInt();
        op["amount"] = q.value(AmountIndex).toFloat();
        op["tag"] = q.value(TagIndex).toInt();
        op["description"] = q.value(DesIndex).toString();
        op["type"] = q.value(OpTypeIndex).toInt();

        opsArray.append(op);
    }
    accObject["operations"] = opsArray;

    ///// Write Rules /////

    QJsonArray rulesArray;
    QStandardItemModel* rules = rulesWidget->model();
    for (int row = 0; row < rules->rowCount(); ++row) {
        QJsonObject ruleObj;
        ruleObj["expression"] = rules->item(row,0)->data(Qt::DisplayRole).toString();
        ruleObj["case_sensitive"] = rules->item(row,0)->data(Qt::UserRole).toBool();
        ruleObj["cat_id"] = rules->item(row,1)->data(Qt::UserRole).toInt();
        ruleObj["tag_id"] = rules->item(row,2)->data(Qt::UserRole).toInt();

        rulesArray.append(ruleObj);
    }
    accObject["rules"] = rulesArray;

    saveFile.write(QCborValue::fromJsonValue(accObject).toCbor());
    changeState(UpToDate);
}

void Account::saveAsFile()
{
    saveFile(true);
}

void Account::saveSettings()
{
    QSettings settings;
    settings.setValue("subSplitterSizes", splitter->saveState());
    settings.setValue("mainSplitterSizes", saveState());
    settings.setValue("lastFile", QVariant(_filepath));
    opsView->saveSettings();
}

void Account::readSettings()
{
    QSettings settings;
    restoreState(settings.value("mainSplitterSizes").toByteArray());
    splitter->restoreState(settings.value("subSplitterSizes").toByteArray());
}

QSqlError Account::loadFile(const QString& filename)
{
    QSqlError err;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return QSqlError("Couldn't open save file.","",QSqlError::UnknownError);
    }

    QByteArray saveData = file.readAll();

    QJsonDocument loadDoc(QJsonDocument(QCborValue::fromCbor(saveData).toMap().toJsonObject()));
    QJsonObject loadObject = loadDoc.object();

    ///// Read Account caracteristics /////

    if (loadObject.contains("title") && loadObject["title"].isString())
        _title = loadDoc["title"].toString();

    if (loadObject.contains("init_balance") && loadObject["init_balance"].isDouble())
        _init_balance = loadObject["init_balance"].toDouble();

    initDatabase();

    QSqlQuery q(QSqlDatabase::database(_title));

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

    file.close();
    _filepath = filename;
    updateBalance();
    initAccount();

    ///// Read Goals after goalsView has been created /////
    if (loadObject.contains("goals") && loadObject["goals"].isArray()) {
        readGoals(loadObject["goals"].toArray());
    }

    ///// Read rules after rulesWidget has been created /////
    if (loadObject.contains("rules") && loadObject["rules"].isArray()) {
        readRules(loadObject["rules"].toArray());
    }

    QSettings().setValue("lastFile", QVariant(_filepath));
    changeState(UpToDate);

    return err;
}

QSqlError Account::readCategories(const QString& query, const QJsonArray &catsArray)
{
    QSqlQuery q(QSqlDatabase::database(_title));
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

        addGroupInDB(q, name, color, type);
    }

    return q.lastError();
}

QSqlError Account::readOperations(const QJsonArray &opsArray)
{
    QSqlQuery q(QSqlDatabase::database(_title));
    if (!q.prepare(INSERT_OPERATION_SQL))
        return q.lastError();

    for (int opIndex = 0; opIndex < opsArray.size(); ++opIndex)
    {
        QJsonObject opObject = opsArray[opIndex].toObject();

        QDate date;
        int category;
        float amount;
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

void Account::readGoals(const QJsonArray &goalsArray)
{
    for (int goalIdx = 0; goalIdx < goalsArray.size(); ++goalIdx) {
        QJsonObject goalObj = goalsArray[goalIdx].toObject();

        Goal goal;

        if (goalObj.contains("group_type") && goalObj["group_type"].isDouble())
            goal.type = (GroupType) goalObj["group_type"].toInt();
        else
            continue;

        if (goalObj.contains("type_id") && goalObj["type_id"].isDouble())
            goal.typeId = goalObj["type_id"].toInt();
        else
            continue;

        if (goalObj.contains("amount") && goalObj["amount"].isDouble())
            goal.max = goalObj["amount"].toDouble();
        else
            continue;

        goalsView->addGoal(goal);
    }
}

void Account::readRules(const QJsonArray &rulesArray)
{
    for (int ruleIdx = 0; ruleIdx < rulesArray.size(); ++ruleIdx) {
        QJsonObject ruleObj = rulesArray[ruleIdx].toObject();

        QString expr;
        bool isCaseSensitive;
        int catId;
        int tagId;

        if (ruleObj.contains("expression") && ruleObj["expression"].isString())
            expr = ruleObj["expression"].toString();
        else
            continue;

        if (ruleObj.contains("case_sensitive") && ruleObj["case_sensitive"].isBool())
            isCaseSensitive = ruleObj["case_sensitive"].toBool();
        else
            continue;

        if (ruleObj.contains("cat_id") && ruleObj["cat_id"].isDouble())
            catId = ruleObj["cat_id"].toInt();
        else
            continue;

        if (ruleObj.contains("tag_id") && ruleObj["tag_id"].isDouble())
            tagId = ruleObj["tag_id"].toInt();
        else
            continue;

        rulesWidget->createRule(_title, expr, isCaseSensitive, catId, tagId);
    }
}

void Account::showCategories()
{
    catsWidget->show();
}

void Account::showTags()
{
    tagsWidget->show();
}

void Account::showStats()
{
    statsWidget->populateTable();
    statsWidget->show();
}

void Account::manageGoals()
{
    GoalDialog diag(cats_model, tags_model, this);
    if (diag.exec()) {
        goalsView->addGoal(diag.goal());
    }
}

void Account::manageRules()
{
    rulesWidget->show();
}

QSqlError Account::createFile(const QString & title, float balance)
{
    _title = title;
    _init_balance = balance;
    _balance = _init_balance;
    _future_balance = _init_balance;

    QSqlError err = initDatabase();
    if (err.isValid())
        return err;

    // initialize categories
    err = setStandardCategories();
    if (err.isValid())
        return err;

    // initialize tags
    QSqlQuery q(QSqlDatabase::database(_title));
    if (!q.prepare(INSERT_TAG_SQL))
        return q.lastError();

    addGroupInDB(q, tr("-AUCUN-"), "#000000" , Expense);

    // initialize account
    initAccount();


    return QSqlError();
}

QSqlError Account::setStandardCategories()
{
    QSqlQuery q(QSqlDatabase::database(_title));

    if (!q.prepare(INSERT_CATEGORY_SQL))
        return q.lastError();

    QStringList colors = QColor::colorNames();

    // Depenses

    addGroupInDB(q, tr("-AUCUN-"), "#000000" , Expense);

    int i = 0;
    addGroupInDB(q, tr("Abonnements"), colors[i++], Expense);
    addGroupInDB(q, tr("Alimentation"), colors[i++], Expense);
    addGroupInDB(q, tr("Animaux"), colors[i++], Expense);
    addGroupInDB(q, tr("Auto/Moto"), colors[i++], Expense);
    addGroupInDB(q, tr("Bien-Etre/Soins"), colors[i++], Expense);
    addGroupInDB(q, tr("Divers"), colors[i++], Expense);
    addGroupInDB(q, tr("Dons/Cadeaux"), colors[i++], Expense);
    addGroupInDB(q, tr("Electronique/Informatique"), colors[i++], Expense);
    addGroupInDB(q, tr("Epargne"), colors[i++], Expense);
    addGroupInDB(q, tr("Frais"), colors[i++], Expense);
    addGroupInDB(q, tr("Habillement"), colors[i++], Expense);
    addGroupInDB(q, tr("Impôts/Taxes"), colors[i++], Expense);
    addGroupInDB(q, tr("Logement"), colors[i++], Expense);
    addGroupInDB(q, tr("Loisirs/Culture/Sport"), colors[i++], Expense);
    addGroupInDB(q, tr("Santé"), colors[i++], Expense);
    addGroupInDB(q, tr("Transport"), colors[i++], Expense);
    addGroupInDB(q, tr("Vacances"), colors[i++], Expense);

    // Revenus

    addGroupInDB(q, tr("Autres Revenus"), colors[i++], Earning);
    addGroupInDB(q, tr("Indemnités"), colors[i++], Earning);
    addGroupInDB(q, tr("Placements"), colors[i++], Earning);
    addGroupInDB(q, tr("Traitements/Salaires"), colors[i++], Earning);
    addGroupInDB(q, tr("Remboursements"), colors[i++], Earning);

    return QSqlError();
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
