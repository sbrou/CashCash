#include "ruleslist.h"

#include <QToolBar>

#include <QHBoxLayout>
#include <QMenu>
#include <QHeaderView>
#include <QGuiApplication>
#include <QScreen>
#include <QSqlQuery>

#include "tablewidget.h"
#include "utilities.h"

using namespace Utilities;

RulesList::RulesList(QSqlTableModel * mod_cats, QSqlTableModel * mod_tags, QWidget *parent) :
    QDialog(parent),
    cats(mod_cats),
    tags(mod_tags)
{
    setWindowIcon(QIcon(":/images/images/check_rules.png"));
    setWindowTitle("Règles d'affectations");
    setWindowModality(Qt::WindowModal);
    rulesWidget = new TableWidget(this);

    ToolBar *toolBar = new ToolBar;
    connect(toolBar, SIGNAL(actTriggered(Action)), this, SLOT(applyAction(Action)));
    rulesWidget->addToolBar(toolBar);

    model()->setHorizontalHeaderLabels({tr("Expression clé"), tr("Categorie"), tr("Tag")});
    rulesWidget->table()->setSelectionBehavior(QAbstractItemView::SelectRows);
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

void RulesList::applyAction(Action act)
{
    switch (act)
    {
        case AddAction:
            addRule();
            break;
        case EditAction:
            editRule();
            break;
        case RemoveAction:
            removeRule();
            break;
        default:
            break;
    }
}

void RulesList::createRule(const QString & database, const QString &expr, bool isCaseSensitive, int catId, int tagId)
{
    QStandardItem *exprItem = new QStandardItem(expr);
    exprItem->setData(isCaseSensitive, Qt::UserRole);

    QString name;
    QSqlQuery query(QSqlDatabase::database(database));
    query.exec(QueryStatement(selectGroupCmd(CatType), idCondition(catId)).get());
    while (query.next())
        name = query.value(1).toString();

    QStandardItem *catItem = new QStandardItem(name);
    catItem->setData(catId, Qt::UserRole);

    query.exec(QueryStatement(selectGroupCmd(TagType), idCondition(tagId)).get());
    while (query.next())
        name = query.value(1).toString();

    QStandardItem *tagItem = new QStandardItem(name);
    tagItem->setData(tagId, Qt::UserRole);

    int newRow = model()->rowCount();
    model()->insertRow(newRow);
    setRuleItems(newRow, exprItem, catItem, tagItem);
}

void RulesList::addRule()
{
    RulesDialog diag(cats, tags, this);
    if (diag.exec()) {
        int newRow = model()->rowCount();
        model()->insertRow(newRow);
        setRuleInModel(newRow, diag);
    }
}

void RulesList::removeRule()
{
    QString exp = model()->item(rulesWidget->currentRow(),0)->data(Qt::DisplayRole).toString();

    rulesWidget->setRemoveTitle(tr("Supprimer une règle d'affectation"));
    rulesWidget->setRemoveQuestion(QString(tr("Etes-vous sûr de vouloir supprimer la règle d'affection sur ") + "\"%1\" ?").arg(exp));

    rulesWidget->removeRow();
}

void RulesList::editRule()
{
    RulesDialog diag(cats, tags, this);
    int row = rulesWidget->currentRow();
    diag.setExpression(model()->item(row,0)->data(Qt::DisplayRole).toString(), model()->item(row,0)->data(Qt::UserRole).toBool());
    diag.setCategory(model()->item(row,1)->data(Qt::UserRole).toInt());
    diag.setTag(model()->item(row,2)->data(Qt::UserRole).toInt());
    if (diag.exec()) {
        setRuleInModel(row, diag);
    }
}

void RulesList::setRuleInModel(int row, const RulesDialog & diag)
{
    QStandardItem *exprItem = new QStandardItem(diag.expression());
    exprItem->setData(diag.isCaseSensitive(), Qt::UserRole);

    QStandardItem *catItem = new QStandardItem(diag.categoryName());
    catItem->setData(diag.categoryId(), Qt::UserRole);

    QStandardItem *tagItem = new QStandardItem(diag.tagName());
    tagItem->setData(diag.tagId(), Qt::UserRole);

    setRuleItems(row, exprItem, catItem, tagItem);
}

QStandardItemModel* RulesList::model()
{
    return rulesWidget->model();
}

void RulesList::setRuleItems(int row, QStandardItem *expr, QStandardItem *cat, QStandardItem *tag)
{
    model()->setItem(row, 0, expr);
    model()->setItem(row, 1, cat);
    model()->setItem(row, 2, tag);

    rulesWidget->table()->resizeRowsToContents();
    rulesWidget->table()->resizeColumnToContents(1);
    rulesWidget->table()->resizeColumnToContents(2);

    emit changeState(Modified);
}

void RulesList::assignDescriptionToGroups(const QString & description, int & cat, int & tag)
{
    cat = DEFAULT_GROUP;
    tag = DEFAULT_GROUP;
    
    for (int row = 0; row < model()->rowCount(); ++row)
    {
        QString exp(model()->item(row,0)->data(Qt::DisplayRole).toString());
        QRegularExpression regex;
        if (!model()->item(row,0)->data(Qt::UserRole).toBool())
            regex = QRegularExpression(exp, QRegularExpression::CaseInsensitiveOption);
        else
            regex = QRegularExpression(exp);

        if (description.contains(regex)) {
            cat = model()->item(row,1)->data(Qt::UserRole).toInt();
            tag = model()->item(row,2)->data(Qt::UserRole).toInt();
            break;
        }
    }
}
