#ifndef STATSWIDGET_H
#define STATSWIDGET_H

#include <QWidget>
#include <QDate>


typedef QMap<QPair<int, QString>, QList<float> > SumsByGroup;

namespace Ui {
class StatsWidget;
}

class StatsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StatsWidget(float balance, const QString & account_title, QWidget *parent = nullptr);
    float getBalanceByDate(QDate date);

public slots:
    void populateTable();

private slots:
    void changeTimePeriod(int index);
    void changeCustomPeriod();

private:
    // Méthodes
    void addItemInTable(float amount, int row, int column);

    // Attributs

    Ui::StatsWidget *ui;

    QDate dateFrom;
    QDate dateTo;

    float _init_balance;
    QString _account_name;

};

#endif // STATSWIDGET_H
