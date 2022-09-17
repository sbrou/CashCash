#include "ruleslist.h"

#include <QToolBar>

#include <QHBoxLayout>
#include <QMenu>
#include <QHeaderView>
#include <QGuiApplication>
#include <QScreen>

#include "tablewidget.h"

RulesList::RulesList(QSqlTableModel * mod_cats, QSqlTableModel * mod_tags, QWidget *parent) :
    QDialog(parent),
    cats(mod_cats),
    tags(mod_tags)
{
    setWindowIcon(QIcon(":/images/images/check_rules.png"));
    setWindowTitle("Règles d'affectations");
    rulesWidget = new TableWidget(this);

    QToolBar *toolBar = new QToolBar;
    toolBar->addAction(ADD_ICON, tr("A&dd"), this, &RulesList::addRule);
    toolBar->addAction(EDIT_ICON, tr("E&dit"), this, &RulesList::editRule);
    toolBar->addAction(REMOVE_ICON, tr("R&emove"), this, &RulesList::removeRule);
    toolBar->setIconSize(QSize(18,18));
    rulesWidget->addToolBar(toolBar);

    rulesWidget->model()->setHorizontalHeaderLabels({tr("Expression clé"), tr("Categorie"), tr("Tag")});
    rulesWidget->table()->setSelectionMode(QAbstractItemView::ExtendedSelection);
    rulesWidget->table()->resizeColumnsToContents();
    rulesWidget->table()->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);

    rulesWidget->contextMenu()->addAction(EDIT_ICON, tr("Editer la règle"), this, &RulesList::editRule);
    rulesWidget->contextMenu()->addAction(REMOVE_ICON, tr("Supprimer la règle"), this, &RulesList::removeRule);

    QHBoxLayout *hbox = new QHBoxLayout(this);
    QMargins margs = hbox->contentsMargins();
    hbox->setContentsMargins(margs.left(), 0, margs.right(), margs.bottom());
    hbox->addWidget(rulesWidget);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->availableGeometry();
    int height = screenGeometry.height() / 4;
    int width = screenGeometry.width() / 4;
    resize(width, height);
}

void RulesList::addRule()
{
    RulesDialog diag(cats, tags, this);
    if (diag.exec()) {
        int newRow = rulesWidget->model()->rowCount();
        rulesWidget->model()->insertRow(newRow);
        setRuleInModel(newRow, diag);
    }
}

void RulesList::removeRule()
{
    QString exp = rulesWidget->model()->item(rulesWidget->currentRow(),0)->data().toString();

    rulesWidget->setRemoveTitle(tr("Supprimer une règle d'affectation"));
    rulesWidget->setRemoveQuestion(QString(tr("Etes-vous sûr de vouloir supprimer la règle d'affection sur ") + "\"%1\" ?").arg(exp));

    rulesWidget->removeRow();
}

void RulesList::editRule()
{
    RulesDialog diag(cats, tags, this);
    int row = rulesWidget->currentRow();
    diag.setExpression(rulesWidget->model()->item(row,0)->data(Qt::UserRole).toRegularExpression());
    diag.setCategory(rulesWidget->model()->item(row,1)->data(Qt::UserRole).toInt());
    diag.setTag(rulesWidget->model()->item(row,2)->data(Qt::UserRole).toInt());
    if (diag.exec()) {
        setRuleInModel(row, diag);
    }
}

void RulesList::setRuleInModel(int row, const RulesDialog & diag)
{
    QStandardItem *exprItem = new QStandardItem(diag.expression());
    exprItem->setData(diag.regExpression(), Qt::UserRole);

    QStandardItem *catItem = new QStandardItem(diag.categoryName());
    catItem->setData(diag.categoryId(), Qt::UserRole);

    QStandardItem *tagItem = new QStandardItem(diag.tagName());
    tagItem->setData(diag.tagId(), Qt::UserRole);

    rulesWidget->model()->setItem(row, 0, exprItem);
    rulesWidget->model()->setItem(row, 1, catItem);
    rulesWidget->model()->setItem(row, 2, tagItem);

    rulesWidget->table()->resizeRowsToContents();
    rulesWidget->table()->resizeColumnToContents(1);
    rulesWidget->table()->resizeColumnToContents(2);
}
