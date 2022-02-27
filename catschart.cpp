#include "catschart.h"

#include <QPieLegendMarker>

CatsChart::CatsChart(Categories *cats, QWidget *parent)
    : QChartView(parent),
      _cats(cats)
{
    _series = new QPieSeries();
    for (auto it = _cats->begin(); it != _cats->end(); ++it) {
        if (it.value()->type() == Category::SPENDING)
            _series->append(it.key(),it.value()->amount());
    }

    _series->setLabelsVisible(false);

    _chart = new QChart();
    _chart->addSeries(_series);
    _chart->legend()->setMarkerShape(QLegend::MarkerShapeCircle);
    _chart->legend()->setAlignment(Qt::AlignRight);

    setChart(_chart);
    setRenderHint(QPainter::Antialiasing);

    updateLegendMarkers();

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

    updateLegendMarkers();
}

void CatsChart::updateLegendMarkers()
{
    const auto markers = _chart->legend()->markers(_series);
    for (QLegendMarker *marker : markers) {
        QPieLegendMarker *pieMarker = qobject_cast<QPieLegendMarker *>(marker);
        pieMarker->setLabel(QString("%1 %2€ %3%")
                            .arg(pieMarker->slice()->label())
                            .arg(pieMarker->slice()->value())
                            .arg(pieMarker->slice()->percentage() * 100, 0, 'f', 2));
//        pieMarker->setFont(QFont("Arial", 8));
    }

}
