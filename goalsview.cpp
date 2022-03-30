#include "goalsview.h"

#include <QHeaderView>
#include <QPainter>

GoalsViewDelegate::GoalsViewDelegate(QWidget *parent) : QItemDelegate(parent) {}

void GoalsViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index ) const
{
    if (index.column() != 1) {
        QItemDelegate::paint(painter, option, index);
        return;
    }

    // Set up a QStyleOptionProgressBar to precisely mimic the
    // environment of a progress bar.
    QStyleOptionProgressBar progressBarOption;
    progressBarOption.state = QStyle::State_Enabled;
    progressBarOption.direction = Qt::LeftToRight;
    progressBarOption.rect = option.rect;
    progressBarOption.fontMetrics = qobject_cast<QWidget *>(parent())->fontMetrics();
    progressBarOption.minimum = 0;
    progressBarOption.maximum = 100;
    progressBarOption.textAlignment = Qt::AlignCenter;
    progressBarOption.textVisible = true;
    progressBarOption.bottomToTop = false;

    // Set the progress and text values of the style option.
    int progress = qRound(index.data().toDouble());
    qDebug() << progress;
    progressBarOption.progress = progress < 0 ? 0 : progress;
    progressBarOption.text = QString::asprintf("%d%%", progressBarOption.progress);

    // Draw the progress bar onto the view.
    QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
}


GoalsView::GoalsView(QWidget *parent)
    : QWidget{parent}
{
    mainLayout = new QVBoxLayout(this);

    goals_model = new QStandardItemModel(1, 5, this);
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
    goals_model->setData(goals_model->index(0, 1), (101.71/1002.17)*100);
    goals_model->setData(goals_model->index(0, 2), 1002.78);
    goals_model->setData(goals_model->index(0, 3), 501.71);
    goals_model->setData(goals_model->index(0, 4), 1002.78-501.71);
}
