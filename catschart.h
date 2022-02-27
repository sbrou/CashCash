#ifndef CATSCHART_H
#define CATSCHART_H

#include <QChartView>
#include <QPieSeries>
#include <QChart>
#include <QLegend>

#include "category.h"

class CatsChart : public QChartView
{
public:
    CatsChart(Categories *cats, QWidget * parent = nullptr);
    void updateSlice(const QString & label, double value);

private:
    Categories* _cats;

    QPieSeries *_series;
};

#endif // CATSCHART_H
