#ifndef RULESLIST_H
#define RULESLIST_H

#include <QDialog>
#include <QSqlTableModel>

#include "tablewidget.h"
#include "rulesdialog.h"

class RulesList : public QDialog
{
    Q_OBJECT

public:
    explicit RulesList(QSqlTableModel * mod_cats, QSqlTableModel * mod_tags, QWidget *parent = nullptr);

public slots:
    void addRule();
    void editRule();
    void removeRule();

private:
    // Attributs
    QSqlTableModel *cats, *tags;
    TableWidget *rulesWidget;

    void setRuleInModel(int row, const RulesDialog &);
};

#endif // RULESLIST_H
