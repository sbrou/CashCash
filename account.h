#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QWidget>
#include <QItemSelection>
#include <QToolBar>
#include <QGridLayout>

#include <QtSql>
#include <QtWidgets>
#include <QChartView>

#include "drilldownchart.h"
#include "drilldownslice.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Account; }
QT_END_NAMESPACE

class Account : public QWidget
{
    Q_OBJECT
public:
    // Methodes
    explicit Account(QString title, QWidget *parent = nullptr);

    void setStandardRules();
    void setStandardCategories();

public slots:
    void importFile();
    void addOperation();
    void editOperation();
    void removeOperation();
    void addCategory();
    void selectTest();

private slots:
    void update_actions(const QItemSelection& selected);

    void activateDateFilter(bool on);
    void applyFromDateFilter(QDate);
    void applyToDateFilter(QDate);

    void activateCatFilter(bool on);
    void applyCatFilter(int);

    void activateTagFilter(bool on);
    void applyTagFilter(int);

private:
    // Attributs
    Ui::Account *ui;

    QLocale _locale;
    QString _title;

    QMap<QString,QString> _rules;

    DrilldownChart *chart;
    QChartView *chartView;

    void createToolBar();
    void updatePie();
    void initTabFilters();

    void setDateFilter();

    QToolBar *toolBar;
    QAction *importOpAct;
    QAction *addOpAct;
    QAction *editOpAct;
    QAction *removeOpAct;
    QAction *addCatAct;
    QAction *manBudgetAct;


    bool commitOnDatabase();
    void showError(const QSqlError &err);
    QSqlRelationalTableModel *model;
    int categoryIdx, tagIdx;
    unsigned _nbOperations;

    QDataWidgetMapper * filterMapper;
    QDate dateFrom;
    QDate dateTo;

//    QGridLayout *accountLayout;
};

#endif // ACCOUNT_H
