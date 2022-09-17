#include "catslist.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include "catdialog.h"
#include "utilities.h"

using namespace Utilities;

GroupList::GroupList(GroupType type, QSqlTableModel * mod, QWidget *parent)
    : QDialog{parent}
    , model(mod)
{
    setWindowIcon(QIcon(groupIconByType(type)));
    mainLayout = new QGridLayout(this);

    catsView = new QListWidget(this);
    qpbAddNew = new QPushButton(this);
    qpbEdit = new QPushButton(this);
    qpbRemove = new QPushButton(this);

    qpbAddNew->setIcon(ADD_ICON);
    qpbEdit->setIcon(EDIT_ICON);
    qpbRemove->setIcon(REMOVE_ICON);

    QSqlQuery q(model->database());
    if (type == CatType)
        q.exec(SELECT_CATEGORIES);
    else
        q.exec(SELECT_TAGS);
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
}

void GroupList::edit()
{
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
        model->removeRow(row);
    }
    emit commit();
}

///// CatsList

CatsList::CatsList(QSqlTableModel * mod, QWidget *parent) : GroupList(CatType, mod, parent)
{
    setWindowTitle(tr("Manage categories"));
    qpbAddNew->setText(tr("New Category"));
    qpbEdit->setText(tr("Edit Category"));
    qpbRemove->setText(tr("Remove Category"));
}

///// TagsList

TagsList::TagsList(QSqlTableModel * mod, QWidget *parent) : GroupList(TagType, mod, parent)
{
    setWindowTitle(tr("Manage tags"));
    qpbAddNew->setText(tr("New Tag"));
    qpbEdit->setText(tr("Edit Tag"));
    qpbRemove->setText(tr("Remove Tag"));
}



