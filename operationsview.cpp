#include "operationsview.h"

#include <QSqlRelationalDelegate>
#include <QHeaderView>

OperationsView::OperationsView()
{
    mainLayout = new QHBoxLayout(this);

    opsTable = new QTableView;
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
    //        opsTable->setColumnHidden(model->fieldIndex("id"), true);
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
}

