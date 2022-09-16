#include "goalsview.h"

#include <QHeaderView>
#include <QPainter>
#include <QStandardItem>
#include <QDate>
#include <QSqlQuery>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>

#include "utilities.h"

using namespace Utilities;

void GoalsViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index ) const
{
    if (index.column() != 1) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    ColoredProgressBar bar;
    int progress = qRound(index.data().toFloat());
    if (progress < 0)
        progress = 0;
    else if (progress > 100)
        progress = 100;
    progress = progress < 0 ? 0 : progress;
    bar.setValue(progress);
    bar.setPercentage(index.data().toFloat());
    bar.paint(painter, option.rect);
}


MyStandardItemModel::MyStandardItemModel(QObject *parent) : QStandardItemModel(parent)
{

}

QVariant MyStandardItemModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole )
        return Qt::AlignCenter;
    else
        return QStandardItemModel::data(index, role);
}


GoalsView::GoalsView(const QString &accountName, QWidget *parent)
    : QWidget{parent},
      databaseName(accountName),
      currentGoal(-1)
{
    mainLayout = new QVBoxLayout(this);

    goals_model = new MyStandardItemModel(this);
    goals_model->setHorizontalHeaderLabels({tr("Category/Tag"), tr("Progress"), tr("Goal"), tr("Spent"), tr("Rest")});

    tableView = new QTableView(this);
    tableView->setModel(goals_model);
    tableView->verticalHeader()->hide();
    tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);
    tableView->clearSpans();
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSelectionMode(QAbstractItemView::NoSelection);
    tableView->setShowGrid(false);

    delegate = new GoalsViewDelegate(this);
    tableView->setItemDelegate(delegate);


    createCustomContextMenu();
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tableView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customMenuRequested(QPoint)));

    mainLayout->addWidget(tableView);
}

void GoalsView::createCustomContextMenu()
{
    contextMenu = new QMenu(tableView);
    QAction *editAct = contextMenu->addAction(QIcon(":/images/images/edit_48px.png"),
                                              tr("Editer l'objectif"), this, &GoalsView::EditGoal);
    contextMenu->addAction(editAct);
    QAction *removeAct = contextMenu->addAction(QIcon(":/images/images/Remove_48px.png"),
                                                tr("Supprimer l'objectif"), this, &GoalsView::RemoveGoal);
    contextMenu->addAction(removeAct);
}

void GoalsView::customMenuRequested(QPoint pos){
    QModelIndex index=tableView->indexAt(pos);
    if (index.isValid()) {
        currentGoal = index.row();
        contextMenu->popup(tableView->viewport()->mapToGlobal(pos));
    }
}

void GoalsView::RemoveGoal()
{
    Goal goal = goals_model->item(currentGoal,0)->data(Qt::UserRole).value<Goal>();
    QString name;

    QSqlQuery query(QSqlDatabase::database(databaseName));
    query.exec(QueryStatement(selectGroupCmd(goal.type), idCondition(goal.typeId)).get());
    while (query.next()) {
        name = query.value(1).toString();
    }

    QMessageBox::StandardButton choice = QMessageBox::question(this, tr("Supprimer un objectif"),
                                                               QString(tr("Etes-vous sûr de vouloir supprimer l'objectif sur ") + "\"%1\" ?").arg(name));
    switch(choice) {
    case QMessageBox::Yes:
        goals_model->removeRow(currentGoal);
        emit changeState(Modified);
        break;
    default:
        return;
        break;
    }
}

void GoalsView::EditGoal()
{
    Goal goal = goals_model->item(currentGoal,0)->data(Qt::UserRole).value<Goal>();
    bool ok;
    float max = QInputDialog::getDouble(this, tr("Editer l'objectif"),
                                       tr("Montant:"), goal.max, 0, INT_MAX, 2, &ok,
                                       Qt::WindowFlags(), 0.01);
    if (ok)
        emit changeState(Modified);

    updateGoalProgress(currentGoal, max);
}

void GoalsView::updateGoalProgress(int goalIndex, float amount)
{
    Goal goal = goals_model->item(goalIndex,0)->data(Qt::UserRole).value<Goal>();
    goal.max = amount > 0 ? amount : goal.max;
    float spent = qQNaN();

    QDate today = QDate::currentDate();
    int year = today.year();
    int month = today.month();

    QDate begin(year, month, 1);
    QDate end(year, month, daysInMonth(month, year));
    QueryStatement statement(SELECT_SUM);
    statement.addCondition(lowerDateCondition(begin));
    statement.addCondition(upperDateCondition(end));
    statement.addCondition(groupCondition(goal.type, goal.typeId));

    QSqlQuery query(QSqlDatabase::database(databaseName));
    query.exec(statement.get());
    while (query.next()) {
        spent = qAbs(query.value(0).toFloat());
    }

    QVariant qVarGoal;
    qVarGoal.setValue(goal);
    goals_model->item(goalIndex,0)->setData(qVarGoal, Qt::UserRole);

    QStandardItem *progressItem = new QStandardItem(QString::number(100*spent/goal.max));
    QStandardItem *goalItem = new QStandardItem(QString::number(goal.max) + " €");
    QStandardItem *spentItem = new QStandardItem(QString::number(spent) + " €");
    QStandardItem *restItem = new QStandardItem(QString::number(goal.max-spent) + " €");

    goals_model->setItem(goalIndex, 1, progressItem);
    goals_model->setItem(goalIndex, 2, goalItem);
    goals_model->setItem(goalIndex, 3, spentItem);
    goals_model->setItem(goalIndex, 4, restItem);
    tableView->resizeRowsToContents();
    tableView->resizeColumnToContents(0);
}

void GoalsView::addGoal(Goal newGoal)
{
    QString name;
    QSqlQuery query(QSqlDatabase::database(databaseName));
    query.exec(QueryStatement(selectGroupCmd(newGoal.type), idCondition(newGoal.typeId)).get());
    while (query.next()) {
        name = query.value(1).toString();
    }

    QStandardItem *nameItem = new QStandardItem(QIcon(groupIconByType(newGoal.type)),name);
    QVariant qVarGoal;
    qVarGoal.setValue(newGoal);
    nameItem->setData(qVarGoal, Qt::UserRole);

    int newRow = goals_model->rowCount();
    goals_model->insertRow(newRow);
    goals_model->setItem(newRow, 0, nameItem);
    updateGoalProgress(newRow, newGoal.max);
    emit changeState(Modified);
}

void GoalsView::updateGoals()
{
    for (int row = 0; row < goals_model->rowCount(); ++row)
        updateGoalProgress(row);
}

QStandardItemModel* GoalsView::goalsModel()
{
    return goals_model;
}
