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

    table->setRowCount(nb_tags + 3); // + tous les earnings + resultat du mois + solde
    QStringList vertical_labels;
    vertical_labels << tr("Revenus");
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

    double earnings_sum = 0;
    double results_sum = 0;

    for (int month = 0; month < 12; ++month) // 12 mois
    {
        double result = 0;

        QDate today = QDate::currentDate();
        QDate start = QDate(today.year(), month+1, 1);
        QDate end = QDate(today.year(), month+1, daysInMonth(month+1, today.year()));

        QString date = QString("op_date>='%1' AND op_date<='%2' AND ")
                .arg(start.toString(Qt::ISODateWithMs))
                .arg(end.toString(Qt::ISODateWithMs));

        int row = 0;
        query.exec(QString("SELECT SUM (amount) FROM operations WHERE " + date + "type=1")); // Revenus
        while (query.next()) {
            result += query.value(0).toDouble();
            earnings_sum += query.value(0).toDouble();
            QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(query.value(0).toDouble()));
            table->setItem(row++, month, newItem);
        }

        TagsMap::iterator tag = tags.begin();
        while (tag != tags.end()) {
            query.exec(QString("SELECT SUM (amount) FROM operations WHERE " + date + "tag=%1").arg(tag.key())); // Tag
            while (query.next()) {
                result += query.value(0).toDouble();
                tag.value().second += query.value(0).toDouble();
                QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(query.value(0).toDouble()));
                table->setItem(row++, month, newItem);
            }
            ++tag;
        }

        results_sum += result;
        table->setItem(row++, month, new QTableWidgetItem(QString::number(result))); // Resultat
        table->setItem(row++, month, new QTableWidgetItem(QString::number(0))); // Solde (pas bon à revoir)
    }

    int row = 0;
    table->setItem(row, 12, new QTableWidgetItem(QString::number(earnings_sum/12.0))); // Moyenne revenus
    table->setItem(row++, 13, new QTableWidgetItem(QString::number(earnings_sum))); // total revenus

    TagsMap::const_iterator ctag = tags.constBegin();
    while (ctag != tags.constEnd()) {
        table->setItem(row, 12, new QTableWidgetItem(QString::number(ctag.value().second/12.0))); // Moyenne tag
        table->setItem(row++, 13, new QTableWidgetItem(QString::number(ctag.value().second))); // total tag
        ++ctag;
    }

    table->setItem(1+nb_tags, 12, new QTableWidgetItem(QString::number(results_sum/12.0))); // Moyenne resultats
    table->setItem(1+nb_tags, 13, new QTableWidgetItem(QString::number(results_sum))); // total resultats

    table->setItem(2+nb_tags, 12, new QTableWidgetItem("-")); // Moyenne solde : sens ?
    table->setItem(2+nb_tags, 13, new QTableWidgetItem(QString::number(0))); // solde actuel
}
