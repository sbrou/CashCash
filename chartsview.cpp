#include "chartsview.h"

#include <QPieSeries>
#include <QSqlQuery>
#include <QSqlRecord>

ChartsView::ChartsView(QSqlRelationalTableModel *mod, QWidget *parent)
    : QWidget{parent}
    , model(mod)
{
    mainLayout = new QHBoxLayout(this);

    chart = new QChart;
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    tags_series = new QPieSeries(this);
    tags_series->setName("Tags");

    cats_series = new QPieSeries(this);
    cats_series->setName("Categories");

    model->select();

    QSqlQuery q;
    q.exec("SELECT * FROM tags WHERE type=0");
    while (q.next()) {
        int id = q.value(0).toInt();
        QString name = q.value(1).toString();
        QString color = q.value(2).toString();

        QSqlQuery query;
        query.exec(QString("SELECT SUM (amount) FROM operations WHERE tag=%1").arg(id));
        while (query.next()) {
            CustomSlice *slice = new CustomSlice(query.value(0).toDouble(), name, QColor(color));
            *tags_series << slice;
            tags_slices.insert(id, slice);
        }
    }

    q.exec("SELECT * FROM categories WHERE type=0");
    while (q.next()) {
        int id = q.value(0).toInt();
        QString name = q.value(1).toString();
        QString color = q.value(2).toString();

        QSqlQuery query;
        query.exec(QString("SELECT SUM (amount) FROM operations WHERE category=%1").arg(id));
        while (query.next()) {
            qDebug() << query.value(0).toDouble();
            CustomSlice *slice = new CustomSlice(query.value(0).toDouble(), name, QColor(color));
            *cats_series << slice;
            cats_slices.insert(id, slice);
        }
    }

    chart->addSeries(cats_series);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    mainLayout->addWidget(chartView);
}

void ChartsView::updatePie()
{

}


void ChartsView::updateChart(const QSqlRecord & record)
{
    int cat = record.value(2).toInt();
    int tag = record.value(4).toInt();
    double amount = record.value(3).toDouble();

    // tag
    QMap<int, QPieSlice*>::iterator it_tag = tags_slices.find(tag);
    if ( it_tag != tags_slices.end())
    {
        CustomSlice * slice = static_cast<CustomSlice *>(it_tag.value());
        slice->updateValue(amount);
    }
    else
    {
        QSqlQuery q;
        q.exec(QString("SELECT * FROM tags WHERE id=%1").arg(tag));
        while (q.next()) {
            CustomSlice *slice = new CustomSlice(amount, q.value(1).toString(), QColor(q.value(2).toString()));
            *tags_series << slice;
            tags_slices.insert(tag, slice);
        }
    }

    // category
    QMap<int, QPieSlice*>::iterator it_cat = cats_slices.find(cat);
    if ( it_cat != cats_slices.end())
    {
        CustomSlice * slice = static_cast<CustomSlice *>(it_cat.value());
        slice->updateValue(amount);
    }
    else
    {
        QSqlQuery q;
        q.exec(QString("SELECT * FROM categories WHERE id=%1").arg(cat));
        while (q.next()) {
            CustomSlice *slice = new CustomSlice(amount, q.value(1).toString(), QColor(q.value(2).toString()));
            *cats_series << slice;
            cats_slices.insert(cat, slice);
        }
    }
}
