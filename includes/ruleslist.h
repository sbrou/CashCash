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
    QStandardItemModel *model();
    void createRule(const QString & database, const QString &expr, bool isCaseSensitive, int catId, int tagId);
    void assignDescriptionToGroups(const QString & description, int & cat, int & tag);

signals:
    void changeState(AccountState);

public slots:
    void addRule();
    void editRule();
    void removeRule();

protected slots:
    void applyAction(Action);

private:
    // Attributs
    QSqlTableModel *cats, *tags;
    TableWidget *rulesWidget;

    void setRuleInModel(int row, const RulesDialog &);
    void setRuleItems(int row, QStandardItem *expr, QStandardItem *cat, QStandardItem *tag);
};

#endif // RULESLIST_H
