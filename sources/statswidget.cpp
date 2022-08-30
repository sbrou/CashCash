#include "statswidget.h"

#include <QSqlQuery>
#include <QPair>
#include <QDate>

#include "utilities.h"

StatsWidget::StatsWidget(double balance, QWidget *parent)
    : QDialog{parent}
    , _balance(balance)
{
    mainLayout = new QGridLayout(this);

    settings = new QGroupBox;
    mainLayout->addWidget(settings, 0, 0);

    dateFilter = new QGroupBox;
    mainLayout->addWidget(dateFilter, 1, 0);

    table = new QTableWidget;
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    populateTable();
    mainLayout->addWidget(table, 0, 1, 3, 5);
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

    TagsMap tags; // QMap<id_tag, QPair<name_tag,sum_tag> >
    QSqlQuery query;
    query.exec("SELECT * FROM tags WHERE type=0");
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        tags.insert(id, qMakePair(name,0));
    }

    int nb_tags = tags.size();

    table->setRowCount(nb_tags + 2); // + resultat du mois + solde
    QStringList vertical_labels;
    TagsMap::iterator tag = tags.begin();
    while (tag != tags.end()) {
        vertical_labels << tag.value().first;
        ++tag;
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
        TagsMap::iterator tag = tags.begin();
        while (tag != tags.end()) {
            query.exec(QString("SELECT SUM (amount) FROM operations WHERE " + date + "tag=%1").arg(tag.key())); // Tag
            while (query.next()) {
                result += query.value(0).toDouble();
                tag.value().second += query.value(0).toDouble();
                addItemInTable(query.value(0).toDouble(), row++, month);
            }
            ++tag;
        }

        results_sum += result; 
        addItemInTable(result, row++, month); // Resultat
        addItemInTable(getBalanceByDate(end), row++, month); // Solde
    }

    int row = 0;
    TagsMap::const_iterator ctag = tags.constBegin();
    while (ctag != tags.constEnd()) {
        addItemInTable(ctag.value().second/12.0, row, 12); // Moyenne tag
        addItemInTable(ctag.value().second, row++, 13); // total tag
        ++ctag;
    }

    addItemInTable(results_sum/12.0, 1+nb_tags, 12); // Moyenne resultats
    addItemInTable(results_sum, 1+nb_tags, 13); // total resultats

    table->setItem(2+nb_tags, 12, new QTableWidgetItem("-")); // Moyenne solde : sens ?
    addItemInTable(getBalanceByDate(today), 2+nb_tags, 13); // Solde actuel
}

double StatsWidget::getBalanceByDate(QDate date)
{
    double balance = 0;
    QSqlQuery query;
    QString date_query = QString("op_date<='%1'").arg(date.toString(Qt::ISODateWithMs));
    query.exec(QString("SELECT SUM (amount) FROM operations WHERE " + date_query));
    while (query.next()) {
        balance = _balance + query.value(0).toDouble();
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
