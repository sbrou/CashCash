#ifndef CHARTSVIEW_H
#define CHARTSVIEW_H

#include <QWidget>
#include <QGridLayout>
#include <QChartView>
#include <QSqlRelationalTableModel>
#include <QPushButton>
#include <QPieSlice>
#include <QButtonGroup>
#include <QComboBox>
#include <QDate>

#include "customslice.h"

class ChartsView : public QWidget
{
    Q_OBJECT
public:
    explicit ChartsView(QSqlRelationalTableModel* mod, QWidget *parent = nullptr);

public slots:
    void updatePie();

private slots:
    void changeChart(int);
    void changeTimePeriod(int);

private:
    void populateSeries(const QString& table, const QString& key, const QDate &begin, const QDate &end, QPieSeries& series);
    void changeSeries(QPieSeries *o_series, QPieSeries *n_series);


    QGridLayout *mainLayout;
    QChart *chart;
    QChartView *chartView;

    QSqlRelationalTableModel *model;

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
