#ifndef STATSWIDGET_H
#define STATSWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QGroupBox>
#include <QTableWidget>
#include <QDialog>

typedef QMap<int, QPair<QString, double> > TagsMap; // QMap<id_tag, QPair<name_tag,sum_tag> >

class StatsWidget : public QDialog
{
    Q_OBJECT
public:
    explicit StatsWidget(double balance, QWidget *parent = nullptr);
    ~StatsWidget();

signals:

private:
    // Méthodes
    void populateTable();

    // Attributs

    QGridLayout *mainLayout;

    QGroupBox *settings;
    QGroupBox *dateFilter;
    QTableWidget *table;

    double _balance;


};

#endif // STATSWIDGET_H
