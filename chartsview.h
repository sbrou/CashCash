#ifndef CHARTSVIEW_H
#define CHARTSVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QChartView>
#include <QSqlRelationalTableModel>
#include <QPushButton>
#include <QPieSlice>
#include <QButtonGroup>

#include "customslice.h"

class ChartsView : public QWidget
{
    Q_OBJECT
public:
    explicit ChartsView(QSqlRelationalTableModel* mod, QWidget *parent = nullptr);

public slots:
    void updatePie();
    void updateChart(const QSqlRecord &);

private slots:
    void changeChart(int);

private:
    void changeSeries(QPieSeries *o_series, QPieSeries *n_series);


    QVBoxLayout *mainLayout;
    QChart *chart;
    QChartView *chartView;

    QSqlRelationalTableModel *model;

    QPieSeries *tags_series;
    QMap<int, CustomSlice*> tags_slices;

    QPieSeries *cats_series;
    QMap<int, CustomSlice *> cats_slices;

    QButtonGroup *buttons;
    QPushButton *qpbCats;
    QPushButton *qpbTags;
signals:

};

#endif // CHARTSVIEW_H
