#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QWidget>
#include <QItemSelection>
#include <QToolBar>

#include "operation.h"
#include "category.h"
#include "operationsmodel.h"
#include "catschart.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Account; }
QT_END_NAMESPACE

typedef QMap<QString,Category*> Categories;

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

signals:

private slots:
    void update_actions(const QItemSelection& selected);

private:
    // Attributs
    Ui::Account *ui;

    QLocale _locale;
    QString _title;

    Categories _categories;

    QMap<QString,QString> _rules;
    QMap<QString,QString> _opsType;

    OperationsTableModel* opsModel;
//    CatsChart* catsPie;

    void createToolBar();

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

};

#endif // ACCOUNT_H
