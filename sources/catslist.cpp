#include "catslist.h"
#include "ui_catslist.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QMessageBox>

#include "catdialog.h"
#include "utilities.h"

using namespace Utilities;

GroupList::GroupList(const QString & database, GroupType type, QSqlTableModel * mod, QWidget *parent)
    : QDialog{parent},
      ui(new Ui::GroupList),
      model(mod),
      groupType(type),
      databaseName(database)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(groupIcon(type)));
    setWindowTitle(tr("Manage ") + groupTable(type));
    setWindowModality(Qt::WindowModal);

    toolBar = new ToolBar;
    connect(toolBar, SIGNAL(actTriggered(Action)), this, SLOT(applyAction(Action)));
    ui->toolLayout->addWidget(toolBar);

    ui->catsView->setModel(model);
    ui->catsView->setModelColumn(1);
    connect(ui->catsView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(updateActions(QItemSelection)));

    contextMenu = new QMenu(ui->catsView);
    QString editTitle = type == CatType ? tr("Editer une catégorie") : tr("Editer un tag");
    contextMenu->addAction(EDIT_ICON, editTitle, this, &GroupList::edit);
    contextMenu->addAction(REMOVE_ICON, removeAGroup(type), this, &GroupList::remove);
    ui->catsView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->catsView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequested(QPoint)));
    ui->catsView->setSelectionMode(QAbstractItemView::SingleSelection);
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
    int row = ui->catsView->selectionModel()->selectedIndexes().first().row();

    QSqlRecord current_record = model->record(row);
    GroupDialog diag(groupType);
    diag.setFields(current_record);
    if (diag.exec())
    {
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
    QItemSelectionModel *selectionModel = ui->catsView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedIndexes();
    QModelIndex index;

    foreach (index, indexes) {

        int row = index.row();

        QString text = tr("Etes-vous sûr de vouloir ") + removeTheGroup(groupType) + QString(" \"%1\" ?").arg(index.data().toString());
        QMessageBox::StandardButton choice = QMessageBox::question(this, removeAGroup(groupType), text);
        if (choice == QMessageBox::Yes) {
            QueryStatement statement(SELECT_OPERATIONS, groupCondition(groupType, row+1));
            QSqlQuery query(QSqlDatabase::database(databaseName));
            int nb_ops = 0;
            query.exec(statement.get());
            while (query.next()) {
                ++nb_ops;
            }
            if (nb_ops > 0) {
                emit groupToBeRemoved(groupType, index.data().toString(), row+1);
            }
        }
    }
}

void GroupList::updateActions(const QItemSelection &selected)
{
    QModelIndexList indexes = selected.indexes();
    if (indexes.first().row() == 0)
        toolBar->updateActions(QItemSelection());
    else
        toolBar->updateActions(selected);
}


