#ifndef CHARTSVIEW_H
#define CHARTSVIEW_H

#include <QWidget>
#include <QGridLayout>
#include <QChartView>
#include <QPushButton>
#include <QPieSlice>
#include <QButtonGroup>
#include <QComboBox>
#include <QDate>

#include "sqlrelationalmodel.h"
#include "customslice.h"
#include "defines.h"

class ChartsView : public QWidget
{
    Q_OBJECT
public:
    explicit ChartsView(SqlRelationalTableModel* mod, QWidget *parent = nullptr);

public slots:
    void updatePie();

private slots:
    void changeChart(int);
    void changeTimePeriod(int);

private:
    void populateSeries(GroupType type, QPieSeries& series);
    void changeSeries(QPieSeries *o_series, QPieSeries *n_series);


    QGridLayout *mainLayout;
    QChart *chart;
    QChartView *chartView;

    SqlRelationalTableModel *model;

    QPieSeries *tags_series;
    QPieSeries *cats_series;
    QPieSeries *visible_series;

    QButtonGroup *buttons;
    QPushButton *qpbCats;
    QPushButton *qpbTags;
    QComboBox *qcbPeriod;
    QDate beginDate;
    QDate endDate;
signals:

};

#endif // CHARTSVIEW_H
