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

#include "defines.h"
#include "operationsview.h"
#include "customslice.h"
#include "chartsview.h"
#include "goalsview.h"
#include "catslist.h"
#include "statswidget.h"

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

    AccountState state();
    QString title();

signals:
    void accountReady(const QString&);
    void selectionChanged(const QItemSelection&);
    void balanceChanged(float, float);
    void stateChanged(bool isModified);

public slots:
    QSqlError initDatabase();
    void initAccount();
    void importFile();
    void addOperation();
    void editOperation();
    void removeOperation();

    void changeState(AccountState newState);
    void saveFile(bool isNewFile = false);
    void saveAsFile();
    QSqlError loadFile(const QString& account_file);
    QSqlError createFile(const QString & title, float balance);

    void showCategories();
    void showTags();
    void manageGoals();
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
    float _init_balance;
    float _balance;
    float _future_balance;
    QString _filepath;
    AccountState _state;

    QSplitter * splitter;

    QSqlRelationalTableModel *model;
    QSqlTableModel *cats_model;
    QSqlTableModel *tags_model;
    unsigned _nbOperations;

    OperationsView *opsView;
    ChartsView *chartView;
    GoalsView *goalsView;
    StatsWidget *statsWidget;

    CatsList *catsWidget;
    TagsList *tagsWidget;

    //////////////////////////////////////////////

    QMap<QString,QString> _rules;

    QDate dateFrom;
    QDate dateTo;

    QSqlError readCategories(const QString& query, const QJsonArray &catsArray);
    QSqlError readOperations(const QJsonArray &opsArray);
    void readGoals(const QJsonArray &goalsArray);

};

#endif // ACCOUNT_H
