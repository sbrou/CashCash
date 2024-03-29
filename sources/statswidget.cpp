#include "statswidget.h"
#include "ui_statswidget.h"

#include <QGuiApplication>
#include <QScreen>
#include <QSqlQuery>
#include <QPair>
#include <QDate>

#include "defines.h"
#include "utilities.h"

using namespace utilities;

StatsWidget::StatsWidget(float balance, const QString & account_title, QWidget *parent)
    : QWidget{parent},
      ui(new Ui::StatsWidget),
     _init_balance(balance),
     _account_name(account_title)
{
    ui->setupUi(this);

    setWindowTitle(_account_name + " - " + tr("Statistiques"));

    ui->qcbOpType->addItems({ tr("Dépenses"), tr("Revenus"), tr("Tout") });
    connect(ui->qcbOpType, SIGNAL(currentIndexChanged(int)), this, SLOT(populateTable()));

    ui->qcbGroupType->addItems({ tr("Catégories"), tr("Tags") });
    connect(ui->qcbGroupType, SIGNAL(currentIndexChanged(int)), this, SLOT(populateTable()));

    ui->qcbPeriod->addItems({ tr("Ce Mois"), tr("Le mois dernier"), tr("Ce trimestre"), tr("Cette année"),
                          tr("L'année dernière"), tr("Personnalisé")});
    connect(ui->qcbPeriod, SIGNAL(currentIndexChanged(int)), this, SLOT(changeTimePeriod(int)));
    getTimePeriod(CurrentMonth, dateFrom, dateTo);

    ui->qdeDateFrom->setDate(dateFrom);
    ui->qdeDateFrom->setEnabled(false);
    connect(ui->qdeDateFrom, SIGNAL(dateChanged(QDate)), this, SLOT(changeCustomPeriod()));

    ui->qdeDateTo->setDate(dateTo);
    ui->qdeDateTo->setEnabled(false);
    connect(ui->qdeDateTo, SIGNAL(dateChanged(QDate)), this, SLOT(changeCustomPeriod()));

    connect(ui->qpbRefresh, SIGNAL(clicked()), this, SLOT(populateTable()));

    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    setWindowFlags(Qt::Window);
}

void StatsWidget::populateTable()
{
    ui->table->clear();

    int groupTypeIndex = ui->qcbGroupType->currentIndex();

    SumsByGroup groups;
    QueryStatement statement(selectGroupCmd((GroupType)groupTypeIndex));

    int opTypeIndex = ui->qcbOpType->currentIndex();
    if (opTypeIndex != 2)
    {
        statement.addCondition(typeCondition((OpType)opTypeIndex));
    }

    int month1 = dateFrom.month();
    int month2 = dateTo.month();
    int nb_months = month2 >= month1 ? month2 - month1 + 1 : month2 - month1 + 13;
    
    QSqlQuery query(QSqlDatabase::database(_account_name));
    query.exec(statement.get());
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QList<float> sums(nb_months + 1, 0); // les mois + la somme des mois
        groups.insert(qMakePair(id,name), sums);
    }

    ui->table->setRowCount(2); // Resultat et solde
    ui->table->setVerticalHeaderLabels({ tr("Résultats"), tr("Solde") });

    ui->table->setColumnCount(nb_months + 2); // les mois + moyenne de l'annee + total
    QStringList verticalsLabels;

    for (int i = 0; i < nb_months; ++i)
        verticalsLabels << monthName((month1+i)%12);
    verticalsLabels << tr("Moyenne");
    verticalsLabels << tr("Total");
    ui->table->setHorizontalHeaderLabels(verticalsLabels);

    float results_sum = 0;
    QDate today = QDate::currentDate();
    int yearFrom = dateFrom.year();

    for (int m = 0; m < nb_months; ++m)
    {
        float month_result = 0;

        int month = (month1+m)%12;
        if (month == 0) month = 12;

        int year = month1+m <= 12 ? yearFrom : yearFrom + 1;

        QDate start = m == 0 ? dateFrom : QDate(year, month, 1);
        QDate end = m == nb_months ? dateTo : QDate(year, month, daysInMonth(month, year));

        statement.setCommand(SELECT_SUM);
        statement.addCondition(lowerDateCondition(start));
        statement.addCondition(upperDateCondition(end));

        SumsByGroup::iterator it_group = groups.begin();
        while (it_group != groups.end()) {
            statement.addCondition(groupCondition((GroupType)groupTypeIndex, it_group.key().first));
            query.exec(statement.get());
            while (query.next()) {
                float sum = query.value(0).toFloat();
                month_result += sum;
                it_group.value()[m] = sum;
                it_group.value()[nb_months] += sum;
            }
            statement.clearConditions(2);
            ++it_group;
        }

        results_sum += month_result;
        addItemInTable(month_result, 0, m); // Resultat
        addItemInTable(getBalanceByDate(end), 1, m); // Solde
    }

    int row = 0;
    SumsByGroup::const_iterator cgroup = groups.constBegin();
    while (cgroup != groups.constEnd()) {
        QList<float> sums = cgroup.value();
        if (sums.at(nb_months) != 0)
        {
            ui->table->insertRow(row);
            ui->table->setVerticalHeaderItem(row, new QTableWidgetItem(cgroup.key().second));

            for (qsizetype i = 0; i < ui->table->columnCount(); ++i) {
                float amount;
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

    addItemInTable(results_sum/nb_months, ui->table->rowCount() - 2, nb_months); // Moyenne resultats
    addItemInTable(results_sum, ui->table->rowCount() - 2, nb_months + 1); // total resultats

    QTableWidgetItem *nonSense = new QTableWidgetItem("-");
    nonSense->setTextAlignment(Qt::AlignCenter);
    ui->table->setItem(ui->table->rowCount() - 1, nb_months, nonSense); // Moyenne solde : sens ?
    addItemInTable(getBalanceByDate(today), ui->table->rowCount() - 1, nb_months + 1); // Solde actuel
}

float StatsWidget::getBalanceByDate(QDate date)
{
    float balance = 0;
    QSqlQuery query(QSqlDatabase::database(_account_name));
    query.exec(QueryStatement(SELECT_SUM, upperDateCondition(date)).get());
    while (query.next()) {
        balance = _init_balance + query.value(0).toFloat();
    }
    return balance;
}

void StatsWidget::addItemInTable(float amount, int row, int column)
{
    QColor aColor = amount >= 0 ? Qt::darkGreen : Qt::darkRed;
    QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(qAbs(amount), 'f', 2));
    newItem->setForeground(QBrush(aColor));
    newItem->setTextAlignment(Qt::AlignCenter);
    ui->table->setItem(row, column, newItem);
}

void StatsWidget::changeTimePeriod(int index)
{
    ui->qdeDateFrom->setEnabled(index > 4);
    ui->qdeDateTo->setEnabled(index > 4);

    switch (index)
    {
    case 0:
        getTimePeriod(CurrentMonth, dateFrom, dateTo);
        break;
    case 1:
        getTimePeriod(PreviousMonth, dateFrom, dateTo);
        break;
    case 2:
        getTimePeriod(ThreePastMonths,dateFrom,dateTo);
        break;
    case 3: // this year
        getTimePeriod(CurrentYear,dateFrom,dateTo);
        break;
    case 4: // previous year
        getTimePeriod(PreviousYear,dateFrom,dateTo);
        break;
    case 5: //personalized
        dateFrom = ui->qdeDateFrom->date();
        dateTo = ui->qdeDateTo->date();
        break;
    default:
        break;
    }

    populateTable();
}

void StatsWidget::changeCustomPeriod()
{
    changeTimePeriod(5);
}
