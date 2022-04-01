#include "chartsview.h"

#include <QPieSeries>
#include <QSqlQuery>

ChartsView::ChartsView(QSqlRelationalTableModel *mod, QWidget *parent)
    : QWidget{parent}
    , model(mod)
{
    mainLayout = new QHBoxLayout(this);

    pie = new DrilldownChart();
//    pie->setTheme(QChart::ChartThemeLight);
    pie->setAnimationOptions(QChart::AllAnimations);
    pie->legend()->setVisible(true);
    pie->legend()->setAlignment(Qt::AlignRight);

    updatePie();

    chartView = new QChartView(pie);
    chartView->setRenderHint(QPainter::Antialiasing);

    mainLayout->addWidget(chartView);
}

void ChartsView::updatePie()
{
    QPieSeries *tagsSeries = new QPieSeries(this);
    tagsSeries->setName("Tags");


    model->select();
//    QSqlQuery qu("SELECT * FROM operations WHERE category=1");
//    while (qu.next()) {
//        qDebug() << "tag : " << qu.value(0);
//        qDebug() << "      " << qu.value(1);
//        qDebug() << "      " << qu.value(2);
//        qDebug() << "      " << qu.value(3);
//        qDebug() << "      " << qu.value(4);
//        qDebug() << "      " << qu.value(5);
//    }

    QMap<int,QString> map_tags;
    QMap<int,QColor> tags_colors;
    QSqlQuery q("SELECT * FROM tags WHERE type=0");
    while (q.next()) {
//        qDebug() << "tag : " << q.value(0);
//        qDebug() << "tag : " << q.value(1);
//        qDebug() << "tag : " << q.value(2);
//        qDebug() << "tag : " << q.value(3);
        map_tags.insert(q.value(0).toInt(), q.value(1).toString());
        QString color = QString("#%1").arg(q.value(2).toString());
//        qDebug() << color << QColor(color);
        tags_colors.insert(q.value(0).toInt(), QColor(q.value(2).toString()));
    }

    QMap<int,QString> map_cats;
    QMap<int,QColor> cats_colors;
    q.exec("SELECT * FROM categories WHERE type=0");
    while (q.next()) {
        //        qDebug() << "cat : " << q.value(0);
        //        qDebug() << "cat " << q.value(1);
        map_cats.insert(q.value(0).toInt(), q.value(1).toString());
        QString color = QString("#%1").arg(q.value(2).toString());
        //        qDebug() << color << QColor(color);
        cats_colors.insert(q.value(0).toInt(), QColor(q.value(2).toString()));
    }

    for (QMap<int,QString>::const_iterator tag = map_tags.constBegin(); tag != map_tags.constEnd(); ++tag)
    {
        QPieSeries *series = new QPieSeries(this);
        series->setName("Categories in " + tag.value());

        q.exec(QString("SELECT category, SUM (amount) FROM operations WHERE (tag=%1 AND type=0) GROUP BY category").arg(tag.key()));
//        qDebug() << "BOUCLE : " << tag.key() << " : " << tag.value();
        while (q.next()) {
//            qDebug() << q.value(0);
//            qDebug() << q.value(1);
            int id = q.value(0).toInt();
            *series << new DrilldownSlice(q.value(1).toDouble(), map_cats[id], cats_colors[id], tagsSeries);
        }

        QObject::connect(series, &QPieSeries::clicked, pie, &DrilldownChart::handleSliceClicked);

        *tagsSeries << new DrilldownSlice(series->sum(), tag.value(), cats_colors[tag.key()], series);
    }

    QObject::connect(tagsSeries, &QPieSeries::clicked, pie, &DrilldownChart::handleSliceClicked);

    pie->changeSeries(tagsSeries);
}
