#ifndef STATSWIDGET_H
#define STATSWIDGET_H

#include <QWidget>
#include <QDate>


typedef QMap<QPair<int, QString>, QList<double> > SumsByGroup;

namespace Ui {
class StatsWidget;
}

class StatsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StatsWidget(double balance, const QString & account_title, QWidget *parent = nullptr);
    double getBalanceByDate(QDate date);

public slots:
    void populateTable();

private slots:
    void changeTimePeriod(int index);
    void setDateFrom(QDate);
    void setDateTo(QDate);

private:
    // Méthodes
    void addItemInTable(double amount, int row, int column);

    // Attributs

    Ui::StatsWidget *ui;

    QDate dateFrom;
    QDate dateTo;

    double _init_balance;
    QString _account_name;

};

#endif // STATSWIDGET_H
