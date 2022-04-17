#include "chartsview.h"

#include <QPieSeries>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QGroupBox>

ChartsView::ChartsView(QSqlRelationalTableModel *mod, QWidget *parent)
    : QWidget{parent}
    , model(mod)
{
    mainLayout = new QVBoxLayout(this);

    qpbCats = new QPushButton(tr("Categories"), this);
    qpbCats->setToolTip(tr("Show categories chart"));
    qpbCats->setCheckable(true);
    qpbCats->setChecked(true);

    qpbTags = new QPushButton(tr("Tags"));
    qpbTags->setToolTip(tr("Show tags chert"));
    qpbTags->setCheckable(true);

    buttons = new QButtonGroup(this);
    buttons->setExclusive(true);
    buttons->addButton(qpbCats, 1);
    buttons->addButton(qpbTags, 2);
    connect(buttons, SIGNAL(idClicked(int)), this, SLOT(changeChart(int)));

    QGroupBox * qgbButtons = new QGroupBox(this);
    QHBoxLayout * buttonsLayout = new QHBoxLayout(qgbButtons);
    buttonsLayout->addWidget(qpbCats);
    buttonsLayout->addWidget(qpbTags);

    mainLayout->addWidget(qgbButtons, 0, Qt::AlignRight);

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
//            qDebug() << query.value(0).toDouble();
            CustomSlice *slice = new CustomSlice(query.value(0).toDouble(), name, QColor(color));
            *cats_series << slice;
            cats_slices.insert(id, slice);
        }
    }

    chart->addSeries(cats_series);
    chart->setTitle(cats_series->name());

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
    QMap<int, CustomSlice*>::iterator it_tag = tags_slices.find(tag);
    if ( it_tag != tags_slices.end())
    {
        it_tag.value()->updateValue(amount);
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
    QMap<int, CustomSlice*>::iterator it_cat = cats_slices.find(cat);
    if ( it_cat != cats_slices.end())
    {
        it_cat.value()->updateValue(amount);
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

void ChartsView::changeSeries(QPieSeries *o_series, QPieSeries *n_series)
{
    chart->removeSeries(o_series);
    chart->addSeries(n_series);
    chart->setTitle(n_series->name());
}

void ChartsView::changeChart(int id)
{
    switch (id)
    {
    case 1: // show cats
        changeSeries(tags_series, cats_series);
        break;
    case 2: // show tags
        changeSeries(cats_series, tags_series);
        break;
    }
}
