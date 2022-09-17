#include "tablewidget.h"
#include "ui_tablewidget.h"

#include <QMenu>
#include <QMessageBox>

TableWidget::TableWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableWidget)
{
    ui->setupUi(this);

    mod = new MyStandardItemModel(this);
    ui->tableView->setModel(mod);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->clearSpans();
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setShowGrid(false);
    cntxtMenu = new QMenu(ui->tableView);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequested(QPoint)));
}

TableWidget::~TableWidget()
{
    delete ui;
}

void TableWidget::customMenuRequested(QPoint pos){
    QModelIndex index=ui->tableView->indexAt(pos);
    if (index.isValid()) {
        // currentGoal = index.row();
        cntxtMenu->popup(ui->tableView->viewport()->mapToGlobal(pos));
    }
}

int TableWidget::currentRow()
{
    return ui->tableView->currentIndex().row();
}

void TableWidget::removeRow()
{
    QMessageBox::StandardButton choice = QMessageBox::question(this, removeTitle, removeQuestion);
    switch(choice) {
    case QMessageBox::Yes:
        mod->removeRow(currentRow());
        emit changeState(Modified);
        break;
    default:
        return;
        break;
    }
}

QStandardItemModel* TableWidget::model()
{
    return mod;
}

QTableView * TableWidget::table()
{
    return ui->tableView;
}

QMenu * TableWidget::contextMenu()
{
    return cntxtMenu;
}

void TableWidget::addToolBar(QToolBar* toolbar)
{
    ui->toolLayout->addWidget(toolbar);
}

void TableWidget::setTableDelegate(QStyledItemDelegate *delegate)
{
    ui->tableView->setItemDelegate(delegate);
}

void TableWidget::setRemoveTitle(const QString & title)
{
    removeTitle = title;
}

void TableWidget::setRemoveQuestion(const QString & question)
{
    removeQuestion = question;
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
