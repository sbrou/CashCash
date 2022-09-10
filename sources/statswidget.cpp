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
    mainLayout = new QGridLayout(this);

    settings = new QGroupBox;
    QFormLayout *sLayout = new QFormLayout(settings);
    qcbOpType = new QComboBox;
    qcbOpType->addItems({ tr("Dépenses"), tr("Revenus"), tr("Tout") });
    connect(qcbOpType, SIGNAL(currentIndexChanged(int)), this, SLOT(populateTable()));
    sLayout->addRow(tr("Opérations :"), qcbOpType);
    qcbGroupType = new QComboBox;
    qcbGroupType->addItems({ tr("Categories"), tr("Tags") });
    connect(qcbGroupType, SIGNAL(currentIndexChanged(int)), this, SLOT(populateTable()));
    sLayout->addRow(tr("Type :"), qcbGroupType);
    mainLayout->addWidget(settings, 0, 0);

    dateFilter = new QGroupBox;
    mainLayout->addWidget(dateFilter, 1, 0);

    table = new QTableWidget;
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    populateTable();
    mainLayout->addWidget(table, 0, 1, 3, 5);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->availableGeometry();
    int height = 3*screenGeometry.height() / 4;
    int width = 3*screenGeometry.width() / 4;
    resize(width, height);
}

StatsWidget::~StatsWidget()
{
    table->deleteLater();
    dateFilter->deleteLater();
    settings->deleteLater();
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
    
    QSqlQuery query(QSqlDatabase::database(_account_name));
    query.exec(statement.get());
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QList<double> sums(13, 0); // 12 mois + la somme des 12
        groups.insert(qMakePair(id,name), sums);
    }

    table->setRowCount(2); // Resultat et solde
    table->setVerticalHeaderLabels({ tr("Resultats"), tr("Solde") });

    table->setColumnCount(12 + 2); // 12 mois + moyenne de l'annee + total
    table->setHorizontalHeaderLabels({ tr("Janvier"), tr("Février"), tr("Mars"), tr("Avril"),
                                       tr("Mai"), tr("Juin"), tr("Juillet"), tr("Août"),
                                       tr("Septembre"), tr("Octobre"), tr("Novembre"), tr("Décembre"),
                                       tr("Moyenne"), tr("Total")});

    double results_sum = 0;
    QDate today = QDate::currentDate();

    for (int month = 0; month < 12; ++month)
    {
        double month_result = 0;

        QDate start = QDate(today.year(), month+1, 1);
        QDate end = QDate(today.year(), month+1, daysInMonth(month+1, today.year()));
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
                it_group.value()[12] += sum;
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
        if (sums.at(12) != 0)
        {
            table->insertRow(row);
            table->setVerticalHeaderItem(row, new QTableWidgetItem(cgroup.key().second));

            for (qsizetype i = 0; i < table->columnCount(); ++i) {
                double amount;
                if (i == 12)
                    amount = sums.at(12) / 12.0;
                else if (i == 13)
                    amount = sums.at(12);
                else
                    amount = sums.at(i);
                addItemInTable(amount, row, i);
            }

            ++row;
        }
        ++cgroup;
    }

    addItemInTable(results_sum/12.0, table->rowCount() - 2, 12); // Moyenne resultats
    addItemInTable(results_sum, table->rowCount() - 2, 13); // total resultats

    table->setItem(table->rowCount() - 1, 12, new QTableWidgetItem("-")); // Moyenne solde : sens ?
    addItemInTable(getBalanceByDate(today), table->rowCount() - 1, 13); // Solde actuel
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
