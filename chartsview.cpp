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
    qpbTags->setToolTip(tr("Show tags chart"));
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

    visible_series = cats_series;

    updatePie();

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    mainLayout->addWidget(chartView);
}

void ChartsView::updatePie()
{
    tags_series->clear();
    cats_series->clear();

    model->select();

    populateSeries("categories", "category", *cats_series);
    populateSeries("tags", "tag", *tags_series);

    changeSeries(visible_series, visible_series);
}



void ChartsView::populateSeries(const QString& table, const QString& key, QPieSeries& series)
{
    QSqlQuery q;
    q.exec("SELECT * FROM " + table + " WHERE type=0");
    while (q.next()) {
        int id = q.value(0).toInt();
        QString name = q.value(1).toString();
        QString color = q.value(2).toString();

        QSqlQuery query;
        query.exec(QString("SELECT SUM (amount) FROM operations WHERE " + key +"=%1").arg(id));
        while (query.next()) {
            qreal amount = query.value(0).toDouble();
            if (amount != 0)
            {
                CustomSlice *slice = new CustomSlice(amount, name, QColor(color));
                series << slice;
            }
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
