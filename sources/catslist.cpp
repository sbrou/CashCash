#include "catslist.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include "catdialog.h"

GroupList::GroupList(Type type, QSqlTableModel * mod, QWidget *parent)
    : QDialog{parent}
    , model(mod)
{
    mainLayout = new QGridLayout(this);

    catsView = new QListWidget(this);
    qpbAddNew = new QPushButton(this);
    qpbEdit = new QPushButton(this);
    qpbRemove = new QPushButton(this);

    qpbAddNew->setIcon(QIcon(":/images/images/add_48px.png"));
    qpbEdit->setIcon(QIcon(":/images/images/edit_48px.png"));
    qpbRemove->setIcon(QIcon(":/images/images/Remove_48px.png"));

    QSqlQuery q(model->database());
    if (type == CAT)
        q.exec("SELECT * from categories");
    else
        q.exec("SELECT * from tags");
    while (q.next())
        catsView->addItem(q.value(1).toString());

    catsView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mainLayout->addWidget(catsView, 0, 0, 5, 3);

    connect(qpbAddNew, SIGNAL(clicked()), this, SLOT(add()));
    mainLayout->addWidget(qpbAddNew, 0, 3);

    connect(qpbEdit, SIGNAL(clicked()), this, SLOT(edit()));
    mainLayout->addWidget(qpbEdit, 1, 3);

    connect(qpbRemove, SIGNAL(clicked()), this, SLOT(remove()));
    mainLayout->addWidget(qpbRemove, 2, 3);
}

GroupList::~GroupList()
{
    catsView->deleteLater();
    qpbAddNew->deleteLater();
    qpbEdit->deleteLater();
    qpbRemove->deleteLater();
}

void GroupList::add()
{
    qDebug() << "new category";
    CatDialog diag;
    if (diag.exec())
    {
        catsView->addItem(diag.name());

        QSqlRecord new_record = model->record();

        new_record.setGenerated(0, false);
        new_record.setValue(1, QVariant(diag.name()));
        new_record.setValue(2, QVariant(diag.color()));
        new_record.setValue(3, QVariant(diag.type()));

        if (model->insertRecord(-1, new_record))
        {
            model->submitAll();
            emit commit();
        }
    }
    qDebug() << "end";
}

void GroupList::edit()
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

void GroupList::remove()
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

///// CatsList

CatsList::CatsList(QSqlTableModel * mod, QWidget *parent) : GroupList(Type::CAT, mod, parent)
{
    setWindowIcon(QIcon(":/images/images/category_48px.png"));
    setWindowTitle(tr("Manage categories"));

    qpbAddNew->setText(tr("New Category"));
    qpbEdit->setText(tr("Edit Category"));
    qpbRemove->setText(tr("Remove Category"));
}

///// TagsList

TagsList::TagsList(QSqlTableModel * mod, QWidget *parent) : GroupList(Type::TAG, mod, parent)
{
    setWindowIcon(QIcon(":/images/images/tag_window_48px.png"));
    setWindowTitle(tr("Manage tags"));

    qpbAddNew->setText(tr("New Tag"));
    qpbEdit->setText(tr("Edit Tag"));
    qpbRemove->setText(tr("Remove Tag"));
}



