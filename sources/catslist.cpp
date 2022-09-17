#include "catslist.h"
#include "ui_catslist.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include "catdialog.h"
#include "utilities.h"

using namespace Utilities;

GroupList::GroupList(GroupType type, QSqlTableModel * mod, QWidget *parent)
    : QDialog{parent},
      ui(new Ui::GroupList),
      model(mod),
      groupType(type)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(groupIconByType(type)));
    setWindowTitle(tr("Manage ") + groupTableByType(type));
    setWindowModality(Qt::WindowModal);

    ToolBar *toolBar = new ToolBar;
    connect(ui->catsView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), toolBar, SLOT(updateActions(QItemSelection)));
    connect(toolBar, SIGNAL(actTriggered(Action)), this, SLOT(applyAction(Action)));
    ui->toolLayout->addWidget(toolBar);

    QSqlQuery q(model->database());
    if (type == CatType)
        q.exec(SELECT_CATEGORIES);
    else
        q.exec(SELECT_TAGS);
    while (q.next())
        ui->catsView->addItem(q.value(1).toString());

    contextMenu = new QMenu(ui->catsView);
    QString editTitle = type == CatType ? tr("Editer une catégorie") : tr("Editer un tag");
    QString removeTitle = type == CatType ? tr("Supprimer une catégorie") : tr("Supprimer un tag");
    contextMenu->addAction(EDIT_ICON, editTitle, this, &GroupList::edit);
    contextMenu->addAction(REMOVE_ICON, removeTitle, this, &GroupList::remove);
    ui->catsView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->catsView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequested(QPoint)));
    ui->catsView->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void GroupList::customMenuRequested(QPoint pos){
    QModelIndex index=ui->catsView->indexAt(pos);
    if (index.isValid()) {
        contextMenu->popup(ui->catsView->viewport()->mapToGlobal(pos));
    }
}

void GroupList::applyAction(Action act)
{
    switch (act)
    {
        case AddAction:
            add();
            break;
        case EditAction:
            edit();
            break;
        case RemoveAction:
            remove();
            break;
        default:
            break;
    }
}

void GroupList::add()
{
    GroupDialog diag(groupType);
    if (diag.exec())
    {
        ui->catsView->addItem(diag.name());

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
    int row = ui->catsView->currentRow();
    // pour l'instant, l'index de la currentRow correspond à l'id de la categorie dans la database
    // mais ça va causer des problemes le jour ou je voudrais sort les combobox ou la qListWidget par ordre
    // alphabetique

    QSqlRecord current_record = model->record(row);
    GroupDialog diag(groupType);
    diag.setFields(current_record);
    if (diag.exec())
    {
        ui->catsView->currentItem()->setText(diag.name());
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
    QItemSelectionModel *selectionModel = ui->catsView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();
    QModelIndex index;

    foreach (index, indexes) {
        int row = index.row();
        model->removeRow(row);
        ui->catsView->model()->removeRow(row);
    }
    emit commit();
}


