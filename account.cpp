#include "account.h"
#include "ui_account.h"

#include <QFileDialog>
#include <QPushButton>
#include <QDebug>

#include <addopdialog.h>
#include <addcatdialog.h>

#include "initdb.h"

Account::Account(QString title, QWidget *parent)
    : QWidget{parent}
    , ui(new Ui::Account)
    , _locale(QLocale::German)
    , _title(title)
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

        // Set the model and hide the ID column:
        ui->opsView->setModel(model);
        ui->opsView->setItemDelegate(new QSqlRelationalDelegate(ui->opsView));
//        ui->opsView->setColumnHidden(model->fieldIndex("id"), true);
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

//    catsPie = new CatsChart(&_opsCategories, ui->catsWidget);

    chart = new DrilldownChart();
    chart->setTheme(QChart::ChartThemeLight);
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    QPieSeries *yearSeries = new QPieSeries(ui->catsWidget);
    yearSeries->setName("Sales by year - All");

    const QStringList months = {
            "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
        };
        const QStringList names = {
            "Jane", "John", "Axel", "Mary", "Susan", "Bob"
        };

        for (const QString &name : names) {
            QPieSeries *series = new QPieSeries(ui->catsWidget);
            series->setName("Sales by month - " + name);

            for (const QString &month : months)
                *series << new DrilldownSlice(QRandomGenerator::global()->bounded(1000), month, yearSeries);

            QObject::connect(series, &QPieSeries::clicked, chart, &DrilldownChart::handleSliceClicked);

            *yearSeries << new DrilldownSlice(series->sum(), name, series);
        }

        QObject::connect(yearSeries, &QPieSeries::clicked, chart, &DrilldownChart::handleSliceClicked);

        chart->changeSeries(yearSeries);

        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);


    chartView = new QChartView(chart, ui->catsWidget);
    chartView->setMinimumSize(380,260);
    createToolBar();
}

void Account::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                    "Error initializing database: " + err.text());
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
        process_line(line);
    }

//    for (auto op = ops.begin(); op!= ops.end(); ++op)
//        addOperation(op->date(), op->category(), op->amount(), op->description());
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
}

void Account::addOperation()
{

    AddOpDialog aoDiag;
    aoDiag.setWindowTitle(tr("Ajouter une opération"));
    aoDiag.fillCategories(model->relationModel(categoryIdx), model->relationModel(categoryIdx)->fieldIndex("name"));
    aoDiag.fillTags(model->relationModel(tagIdx), model->relationModel(tagIdx)->fieldIndex("name"));

    if (aoDiag.exec()) {
        int row = 0;
        bool st = model->insertRows(row, 1);
        qDebug() << st;
        st = model->setData(model->index(row, 0), model->rowCount());
        qDebug() << model->rowCount() << st;
        st = model->setData(model->index(row, 1), aoDiag.date());
        qDebug() << aoDiag.date() << st;
        st = model->setData(model->index(row, 2), aoDiag.category());
        qDebug() << aoDiag.category() << st;
        st = model->setData(model->index(row, 3), aoDiag.amount());
        qDebug() << aoDiag.amount() << st;
        st = model->setData(model->index(row, 4), aoDiag.tag());
        qDebug() << aoDiag.tag() << st;
        st = model->setData(model->index(row, 5), aoDiag.description());
        qDebug() << aoDiag.description() << st;
        st = model->submitAll();
        qDebug() << st;
    }
//        Categories::iterator it_cat = _opsCategories.find(cat);
//        if (it_cat != _opsCategories.end()) {
//            it_cat.value()->addOperation(-amount);
//            catsPie->updateSlice(cat,it_cat.value()->amount());
//        }

//        it_cat = _opsCategories.find(newCat);
//        if (it_cat != _opsCategories.end()) {
//            it_cat.value()->addOperation(newAmount);
//            catsPie->updateSlice(newCat,it_cat.value()->amount());
//        }
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
    model->submitAll();
}

void Account::addCategory()
{
    addCatDialog acDiag;
    acDiag.setWindowTitle(tr("Ajouter une catégorie"));

    if (acDiag.exec()) {
//        int row = 0;
//        bool st = cat_model->insertRows(row, 1);
//        qDebug() << st;
//        st = cat_model->setData(model->index(row, 0), cat_model->rowCount());
//        qDebug() << cat_model->rowCount() << st;
//        st = cat_model->setData(model->index(row, 1), acDiag.title());
//        qDebug() << acDiag.title() << st;
//        st = cat_model->submitAll();
//        qDebug() << st;


//        QSqlQuery q(model->database());
//        if (!q.prepare(INSERT_CATEGORY_SQL))
//            QMessageBox::critical(this,QStringLiteral("Erreur"),
//                                              q.lastError().text());
//        QVariant newId = addCategoryInDB(q, acDiag.title());
//        qDebug() << newId;
//        bool st = model->submitAll();
//        qDebug() << st;
//        model->setRelation(categoryIdx, QSqlRelation("categories", "id", "name"));
//        model->select();

    }
}

void Account::process_line(QString line)
{
    QStringList infos = line.split(QLatin1Char(';'));

    QStringList date = infos.at(0).split(QLatin1Char('/'));
    QDate op_date(date.at(2).toInt(),date.at(1).toInt(),date.at(0).toInt());
    QString cat = affect_category(infos.at(1),_locale.toDouble(infos.at(2)));

    // Reflechir a comment proprement appeler addOperation en dehors de la lecture du fichier
    add_operation(op_date, infos.at(1), _locale.toDouble(infos.at(2)), cat);
}

QString Account::affect_category(const QString &des, double amount)
{
    for (auto rule = _rules.cbegin(); rule != _rules.cend(); ++rule)
    {
        if (des.contains(rule.key())) {
            Categories::iterator it_cat = _opsCategories.find(rule.value());
            if (it_cat != _opsCategories.end()) {
                it_cat.value()->addOperation(amount);
                return it_cat.key();
            }
        }
    }
    return "-NONE-";
}

void Account::add_operation(QDate date, const QString &des, double amount, const QString &cat)
{
    int pos = opsModel->rowCount();
    opsModel->insertRows(pos, 1, QModelIndex());

    QModelIndex index = opsModel->index(pos, 0, QModelIndex());
    opsModel->setData(index, date, Qt::EditRole);
    index = opsModel->index(pos, 1, QModelIndex());
    opsModel->setData(index, cat, Qt::EditRole);
    index = opsModel->index(pos, 2, QModelIndex());
    opsModel->setData(index, amount, Qt::EditRole);
    index = opsModel->index(pos, 3, QModelIndex());
    opsModel->setData(index, des, Qt::EditRole);

    catsPie->updateSlice(cat,amount);
}

void Account::update_actions(const QItemSelection& selected)
{
    QModelIndexList indexes = selected.indexes();

    if (!indexes.isEmpty()) {
        removeOpAct->setEnabled(true);
        if (indexes.size() == model->columnCount())
            editOpAct->setEnabled(true);
    } else {
        removeOpAct->setEnabled(false);
        editOpAct->setEnabled(false);
    }
}

void Account::setStandardCategories()
{
    _opsCategories.insert("-NONE-", new Category(Category::SPENDING));
    _opsCategories.insert("FOOD", new Category(Category::SPENDING));
    _opsCategories.insert("HOUSE", new Category(Category::SPENDING));
    _opsCategories.insert("HEALTH", new Category(Category::SPENDING));
    _opsCategories.insert("HOBBIES", new Category(Category::SPENDING));
    _opsCategories.insert("MAIKO", new Category(Category::SPENDING));
    _opsCategories.insert("JOINT", new Category(Category::SPENDING));
    _opsCategories.insert("TRANSPORT", new Category(Category::SPENDING));
    _opsCategories.insert("SAVING", new Category(Category::SAVING));
    _opsCategories.insert("SUBS", new Category(Category::SPENDING));
    _opsCategories.insert("SALARY", new Category(Category::INCOME));
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

void Account::selectTest()
{
//    qDebug() << model->filter();
//    model->setFilter(QString("category='1'"));
//    QSqlQuery query = model->query();
//    while (query.next()) {
//        qDebug() << query.value(0);
//    }

    double total = 0.;

    QSqlQuery query("SELECT * FROM operations WHERE category=4",model->database());
    qDebug() << query.isActive();
    while (query.next()) {
        qDebug() << query.value(5).toString();
        total += query.value(3).toDouble();
    }

    qDebug() << "total = " << total;
}
