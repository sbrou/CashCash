#include "operationsview.h"

#include <QHeaderView>
#include <QSqlRelationalDelegate>
#include <QSettings>

#include "defines.h"

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

    qpbHideOrShowFilters = new QPushButton(this);
    qpbHideOrShowFilters->setCheckable(true);
    connect(qpbHideOrShowFilters, SIGNAL(toggled(bool)), this, SLOT(slotShowFilters(bool)));
    mainLayout->addWidget(qpbHideOrShowFilters);

    filters = new filtersWidget(this);
    connect(filters, SIGNAL(statementBuilt(QString)), this, SLOT(applyFilters(QString)));
    mainLayout->addWidget(filters);

    opsTable = new QTableView;

    cxtMenu = new QMenu(opsTable);
    opsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(opsTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequested(QPoint)));

    readSettings();
}

void OperationsView::customMenuRequested(QPoint pos){
    QModelIndex index=opsTable->indexAt(pos);
    if (index.isValid()) {
        cxtMenu->popup(opsTable->viewport()->mapToGlobal(pos));
    }
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

void OperationsView::saveSettings()
{
    QSettings settings;
    settings.setValue("operations/filtersVisible", filters->isVisible());
}

void OperationsView::readSettings()
{
    QSettings settings;
    bool isVisible = settings.value("operations/filtersVisible", false).toBool();
    qpbHideOrShowFilters->setChecked(isVisible);
    slotShowFilters(isVisible);
}

void OperationsView::slotShowFilters(bool isVisible)
{
    if (isVisible) {
        filters->show();
        qpbHideOrShowFilters->setIcon(QIcon(":/images/images/arrow_up.png"));
        qpbHideOrShowFilters->setText(tr("Cacher les filtres"));
    }
    else {
        filters->hide();
        qpbHideOrShowFilters->setIcon(QIcon(":/images/images/arrow_down.png"));
        qpbHideOrShowFilters->setText(tr("Montrer les filtres"));
    }
}

QTableView* OperationsView::table()
{
    return opsTable;
}

QMenu* OperationsView::contextMenu()
{
    return cxtMenu;
}

void OperationsView::loadModel(SqlRelationalTableModel * mod)
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
    filters->buildStatement();
}

void OperationsView::applyFilters(const QString & statement)
{
    model->setFilter(statement);
}

void OperationsView::setBalance(float balance, float future_balance)
{
    QString bColor = balance >= 0 ? "green" : "red";
    QString fbColor = future_balance >= 0 ? "green" : "red";

    qlTodayBalance->setText(tr("Solde actuel : ") + QString("<font color=\"%1\" size=\"8\"><b>%2</b></font>")
                                                    .arg(bColor)
                                                    .arg(QString::number(balance)));
    qlFutureBalance->setText(tr("Solde futur : ") + QString("<font color=\"%1\" size=\"8\"><b>%2</b></font>")
                                                    .arg(fbColor)
                                                    .arg(QString::number(future_balance)));
}

void OperationsView::resizeView()
{
    filters->initComboBoxes();
    opsTable->resizeRowsToContents();
    opsTable->resizeColumnsToContents();
}
