#ifndef CHARTSVIEW_H
#define CHARTSVIEW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QChartView>
#include <QSqlRelationalTableModel>

#include "drilldownchart.h"
#include "drilldownslice.h"

class ChartsView : public QWidget
{
    Q_OBJECT
public:
    explicit ChartsView(QSqlRelationalTableModel* mod, QWidget *parent = nullptr);

public slots:
    void updatePie();

private:
    QHBoxLayout *mainLayout;
    DrilldownChart *pie;
    QChartView *chartView;

    QSqlRelationalTableModel *model;

signals:

};

#endif // CHARTSVIEW_H
