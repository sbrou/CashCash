#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QWidget>
#include <QItemSelection>
#include <QToolBar>

#include <QtSql>
#include <QtWidgets>

#include "operation.h"
#include "category.h"
#include "operationsmodel.h"
#include "catschart.h"

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

private slots:
    void update_actions(const QItemSelection& selected);

private:
    // Attributs
    Ui::Account *ui;

    QLocale _locale;
    QString _title;

    Categories _opsCategories;
    QMap<QString,QString> _opsType;

    Categories _catsTypes;
    QMap<QString,QString> _rules;

    OperationsTableModel* opsModel;
    CatsChart* catsPie;

    void createToolBar();
    void updateCatsPie();

    QToolBar *toolBar;
    QAction *importOpAct;
    QAction *addOpAct;
    QAction *editOpAct;
    QAction *removeOpAct;
    QAction *addCatAct;
    QAction *manBudgetAct;

    // Methodes
    void process_line(QString line);
    void add_operation(QDate date, const QString &des, double amount, const QString &cat);
    QString affect_category(const QString &des, double amount);


    void showError(const QSqlError &err);
    QSqlRelationalTableModel *model;
    int categoryIdx, tagIdx;
};

#endif // ACCOUNT_H
