#include "catschart.h"

#include <QLegendMarker>
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

    qDebug() << "--------------------------------------------------------";
    categories = _series->slices();
    for (auto cat = categories.begin(); cat != categories.end(); ++cat)
    {
        qDebug() << (*cat)->label() << (*cat)->value();
    }

    updateLegendMarkers();
    _chart->legend()->show();
    resize(QSize(400,400));
}

void CatsChart::updateLegendMarkers()
{
    const auto markers = _chart->legend()->markers(_series);
    qDebug() << "before update";
    for (QLegendMarker *marker : markers) {
        QPieLegendMarker *pieMarker = qobject_cast<QPieLegendMarker *>(marker);
        qDebug() << pieMarker->label();
    }

    for (QLegendMarker *marker : markers) {
        QPieLegendMarker *pieMarker = qobject_cast<QPieLegendMarker *>(marker);
//        qDebug() << pieMarker->slice()->label() << pieMarker->slice()->value();
        pieMarker->setLabel(QString("%1 %2€ %3%")
                            .arg(pieMarker->slice()->label())
                            .arg(pieMarker->slice()->value())
                            .arg(pieMarker->slice()->percentage() * 100, 0, 'f', 2));
//        pieMarker->setFont(QFont("Arial", 8));
//        pieMarker->setVisible(true);
    }
    qDebug() << "after update";
    for (QLegendMarker *marker : markers) {
        QPieLegendMarker *pieMarker = qobject_cast<QPieLegendMarker *>(marker);
        qDebug() << pieMarker->label();
    }
}
