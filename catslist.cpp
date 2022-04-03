#include "catslist.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include "catdialog.h"

CatsList::CatsList(QSqlTableModel * mod, QWidget *parent)
    : QDialog{parent}
    , model(mod)
{
    setWindowIcon(QIcon(":/images/images/category_48px.png"));
    setWindowTitle(tr("Gérer les catégories"));

    mainLayout = new QGridLayout(this);

    catsView = new QListWidget(this);

    QSqlQuery q;
    q.exec("SELECT * from categories");
    while (q.next())
        catsView->addItem(q.value(1).toString());

//    catsView->setSortingEnabled(true);
    catsView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mainLayout->addWidget(catsView, 0, 0, 5, 3);

    qpbAddNew = new QPushButton(QIcon(":/images/images/add_48px.png"),tr("New Category"),this);
    connect(qpbAddNew, SIGNAL(clicked()), this, SLOT(addNewCategory()));
    mainLayout->addWidget(qpbAddNew, 0, 3);

    qpbEdit = new QPushButton(QIcon(":/images/images/edit_48px.png"),tr("Edit Category"),this);
    connect(qpbEdit, SIGNAL(clicked()), this, SLOT(editCategory()));
    mainLayout->addWidget(qpbEdit, 1, 3);

    qpbRemove = new QPushButton(QIcon(":/images/images/Remove_48px.png"),tr("Remove Category"),this);
    connect(qpbRemove, SIGNAL(clicked()), this, SLOT(removeCategories()));
    mainLayout->addWidget(qpbRemove, 2, 3);
}

CatsList::~CatsList()
{
    catsView->deleteLater();
    qpbAddNew->deleteLater();
    qpbEdit->deleteLater();
    qpbRemove->deleteLater();
}

void CatsList::addNewCategory()
{
    qDebug() << "new category";
    CatDialog diag;
    if (diag.exec())
    {
        catsView->addItem(diag.name());

//        qDebug() << model->rowCount();

        QSqlRecord new_record = model->record();
//        qDebug() << new_record;

        new_record.setGenerated(0, false);
        new_record.setValue(1, QVariant(diag.name()));
        new_record.setValue(2, QVariant(diag.color()));
        new_record.setValue(3, QVariant(diag.type()));

        if (model->insertRecord(-1, new_record))
        {
            model->submitAll();
            emit commit();
        }

//        for (int i=0; i<6; ++i)
//        {
//            qDebug() <<  model->record(i);

//        }
//        qDebug() << model->rowCount();
    }
    qDebug() << "end";
}

void CatsList::editCategory()
{
    qDebug() << "current row is : " << catsView->currentRow();
    int row = catsView->currentRow();
    // pour l'instant, l'index de la currentRow correspond à l'id de la categorie dans la database
    // mais ça va causer des problemes le jour ou je voudrai sort les combobox ou la qListWidget par ordre
    // alphabetique

    QSqlRecord current_record = model->record(row);
    CatDialog diag;
    diag.setFields(current_record);
    if (diag.exec())
    {
        catsView->currentItem()->setText(diag.name());
        current_record.setValue(1, QVariant(diag.name()));
        current_record.setValue(2, QVariant(diag.color()));
        current_record.setValue(3, QVariant(diag.type()));
        if (model->setRecord(row, current_record))
        {
            model->submitAll();
            emit commit();

        }
    }
}

void CatsList::removeCategories()
{
    // Reflechir a comment gerer les operations a qui sont assignees ces categories
    QItemSelectionModel *selectionModel = catsView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();
    QModelIndex index;

    foreach (index, indexes) {
        int row = index.row();
        qDebug() << row;
        model->removeRow(row);
    }
    emit commit();
}
