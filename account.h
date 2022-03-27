#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QWidget>
#include <QItemSelection>
#include <QToolBar>
#include <QGridLayout>

#include <QtSql>
#include <QtWidgets>
#include <QChartView>

#include "operationsview.h"
#include "drilldownchart.h"
#include "drilldownslice.h"
#include "chartsview.h"

class Account : public QWidget
{
    Q_OBJECT
public:
    // Methodes
    explicit Account(QString title, QWidget *parent = nullptr);

    void setStandardRules();
    void setStandardCategories();

signals:
    void accountReady();
    void selectionChanged(const QItemSelection& );

public slots:
    void importFile();
    void addOperation();
    void editOperation();
    void removeOperation();
    void addCategory();
    void selectTest();

    void saveFile();
    QSqlError loadFile();

private slots:
    void activateDateFilter(bool on);
    void applyFromDateFilter(QDate);
    void applyToDateFilter(QDate);

    void activateCatFilter(bool on);
    void applyCatFilter(int);

    void activateTagFilter(bool on);
    void applyTagFilter(int);

private:
    // Methodes
    bool commitOnDatabase();
    void showError(const QSqlError &err);
    void initAccount();

    // Attributs

    QLocale _locale;
    QString _title;

    QGridLayout *accLayout;

    QSqlRelationalTableModel *model;
    int categoryIdx, tagIdx;
    unsigned _nbOperations;

    OperationsView *opsView;
    ChartsView *chartView;

    //////////////////////////////////////////////

    QMap<QString,QString> _rules;

    void createToolBar();
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
