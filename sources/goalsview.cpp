#include "goalsview.h"

#include <QHeaderView>
#include <QPainter>
#include <QStandardItem>
#include <QDate>
#include <QSqlQuery>

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
      databaseName(accountName)
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

    mainLayout->addWidget(tableView);
}

double GoalsView::computeGoalProgress(const QString &groupName, int typeId)
{
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
                        .arg(typeId);

    QSqlQuery query(QSqlDatabase::database(databaseName));
    query.exec(statement);
    while (query.next()) {
        spent = qAbs(query.value(0).toDouble());
    }

    return spent;
}

void GoalsView::addGoal(Goal newGoal)
{
    QString icon = newGoal.type == CatType ? ":/images/images/category_48px.png" : ":/images/images/tag_window_48px.png";
    QString groupName = newGoal.type == CatType ? "category" : "tag";
    QString groupTable = newGoal.type == CatType ? "categories" : "tags";
    QString name;

    QSqlQuery query(QSqlDatabase::database(databaseName));
    query.exec(QString("SELECT * FROM %1 WHERE id=%2").arg(groupTable).arg(newGoal.typeId));
    while (query.next()) {
        name = query.value(1).toString();
    }

    QStandardItem *nameItem = new QStandardItem(QIcon(icon),name);
    nameItem->setData(newGoal.typeId, Qt::UserRole);

    double spent = computeGoalProgress(groupName, newGoal.typeId);

    QStandardItem *progressItem = new QStandardItem(QString::number(100*spent/newGoal.max));
    QStandardItem *goalItem = new QStandardItem(QString::number(newGoal.max));
    QStandardItem *spentItem = new QStandardItem(QString::number(spent));
    QStandardItem *restItem = new QStandardItem(QString::number(newGoal.max-spent));
    goals_model->appendRow({nameItem, progressItem, goalItem, spentItem, restItem});
    tableView->resizeRowsToContents();
}

void GoalsView::updateGoals()
{

}
