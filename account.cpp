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

    QMap<int,QString> map_tags;
    QMap<int,QColor> tags_colors;
    QSqlQuery q("SELECT * FROM tags WHERE type=0");
    while (q.next()) {
        qDebug() << "tag : " << q.value(0);
        qDebug() << "tag : " << q.value(1);
        qDebug() << "tag : " << q.value(2);
        qDebug() << "tag : " << q.value(3);
        map_tags.insert(q.value(0).toInt(), q.value(1).toString());
        QString color = QString("#%1").arg(q.value(2).toString());
        qDebug() << color << QColor(color);
        tags_colors.insert(q.value(0).toInt(), QColor(color));
    }

    QMap<int,QString> map_cats;
    QMap<int,QColor> cats_colors;
    q.exec("SELECT * FROM categories WHERE type=0");
    while (q.next()) {
        qDebug() << "cat : " << q.value(0);
        qDebug() << "cat " << q.value(1);
        map_cats.insert(q.value(0).toInt(), q.value(1).toString());
        QString color = QString("#%1").arg(q.value(2).toString());
        qDebug() << color << QColor(color);
        cats_colors.insert(q.value(0).toInt(), QColor(color));
    }

    for (QMap<int,QString>::const_iterator tag = map_tags.constBegin(); tag != map_tags.constEnd(); ++tag)
    {
        QPieSeries *series = new QPieSeries(ui->catsWidget);
        series->setName("Categories in " + tag.value());

        q.exec(QString("SELECT category, SUM (amount) FROM operations WHERE (tag=%1 AND type=0) GROUP BY category").arg(tag.key()));
        qDebug() << "BOUCLE : " << tag.key() << " : " << tag.value();
        while (q.next()) {
            qDebug() << q.value(0);
            qDebug() << q.value(1);
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
        process_line(line);
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

}

void Account::setDateFilter()
{
    qDebug() << "From " << dateFrom.toString(Qt::ISODateWithMs) << " To " << dateTo.toString(Qt::ISODateWithMs);
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
        qDebug() << model->filter();
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
