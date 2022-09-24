#include "goalsview.h"

#include <QPainter>
#include <QStandardItem>
#include <QDate>
#include <QSqlQuery>
#include <QInputDialog>
#include <QMenu>
#include <QHeaderView>

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
    bar.setValue(progress);
    bar.setPercentage(index.data().toFloat());
    bar.paint(painter, option.rect);
}


GoalsView::GoalsView(const QString &accountName, QWidget *parent)
    : TableWidget{parent},
      databaseName(accountName)
{
    model()->setHorizontalHeaderLabels({tr("Category/Tag"), tr("Progress"), tr("Goal"), tr("Spent"), tr("Rest")});
    table()->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);
    table()->setSelectionMode(QAbstractItemView::NoSelection);

    delegate = new GoalsViewDelegate(this);
    setTableDelegate(delegate);

    contextMenu()->addAction(EDIT_ICON, tr("Editer l'objectif"), this, &GoalsView::EditGoal);
    contextMenu()->addAction(REMOVE_ICON, tr("Supprimer l'objectif"), this, &GoalsView::RemoveGoal);
}

void GoalsView::RemoveGoal()
{
    Goal goal = model()->item(currentRow(),0)->data(Qt::UserRole).value<Goal>();
    QString name;

    QSqlQuery query(QSqlDatabase::database(databaseName));
    query.exec(QueryStatement(selectGroupCmd(goal.type), idCondition(goal.typeId)).get());
    while (query.next()) {
        name = query.value(1).toString();
    }

    setRemoveTitle(tr("Supprimer un objectif"));
    setRemoveQuestion(QString(tr("Etes-vous sûr de vouloir supprimer l'objectif sur ") + "\"%1\" ?").arg(name));

    TableWidget::removeRow();
}

void GoalsView::EditGoal()
{
    Goal goal = model()->item(currentRow(),0)->data(Qt::UserRole).value<Goal>();
    bool ok;
    float max = QInputDialog::getDouble(this, tr("Editer l'objectif"),
                                       tr("Montant:"), goal.max, 0, INT_MAX, 2, &ok,
                                       Qt::WindowFlags(), 0.01);
    if (ok)
        emit changeState(Modified);

    updateGoalProgress(currentRow(), max);
}

void GoalsView::updateGoalProgress(int goalIndex, float amount)
{
    Goal goal = model()->item(goalIndex,0)->data(Qt::UserRole).value<Goal>();
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
    model()->item(goalIndex,0)->setData(qVarGoal, Qt::UserRole);

    QStandardItem *progressItem = new QStandardItem(QString::number(100*spent/goal.max));
    QStandardItem *goalItem = new QStandardItem(QString::number(goal.max) + " €");

    QBrush brush = spent < goal.max ? QBrush(Qt::black) : QBrush(Qt::red);
    QStandardItem *spentItem = new QStandardItem(QString::number(spent) + " €");
    spentItem->setForeground(brush);
    QStandardItem *restItem = new QStandardItem(QString::number(goal.max-spent) + " €");
    restItem->setForeground(brush);

    model()->setItem(goalIndex, 1, progressItem);
    model()->setItem(goalIndex, 2, goalItem);
    model()->setItem(goalIndex, 3, spentItem);
    model()->setItem(goalIndex, 4, restItem);
    table()->resizeRowsToContents();
    table()->resizeColumnToContents(0);
}

void GoalsView::addGoal(Goal newGoal)
{
    QString name;
    QSqlQuery query(QSqlDatabase::database(databaseName));
    query.exec(QueryStatement(selectGroupCmd(newGoal.type), idCondition(newGoal.typeId)).get());
    while (query.next()) {
        name = query.value(1).toString();
    }

    QStandardItem *nameItem = new QStandardItem(QIcon(groupIcon(newGoal.type)),name);
    QVariant qVarGoal;
    qVarGoal.setValue(newGoal);
    nameItem->setData(qVarGoal, Qt::UserRole);

    int newRow = model()->rowCount();
    model()->insertRow(newRow);
    model()->setItem(newRow, 0, nameItem);
    updateGoalProgress(newRow, newGoal.max);
    emit changeState(Modified);
}

void GoalsView::updateGoals()
{
    for (int row = 0; row < model()->rowCount(); ++row)
        updateGoalProgress(row);
}

