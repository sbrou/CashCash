#include "catschart.h"

CatsChart::CatsChart(Categories *cats, QWidget *parent)
    : QChartView(parent),
      _cats(cats)
{
    _series = new QPieSeries();
    for (auto it = _cats->begin(); it != _cats->end(); ++it) {
        qDebug() << "splice : " << it.key() << it.value()->amount();
        _series->append(it.key(),it.value()->amount());
    }

    _series->setLabelsVisible(false);

    QChart *chart = new QChart();
    chart->addSeries(_series);
    chart->setTitle("Categories");
    chart->legend()->setMarkerShape(QLegend::MarkerShapeCircle);

    setChart(chart);
    setRenderHint(QPainter::Antialiasing);

    setMinimumSize(386,258);

//    setVisible(false);
}

void CatsChart::updateSlice(const QString & label, double value)
{
    QList<QPieSlice *> categories = _series->slices();
    for (auto cat = categories.begin(); cat != categories.end(); ++cat)
    {
        if ((*cat)->label() == label)
            (*cat)->setValue(value);
    }
}
