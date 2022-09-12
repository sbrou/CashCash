#include "chartsview.h"

#include <QPieSeries>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QGroupBox>

#include "utilities.h"

using namespace Utilities;

ChartsView::ChartsView(QSqlRelationalTableModel *mod, QWidget *parent)
    : QWidget{parent}
    , model(mod)
{
    mainLayout = new QGridLayout(this);

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
    qgbButtons->setFlat(true);
    QHBoxLayout * buttonsLayout = new QHBoxLayout(qgbButtons);
    buttonsLayout->addWidget(qpbCats);
    buttonsLayout->addWidget(qpbTags);

    mainLayout->addWidget(qgbButtons, 0, 2);

    qcbPeriod = new QComboBox(this);
    qcbPeriod->addItems({ tr("Ce Mois"), tr("Le mois dernier"), tr("Cette année"), tr("L'année dernière") });
    qcbPeriod->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(qcbPeriod, SIGNAL(currentIndexChanged(int)), this, SLOT(changeTimePeriod(int)));
    mainLayout->addWidget(qcbPeriod, 0, 3);

    chart = new QChart;
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    tags_series = new QPieSeries(this);
    tags_series->setName("Tags");

    cats_series = new QPieSeries(this);
    cats_series->setName("Categories");

    visible_series = cats_series;

    changeTimePeriod(0);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    mainLayout->addWidget(chartView, 1, 0, 1, 4);
}

void ChartsView::updatePie()
{
    if (chart->series().contains(visible_series))
        chart->removeSeries(visible_series);

    tags_series->clear();
    cats_series->clear();

    model->select();

    populateSeries(CatType, *cats_series);
    populateSeries(TagType, *tags_series);

    chart->addSeries(visible_series);
    chart->setTitle(visible_series->name());
}



void ChartsView::populateSeries(GroupType type, QPieSeries& series)
{
    QMap<double, CustomSlice *> sortedSlices;

    QueryStatement query_statement(SELECT_SUM);
    query_statement.addCondition(lowerDateCondition(beginDate));
    query_statement.addCondition(upperDateCondition(endDate));

    QSqlQuery q(model->database());
    q.exec(QueryStatement(selectGroupCmd(type), typeCondition((OpType)0)).get());
    while (q.next()) {
        int id = q.value(0).toInt();
        QString name = q.value(1).toString();
        QString color = q.value(2).toString();

        QSqlQuery query(model->database());
        query_statement.addCondition(groupCondition(type, id));
        query.exec(query_statement.get());
        while (query.next()) {
            qreal amount = query.value(0).toDouble();
            if (amount != 0)
            {
                CustomSlice *slice = new CustomSlice(amount, name, QColor(color));
                sortedSlices.insert(amount, slice);
            }
        }
        query_statement.clearConditions(2);
    }

    foreach (CustomSlice* slice, sortedSlices)
        series << slice;
}

void ChartsView::changeSeries(QPieSeries *o_series, QPieSeries *n_series)
{
    if (chart->series().contains(o_series))
        chart->removeSeries(o_series);
    chart->addSeries(n_series);
    chart->setTitle(n_series->name());
    visible_series = n_series;
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

void ChartsView::changeTimePeriod(int index)
{
    switch (index)
    {
    case 1: // previous month
        getTimePeriod(PreviousMonth,beginDate,endDate);
        break;
    case 2: // this year
        getTimePeriod(CurrentYear,beginDate,endDate);
        break;
    case 3: // previous year
        getTimePeriod(PreviousYear,beginDate,endDate);
        break;
    default: // this month (default : 0) or else
        getTimePeriod(CurrentMonth,beginDate,endDate);
    }

    updatePie();
}
