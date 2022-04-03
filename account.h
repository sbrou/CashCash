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
#include "drilldownchart.h"
#include "drilldownslice.h"
#include "chartsview.h"
#include "goalsview.h"
#include "catslist.h"

class Account : public QSplitter
{
    Q_OBJECT
public:
    // Methodes
    explicit Account(QString title, QWidget *parent = nullptr);

    void setStandardRules();
    void setStandardCategories();
    void saveSettings();
    void readSettings();

signals:
    void accountReady();
    void selectionChanged(const QItemSelection& );

public slots:
    void importFile();
    void addOperation();
    void editOperation();
    void removeOperation();
    void selectTest();

    void saveFile();
    QSqlError loadFile(const QString& account_file = "");

    void showCategories();

private slots:
    void activateDateFilter(bool on);
    void applyFromDateFilter(QDate);
    void applyToDateFilter(QDate);

    void activateCatFilter(bool on);
    void applyCatFilter(int);

    void activateTagFilter(bool on);
    void applyTagFilter(int);

    bool commitOnDatabase();

private:
    // Methodes
    void showError(const QSqlError &err);
    void initAccount();

    // Attributs

    QLocale _locale;
    QString _title;
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

    //////////////////////////////////////////////

    QMap<QString,QString> _rules;

    void initTabFilters();

    void setDateFilter();

    QToolBar *toolBar;
    QAction *importOpAct;
    QAction *addOpAct;
    QAction *editOpAct;
    QAction *removeOpAct;
    QAction *addCatAct;
    QAction *manBudgetAct;

    QDataWidgetMapper * filterMapper;
    QDate dateFrom;
    QDate dateTo;

    QSqlError readCategories(const QString& query, const QJsonArray &catsArray);
    QSqlError readOperations(const QJsonArray &opsArray);

};

#endif // ACCOUNT_H
