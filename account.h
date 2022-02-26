#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QWidget>

#include "operation.h"
#include "category.h"
#include "operationsmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Account; }
QT_END_NAMESPACE

class Account : public QWidget
{
    Q_OBJECT
public:
    // Methodes
    explicit Account(QString title, QWidget *parent = nullptr);
    QString getOperationCategory(const QString &des);

public slots:
    void importFile();
    void addOperation(QDate date, const QString &des, double amount, const QString &cat);

signals:

private:
    // Attributs
    Ui::Account *ui;

    QLocale _locale;
    QString _title;
    QList<Category*> _categories;

    QMap<QString,QString> _rules;
    QMap<QString,QString> _opsType;

    OperationsTableModel* opsModel;


    // Methodes
    Operation process_line(QString line);

};

#endif // ACCOUNT_H
