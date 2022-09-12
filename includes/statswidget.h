#ifndef STATSWIDGET_H
#define STATSWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QGroupBox>
#include <QTableWidget>
#include <QDialog>
#include <QComboBox>
#include <QDateEdit>

typedef QMap<QPair<int, QString>, QList<double> > SumsByGroup;



class StatsWidget : public QDialog
{
    Q_OBJECT
public:
    explicit StatsWidget(double balance, const QString & account_title, QWidget *parent = nullptr);
    double getBalanceByDate(QDate date);

private slots:
    void populateTable();
    void changeTimePeriod(int index);
    void setDateFrom(QDate);
    void setDateTo(QDate);

private:
    // Méthodes
    void addItemInTable(double amount, int row, int column);

    // Attributs

    QComboBox *qcbOpType;
    QComboBox *qcbGroupType;

    QComboBox *qcbPeriod;
    QDateEdit *qdeDateFrom;
    QDate dateFrom;
    QDateEdit *qdeDateTo;
    QDate dateTo;

    QTableWidget *table;

    double _init_balance;
    QString _account_name;

};

#endif // STATSWIDGET_H
