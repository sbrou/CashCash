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

void GoalsViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index ) const
{
    if (index.column() != 1) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    ColoredProgressBar bar;
    int progress = qRound(index.data().toDouble());
    progress = progress < 0 ? 0 : progress;
    bar.setValue(progress);
    bar.setPercentage(index.data().toDouble());
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
    QString groupTable = goal.type == CatType ? "categories" : "tags";
    QString name;

    QSqlQuery query(QSqlDatabase::database(databaseName));
    query.exec(QString("SELECT * FROM %1 WHERE id=%2").arg(groupTable).arg(goal.typeId));
    while (query.next()) {
        name = query.value(1).toString();
    }

    QMessageBox::StandardButton choice = QMessageBox::question(this, tr("Supprimer un objectif"),
                                                               QString(tr("Etes-vous sûr de vouloir supprimer l'objectif sur ") + "\"%1\"").arg(name));
    switch(choice) {
    case QMessageBox::Yes:
        goals_model->removeRow(currentGoal);
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
    double max = QInputDialog::getDouble(this, tr("Editer l'objectif"),
                                       tr("Montant:"), goal.max, 0, INT_MAX, 2, &ok,
                                       Qt::WindowFlags(), 0.01);
    if (!ok) {
        QMessageBox::critical(this, tr("Erreur"), tr("Veuillez entrer un montant valide."));
        return;
    }

    updateGoalProgress(currentGoal, max);
}

void GoalsView::updateGoalProgress(int goalIndex, double amount)
{
    Goal goal = goals_model->item(goalIndex,0)->data(Qt::UserRole).value<Goal>();
    QString groupName = goal.type == CatType ? "category" : "tag";
    double max = amount > 0 ? amount : goals_model->item(goalIndex,2)->data(Qt::UserRole).toDouble();
    double spent = qQNaN();

    QDate today = QDate::currentDate();
    int year = today.year();
    int month = today.month();

    QDate begin(year, month, 1);
    QDate end(year, month, daysInMonth(month, year));
    QString date = QString("op_date>='%1' AND op_date<='%2'")
            .arg(begin.toString(Qt::ISODateWithMs))
            .arg(end.toString(Qt::ISODateWithMs));

    QString statement = QString("SELECT SUM (amount) FROM operations WHERE " + date + " AND %1=%2")
                        .arg(groupName)
                        .arg(goal.typeId);

    QSqlQuery query(QSqlDatabase::database(databaseName));
    query.exec(statement);
    while (query.next()) {
        spent = qAbs(query.value(0).toDouble());
    }

    QStandardItem *progressItem = new QStandardItem(QString::number(100*spent/max));
    QStandardItem *goalItem = new QStandardItem(QString::number(max) + " €");
    goalItem->setData(max, Qt::UserRole);
    QStandardItem *spentItem = new QStandardItem(QString::number(spent) + " €");
    QStandardItem *restItem = new QStandardItem(QString::number(max-spent) + " €");

    goals_model->setItem(goalIndex, 1, progressItem);
    goals_model->setItem(goalIndex, 2, goalItem);
    goals_model->setItem(goalIndex, 3, spentItem);
    goals_model->setItem(goalIndex, 4, restItem);
    tableView->resizeRowsToContents();
    tableView->resizeColumnToContents(0);
}

void GoalsView::addGoal(Goal newGoal)
{
    QString icon = newGoal.type == CatType ? ":/images/images/category_48px.png" : ":/images/images/tag_window_48px.png";
    QString groupTable = newGoal.type == CatType ? "categories" : "tags";
    QString name;

    QSqlQuery query(QSqlDatabase::database(databaseName));
    query.exec(QString("SELECT * FROM %1 WHERE id=%2").arg(groupTable).arg(newGoal.typeId));
    while (query.next()) {
        name = query.value(1).toString();
    }

    QStandardItem *nameItem = new QStandardItem(QIcon(icon),name);
    QVariant qVarGoal;
    qVarGoal.setValue(newGoal);
    nameItem->setData(qVarGoal, Qt::UserRole);

    int newRow = goals_model->rowCount();
    goals_model->insertRow(newRow);
    goals_model->setItem(newRow, 0, nameItem);
    updateGoalProgress(newRow, newGoal.max);
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
