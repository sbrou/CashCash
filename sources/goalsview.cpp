#include "goalsview.h"

#include <QHeaderView>
#include <QPainter>

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


GoalsView::GoalsView(QWidget *parent)
    : QWidget{parent}
{
    mainLayout = new QVBoxLayout(this);

    goals_model = new MyStandardItemModel(this);
    goals_model->setRowCount(3);
    goals_model->setColumnCount(5);
    goals_model->setHeaderData(0, Qt::Horizontal, tr("Category/Tag"));
    goals_model->setHeaderData(1, Qt::Horizontal, tr("Progress"));
    goals_model->setHeaderData(2, Qt::Horizontal, tr("Goal"));
    goals_model->setHeaderData(3, Qt::Horizontal, tr("Spent"));
    goals_model->setHeaderData(4, Qt::Horizontal, tr("Rest"));

    tableView = new QTableView(this);
    tableView->setModel(goals_model);
    tableView->verticalHeader()->hide();
    tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);
    tableView->clearSpans();
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSelectionMode(QAbstractItemView::NoSelection);
//    tableView->resizeColumnsToContents();
    tableView->setShowGrid(false);

    delegate = new GoalsViewDelegate(this);
    tableView->setItemDelegate(delegate);

    init();

    mainLayout->addWidget(tableView);
}

void GoalsView::init()
{
    goals_model->setData(goals_model->index(0, 0), QString("ESSENTIAL"));
    goals_model->setData(goals_model->index(0, 1), 82);
    goals_model->setData(goals_model->index(0, 2), 100);
    goals_model->setData(goals_model->index(0, 3), 82);
    goals_model->setData(goals_model->index(0, 4), 100-82);

    goals_model->setData(goals_model->index(1, 0), QString("WANTINGS"));
    goals_model->setData(goals_model->index(1, 1), 54);
    goals_model->setData(goals_model->index(1, 2), 100);
    goals_model->setData(goals_model->index(1, 3), 54);
    goals_model->setData(goals_model->index(1, 4), 100-54);

    goals_model->setData(goals_model->index(2, 0), QString("SAVINGS"));
    goals_model->setData(goals_model->index(2, 1), 23);
    goals_model->setData(goals_model->index(2, 2), 100);
    goals_model->setData(goals_model->index(2, 3), 23);
    goals_model->setData(goals_model->index(2, 4), 100-23);
}
