#include "statswidget.h"

#include <QGuiApplication>
#include <QScreen>
#include <QSqlQuery>
#include <QPair>
#include <QDate>
#include <QFormLayout>

#include "defines.h"
#include "utilities.h"

using namespace Utilities;

StatsWidget::StatsWidget(double balance, const QString & account_title, QWidget *parent)
    : QDialog{parent}
    , _init_balance(balance)
    , _account_name(account_title)
{
    QGridLayout * mainLayout = new QGridLayout(this);

    QGroupBox * settings = new QGroupBox(tr("Opérations"), this);
    QFormLayout *sLayout = new QFormLayout(settings);
    qcbOpType = new QComboBox;
    qcbOpType->addItems({ tr("Dépenses"), tr("Revenus"), tr("Tout") });
    connect(qcbOpType, SIGNAL(currentIndexChanged(int)), this, SLOT(populateTable()));
    sLayout->addRow(tr("Type :"), qcbOpType);
    qcbGroupType = new QComboBox;
    qcbGroupType->addItems({ tr("Categories"), tr("Tags") });
    connect(qcbGroupType, SIGNAL(currentIndexChanged(int)), this, SLOT(populateTable()));
    sLayout->addRow(tr("Par :"), qcbGroupType);
    mainLayout->addWidget(settings, 0, 0);

    QGroupBox * dateFilter = new QGroupBox(tr("Période"), this);
    QVBoxLayout * vlayout = new QVBoxLayout(dateFilter);
    qcbPeriod = new QComboBox(this);
    qcbPeriod->addItems({ tr("Ce Mois"), tr("Ce trimestre"), tr("Cette année"),
                          tr("L'année dernière"), tr("Personnalisé")});
    connect(qcbPeriod, SIGNAL(currentIndexChanged(int)), this, SLOT(changeTimePeriod(int)));
    getTimePeriod(CurrentMonth, dateFrom, dateTo);
    vlayout->addWidget(qcbPeriod);

    QFormLayout *dLayout = new QFormLayout;

    qdeDateFrom = new QDateEdit(this);
    qdeDateFrom->setCalendarPopup(true);
    qdeDateFrom->setDate(dateFrom);
    qdeDateFrom->setEnabled(false);
    connect(qdeDateFrom, SIGNAL(dateChanged(QDate)), this, SLOT(setDateFrom(QDate)));
    dLayout->addRow("From :", qdeDateFrom);

    qdeDateTo = new QDateEdit(this);
    qdeDateTo->setCalendarPopup(true);
    qdeDateTo->setDate(dateTo);
    qdeDateTo->setEnabled(false);
    connect(qdeDateTo, SIGNAL(dateChanged(QDate)), this, SLOT(setDateTo(QDate)));
    dLayout->addRow("To :", qdeDateTo);
    vlayout->addLayout(dLayout);
    mainLayout->addWidget(dateFilter, 1, 0);

    table = new QTableWidget(this);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    populateTable();
    mainLayout->addWidget(table, 0, 1, 3, 5);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->availableGeometry();
    int height = 3*screenGeometry.height() / 4;
    int width = 3*screenGeometry.width() / 4;
    resize(width, height);
}

void StatsWidget::populateTable()
{
    table->clear();

    int groupTypeIndex = qcbGroupType->currentIndex();

    SumsByGroup groups;
    QueryStatement statement(selectGroupCmd((GroupType)groupTypeIndex));

    int opTypeIndex = qcbOpType->currentIndex();
    if (opTypeIndex != 2)
    {
        statement.addCondition(typeCondition((OpType)opTypeIndex));
    }

    int month1 = dateFrom.month();
    int month2 = dateTo.month();
    int nb_months = month2 - month1 + 1;
    
    QSqlQuery query(QSqlDatabase::database(_account_name));
    query.exec(statement.get());
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QList<double> sums(nb_months + 1, 0); // les mois + la somme des mois
        groups.insert(qMakePair(id,name), sums);
    }

    table->setRowCount(2); // Resultat et solde
    table->setVerticalHeaderLabels({ tr("Resultats"), tr("Solde") });

    table->setColumnCount(nb_months + 2); // les mois + moyenne de l'annee + total
    QStringList verticalsLabels;
    for (int i = month1; i <= month2; ++i)
        verticalsLabels << monthName(i);
    verticalsLabels << tr("Moyenne");
    verticalsLabels << tr("Total");
    table->setHorizontalHeaderLabels(verticalsLabels);

    double results_sum = 0;
    QDate today = QDate::currentDate();

    for (int m = month1; m <= month2; ++m)
    {
        int month = m - month1;
        double month_result = 0;

        QDate start = m == month1 ? dateFrom : QDate(today.year(), m, 1);
        QDate end = m == month2 ? dateTo : QDate(today.year(), m, daysInMonth(m, today.year()));

        statement.setCommand(SELECT_SUM);
        statement.addCondition(lowerDateCondition(start));
        statement.addCondition(upperDateCondition(end));

        SumsByGroup::iterator it_group = groups.begin();
        while (it_group != groups.end()) {
            statement.addCondition(groupCondition((GroupType)groupTypeIndex, it_group.key().first));
            query.exec(statement.get());
            while (query.next()) {
                double sum = query.value(0).toDouble();
                month_result += sum;
                it_group.value()[month] = sum;
                it_group.value()[nb_months] += sum;
            }
            statement.clearConditions(2);
            ++it_group;
        }

        results_sum += month_result;
        addItemInTable(month_result, 0, month); // Resultat
        addItemInTable(getBalanceByDate(end), 1, month); // Solde
    }

    int row = 0;
    SumsByGroup::const_iterator cgroup = groups.constBegin();
    while (cgroup != groups.constEnd()) {
        QList<double> sums = cgroup.value();
        if (sums.at(nb_months) != 0)
        {
            table->insertRow(row);
            table->setVerticalHeaderItem(row, new QTableWidgetItem(cgroup.key().second));

            for (qsizetype i = 0; i < table->columnCount(); ++i) {
                double amount;
                if (i == nb_months)
                    amount = sums.at(nb_months) / nb_months;
                else if (i == nb_months + 1)
                    amount = sums.at(nb_months);
                else
                    amount = sums.at(i);
                addItemInTable(amount, row, i);
            }

            ++row;
        }
        ++cgroup;
    }

    addItemInTable(results_sum/nb_months, table->rowCount() - 2, nb_months); // Moyenne resultats
    addItemInTable(results_sum, table->rowCount() - 2, nb_months + 1); // total resultats

    table->setItem(table->rowCount() - 1, nb_months, new QTableWidgetItem("-")); // Moyenne solde : sens ?
    addItemInTable(getBalanceByDate(today), table->rowCount() - 1, nb_months + 1); // Solde actuel
}

double StatsWidget::getBalanceByDate(QDate date)
{
    double balance = 0;
    QSqlQuery query(QSqlDatabase::database(_account_name));
    query.exec(QueryStatement(SELECT_SUM, upperDateCondition(date)).get());
    while (query.next()) {
        balance = _init_balance + query.value(0).toDouble();
    }
    return balance;
}

void StatsWidget::addItemInTable(double amount, int row, int column)
{
    QColor aColor = amount >= 0 ? Qt::darkGreen : Qt::darkRed;
    QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(qAbs(amount)));
    newItem->setForeground(QBrush(aColor));
    newItem->setTextAlignment(Qt::AlignCenter);
    table->setItem(row, column, newItem);
}

void StatsWidget::changeTimePeriod(int index)
{
    qdeDateFrom->setEnabled(index > 3);
    qdeDateTo->setEnabled(index > 3);

    switch (index)
    {
    case 0:
        getTimePeriod(CurrentMonth, dateFrom, dateTo);
    case 1:
        getTimePeriod(ThreePastMonths,dateFrom,dateTo);
        break;
    case 2: // this year
        getTimePeriod(CurrentYear,dateFrom,dateTo);
        break;
    case 3: // previous year
        getTimePeriod(PreviousYear,dateFrom,dateTo);
        break;
    default:
        break;
    }

    populateTable();
}

void StatsWidget::setDateFrom(QDate date)
{
    dateFrom = date;
    changeTimePeriod(4);
}

void StatsWidget::setDateTo(QDate date)
{
    dateTo = date;
    changeTimePeriod(4);
}
