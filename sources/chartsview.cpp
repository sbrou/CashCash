#include "chartsview.h"

#include <QPieSeries>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QGroupBox>

#include "utilities.h"

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
    chart->removeSeries(visible_series);

    tags_series->clear();
    cats_series->clear();

    model->select();

    populateSeries("categories", "category", beginDate, endDate, *cats_series);
    populateSeries("tags", "tag", beginDate, endDate, *tags_series);

    chart->addSeries(visible_series);
    chart->setTitle(visible_series->name());
}



void ChartsView::populateSeries(const QString& table, const QString& key, const QDate& begin, const QDate& end,
                                QPieSeries& series)
{
    QSqlQuery q(model->database());
    QString date = QString("op_date>='%1' AND op_date<='%2' AND ")
            .arg(beginDate.toString(Qt::ISODateWithMs))
            .arg(endDate.toString(Qt::ISODateWithMs));

    q.exec("SELECT * FROM " + table + " WHERE type=0");
    while (q.next()) {
        int id = q.value(0).toInt();
        QString name = q.value(1).toString();
        QString color = q.value(2).toString();

        QSqlQuery query(model->database());
        query.exec(QString("SELECT SUM (amount) FROM operations WHERE " + date + key +"=%1").arg(id));
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
    QDate today = QDate::currentDate();
    int year = today.year();
    int month = today.month();
    int pre_month = month - 1;

    if (pre_month == 0) pre_month = 12;

    switch (index)
    {
    case 1: // previous month
        beginDate = QDate(year, pre_month , 1);
        endDate = QDate(year, pre_month, daysInMonth(pre_month, year));
        break;
    case 2: // this year
        beginDate = QDate(year, 1, 1);
        endDate = QDate(year, 12, 31);
        break;
    case 3: // previous year
        beginDate = QDate(year-1, 1, 1);
        endDate = QDate(year-1, 12, 31);
        break;
    default: // this month (default : 0) or else
        beginDate = QDate(year, month, 1);
        endDate = QDate(year, month, today.daysInMonth());
    }

    updatePie();
}
