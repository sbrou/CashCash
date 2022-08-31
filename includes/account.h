#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QWidget>
#include <QItemSelection>
#include <QToolBar>
#include <QGridLayout>

#include <QtSql>
#include <QtWidgets>
#include <QChartView>
#include <QSplitter>

#include "operationsview.h"
#include "customslice.h"
#include "chartsview.h"
#include "goalsview.h"
#include "catslist.h"

class Account : public QSplitter
{
    Q_OBJECT
public:
    // Methodes
    explicit Account(QWidget *parent = nullptr);

    void setStandardRules();
    QSqlError setStandardCategories();
    void saveSettings();
    void readSettings();

signals:
    void accountReady(const QString&);
    void selectionChanged(const QItemSelection&);
    void balanceChanged(double, double);

public slots:
    void initAccount();
    void importFile();
    void addOperation();
    void editOperation();
    void removeOperation();

    void saveFile(bool isNewFile = false);
    void saveAsFile();
    QSqlError loadFile(const QString& account_file = "");
    QSqlError createFile();

    void showCategories();
    void showTags();
    void showStats();

private slots:
    bool commitOnDatabase();

private:
    // Methodes
    void showError(const QSqlError &err);
    void updateBalance();
    void importCSV(const QString & filename);
    void importOFX(const QString & filename);

    // Attributs

    QLocale _locale;
    QString _title;
    double _init_balance;
    double _balance;
    double _future_balance;
    QString _filepath;

    QSplitter * splitter;
//    QGridLayout *accLayout;

    QSqlRelationalTableModel *model;
    QSqlTableModel *cats_model;
    QSqlTableModel *tags_model;
    int categoryIdx, tagIdx;
    unsigned _nbOperations;

    OperationsView *opsView;
    ChartsView *chartView;
    GoalsView *goalsView;

    CatsList *catsWidget;
    TagsList *tagsWidget;

    //////////////////////////////////////////////

    QMap<QString,QString> _rules;

    QDate dateFrom;
    QDate dateTo;

    QSqlError readCategories(const QString& query, const QJsonArray &catsArray);
    QSqlError readOperations(const QJsonArray &opsArray);

};

#endif // ACCOUNT_H
