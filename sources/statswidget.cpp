#include "statswidget.h"

#include <QGuiApplication>
#include <QScreen>
#include <QSqlQuery>
#include <QPair>
#include <QDate>
#include <QFormLayout>

#include "utilities.h"

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

    QString table_name;
    QString group_name;
    int type;

    GroupsIdMap groups; // QMap<id_tag, QPair<name_tag,sum_tag> >
    QSqlQuery query(QSqlDatabase::database(_account_name));

    int groupTypeIndex = qcbGroupType->currentIndex();
    if (groupTypeIndex == 0) {
        table_name = "categories";
        group_name = "category";
    }
    else {
        table_name = "tags";
        group_name = "tag";
    }

    QString statement = QString("SELECT * FROM %1").arg(table_name);

    int opTypeIndex = qcbOpType->currentIndex();
    if (opTypeIndex != 2)
    {
        if (opTypeIndex == 0) // expenses
            type = 0;
        else if (opTypeIndex == 1) // earnings
            type = 1;
        statement += QString(" WHERE type=%2").arg(type);
    }

    query.exec(statement);
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        groups.insert(id, qMakePair(name,0));
    }

    int nb_groups = groups.size();

    table->setRowCount(nb_groups + 2); // + resultat du mois + solde
    QStringList vertical_labels;
    GroupsIdMap::iterator it_group = groups.begin();
    while (it_group != groups.end()) {
        vertical_labels << it_group.value().first;
        ++it_group;
    }
    vertical_labels << tr("Resultats") << tr("Solde");
    table->setVerticalHeaderLabels(vertical_labels);

    table->setColumnCount(12 + 2); // 12 mois + moyenne de l'annee + total
    table->setHorizontalHeaderLabels({ tr("Janvier"), tr("Février"), tr("Mars"), tr("Avril"),
                                       tr("Mai"), tr("Juin"), tr("Juillet"), tr("Août"),
                                       tr("Septembre"), tr("Octobre"), tr("Novembre"), tr("Décembre"),
                                       tr("Moyenne"), tr("Total")});

    double results_sum = 0;
    QDate today = QDate::currentDate();

    for (int month = 0; month < 12; ++month) // 12 mois
    {
        double result = 0;

        QDate start = QDate(today.year(), month+1, 1);
        QDate end = QDate(today.year(), month+1, daysInMonth(month+1, today.year()));

        QString date = QString("op_date>='%1' AND op_date<='%2' AND ")
                .arg(start.toString(Qt::ISODateWithMs))
                .arg(end.toString(Qt::ISODateWithMs));

        int row = 0;
        GroupsIdMap::iterator it_group = groups.begin();
        while (it_group != groups.end()) {
            query.exec(QString("SELECT SUM (amount) FROM operations WHERE " + date + "%1=%2")
                       .arg(group_name)
                       .arg(it_group.key()));
            while (query.next()) {
                result += query.value(0).toDouble();
                it_group.value().second += query.value(0).toDouble();
                addItemInTable(query.value(0).toDouble(), row++, month);
            }
            ++it_group;
        }

        results_sum += result; 
        addItemInTable(result, row++, month); // Resultat
        addItemInTable(getBalanceByDate(end), row++, month); // Solde
    }

    int row = 0;
    GroupsIdMap::const_iterator cgroup = groups.constBegin();
    while (cgroup != groups.constEnd()) {
        addItemInTable(cgroup.value().second/12.0, row, 12); // Moyenne tag
        addItemInTable(cgroup.value().second, row++, 13); // total tag
        ++cgroup;
    }

    addItemInTable(results_sum/12.0, 1+nb_groups, 12); // Moyenne resultats
    addItemInTable(results_sum, 1+nb_groups, 13); // total resultats

    table->setItem(2+nb_groups, 12, new QTableWidgetItem("-")); // Moyenne solde : sens ?
    addItemInTable(getBalanceByDate(today), 2+nb_groups, 13); // Solde actuel
}

double StatsWidget::getBalanceByDate(QDate date)
{
    double balance = 0;
    QSqlQuery query(QSqlDatabase::database(_account_name));
    QString date_query = QString("op_date<='%1'").arg(date.toString(Qt::ISODateWithMs));
    query.exec(QString("SELECT SUM (amount) FROM operations WHERE " + date_query));
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
