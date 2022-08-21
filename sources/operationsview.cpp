#include "operationsview.h"

#include <QSqlRelationalDelegate>
#include <QHeaderView>

OperationsView::OperationsView()
{
    mainLayout = new QVBoxLayout(this);

    balanceLayout = new QHBoxLayout;
    qlTodayBalance = new QLabel;
    qlTodayBalance->setTextFormat(Qt::RichText);
    qlFutureBalance = new QLabel;
    qlFutureBalance->setTextFormat(Qt::RichText);
    setBalance(0,0);
    balanceLayout->addWidget(qlTodayBalance);
    balanceLayout->addWidget(qlFutureBalance);
    mainLayout->addLayout(balanceLayout);

    filters = new filtersWidget(this);
    connect(filters, SIGNAL(statementBuilt(QString)), this, SLOT(applyFilters(QString)));
    mainLayout->addWidget(filters);

    opsTable = new QTableView;
}

OperationsView::~OperationsView()
{
    filters->deleteLater();
    opsTable->deleteLater();
    qlTodayBalance->deleteLater();
    qlFutureBalance->deleteLater();
    balanceLayout->deleteLater();
    mainLayout->deleteLater();
}

QTableView* OperationsView::table()
{
    return opsTable;
}

void OperationsView::loadModel(QSqlRelationalTableModel * mod)
{
    model = mod;
    opsTable->setModel(model);
    opsTable->setItemDelegate(new QSqlRelationalDelegate(opsTable));
    opsTable->setColumnHidden(model->fieldIndex("id"), true);
    opsTable->setColumnHidden(model->fieldIndex("type"), true);
    opsTable->setCurrentIndex(model->index(0, 0));
    opsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    opsTable->horizontalHeader()->setStretchLastSection(true);
    opsTable->verticalHeader()->hide();
    opsTable->clearSpans();
    opsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    opsTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    opsTable->resizeColumnsToContents();
    opsTable->setShowGrid(false);
    mainLayout->addWidget(opsTable);

    filters->populateComboBoxes(model->relationModel(2), model->relationModel(4));
}

void OperationsView::applyFilters(const QString & statement)
{
    qDebug() << statement;
    model->setFilter(statement);
}

void OperationsView::setBalance(double balance, double future_balance)
{
    qlTodayBalance->setText(tr("Solde actuel : ") + QString("<font color=\"green\" size=\"8\"><b>%1</b></font>")
                                                    .arg(QString::number(balance)));
    qlFutureBalance->setText(tr("Solde futur : ") + QString::number(future_balance));
}
