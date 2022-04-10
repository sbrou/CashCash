#ifndef CHARTSVIEW_H
#define CHARTSVIEW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QChartView>
#include <QSqlRelationalTableModel>
#include <QPushButton>
#include <QPieSlice>

#include "customslice.h"

class ChartsView : public QWidget
{
    Q_OBJECT
public:
    explicit ChartsView(QSqlRelationalTableModel* mod, QWidget *parent = nullptr);

public slots:
    void updatePie();
    void updateChart(const QSqlRecord &);

private:
    QHBoxLayout *mainLayout;
    QChart *chart;
    QChartView *chartView;

    QSqlRelationalTableModel *model;

    QPieSeries *tags_series;
    QMap<int, QPieSlice*> tags_slices;

    QPieSeries *cats_series;
    QMap<int, QPieSlice *> cats_slices;


signals:

};

#endif // CHARTSVIEW_H
