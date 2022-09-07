#ifndef STATSWIDGET_H
#define STATSWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QGroupBox>
#include <QTableWidget>
#include <QDialog>
#include <QComboBox>

typedef QMap<QPair<int, QString>, QList<double> > SumsByGroup; // QMap<id_tag, QPair<name_tag,sum_tag> >



class StatsWidget : public QDialog
{
    Q_OBJECT
public:
    explicit StatsWidget(double balance, const QString & account_title, QWidget *parent = nullptr);
    ~StatsWidget();

    double getBalanceByDate(QDate date);

private slots:
    void populateTable();

private:
    // Méthodes
    void addItemInTable(double amount, int row, int column);

    // Attributs

    QGridLayout *mainLayout;

    QGroupBox *settings;
    QComboBox *qcbOpType;
    QComboBox *qcbGroupType;

    QGroupBox *dateFilter;
    QTableWidget *table;

    double _init_balance;
    QString _account_name;

};

#endif // STATSWIDGET_H
