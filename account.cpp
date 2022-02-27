#include "account.h"
#include "ui_account.h"

#include <QFileDialog>
#include <QDebug>

#include <addopdialog.h>
#include <addcatdialog.h>

Account::Account(QString title, QWidget *parent)
    : QWidget{parent}
    , ui(new Ui::Account)
    , _locale(QLocale::German)
    , _title(title)
{
    ui->setupUi(this);

    opsModel = new OperationsTableModel(this);
    ui->opsView->setModel(opsModel);
    ui->opsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->opsView->horizontalHeader()->setStretchLastSection(true);
    ui->opsView->verticalHeader()->hide();
    ui->opsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->opsView->setSelectionMode(QAbstractItemView::SingleSelection); // ExtendedSelection pour pouvoir supprimer plusieurs d'un coup

//    connect(ui->qpbImportFile, SIGNAL(clicked()), this, SLOT(importFile()));

    setStandardCategories();
    setStandardRules();

    catsPie = new CatsChart(&_opsCategories, ui->catsWidget);

    createToolBar();
//    show();
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

void Account::addOperation()
{
    AddOpDialog aoDiag(_opsCategories.keys());

    if (aoDiag.exec()) {
        QDate date = aoDiag.date();
        double amount = aoDiag.amount();
        QString cat = aoDiag.category();
        QString des = aoDiag.description();

        add_operation(date, des, amount, cat);
    }
}

void Account::editOperation()
{
    QItemSelectionModel *selectionModel = ui->opsView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();
    QModelIndex index, i;
    QDate date;
    double amount;
    QString cat;
    QString des;
    int row = -1;

    foreach (index, indexes) {
        row = index.row();
        i = opsModel->index(row, 0, QModelIndex());
        QVariant varDate = opsModel->data(i, Qt::DisplayRole);
        date = varDate.toDate();

        i = opsModel->index(row, 1, QModelIndex());
        QVariant varCat = opsModel->data(i, Qt::DisplayRole);
        cat = varCat.toString();

        i = opsModel->index(row, 2, QModelIndex());
        QVariant varAmount = opsModel->data(i, Qt::DisplayRole);
        amount = varAmount.toDouble();

        i = opsModel->index(row, 3, QModelIndex());
        QVariant varDes = opsModel->data(i, Qt::DisplayRole);
        des = varDes.toString();
    }

    AddOpDialog aoDialog(_opsCategories.keys());
    aoDialog.setWindowTitle(tr("Editer une opération"));

    aoDialog.setDate(date);
    aoDialog.setAmount(amount);
    aoDialog.setCategory(cat);
    aoDialog.setDescription(des);

    if (aoDialog.exec()) {
        QDate newDate = aoDialog.date();
        double newAmount = aoDialog.amount();
        QString newCat = aoDialog.category();
        QString newDes = aoDialog.description();


        i = opsModel->index(row, 0, QModelIndex());
        opsModel->setData(i, newDate, Qt::EditRole);

        i = opsModel->index(row, 1, QModelIndex());
        opsModel->setData(i, newCat, Qt::EditRole);

        i = opsModel->index(row, 2, QModelIndex());
        opsModel->setData(i, newAmount, Qt::EditRole);

        i = opsModel->index(row, 3, QModelIndex());
        opsModel->setData(i, newDes, Qt::EditRole);

        Categories::iterator it_cat = _opsCategories.find(cat);
        if (it_cat != _opsCategories.end()) {
            it_cat.value()->addOperation(-amount);
            catsPie->updateSlice(cat,it_cat.value()->amount());
        }

        it_cat = _opsCategories.find(newCat);
        if (it_cat != _opsCategories.end()) {
            it_cat.value()->addOperation(newAmount);
            catsPie->updateSlice(newCat,it_cat.value()->amount());
        }
    }
}

void Account::removeOperation()
{
    QItemSelectionModel *selectionModel = ui->opsView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();
    QModelIndex index;

    foreach (index, indexes) {
        int row = index.row();

        QModelIndex i = opsModel->index(row, 1, QModelIndex());
        QVariant varCat = opsModel->data(i, Qt::DisplayRole);
        QString cat = varCat.toString();

        i = opsModel->index(row, 2, QModelIndex());
        QVariant varAmount = opsModel->data(i, Qt::DisplayRole);
        double amount = varAmount.toDouble();

        Categories::iterator it_cat = _opsCategories.find(cat);
        if (it_cat != _opsCategories.end()) {
            it_cat.value()->addOperation(-amount);
            catsPie->updateSlice(cat,it_cat.value()->amount());
        }

        opsModel->removeRows(row, 1, QModelIndex());
    }
}

void Account::addCategory()
{
    addCatDialog acDiag;

    if (acDiag.exec()) {
        _opsCategories.insert(acDiag.title(), new Category(acDiag.type()));
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
        if (indexes.size() == 4)
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
    _opsCategories.insert("SUBSCRIPTIONS", new Category(Category::SPENDING));
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
    _rules.insert("BOUYGUES","SUBSCRIPTIONS");
    _rules.insert("Deezer","SUBSCRIPTIONS");
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
    editOpAct->setEnabled(false);
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

