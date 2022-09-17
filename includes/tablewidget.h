#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QToolBar>
#include <QStyledItemDelegate>
#include <QTableView>

#include "defines.h"

namespace Ui {
class TableWidget;
}

class MyStandardItemModel : public QStandardItemModel
{
    Q_OBJECT;
public:
    MyStandardItemModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

class TableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TableWidget(QWidget *parent = nullptr);
    ~TableWidget();
    void addToolBar(QToolBar* toolbar);
    void setTableDelegate(QStyledItemDelegate *delegate);
    void setRemoveTitle(const QString & title);
    void setRemoveQuestion(const QString & question);
    int currentRow();
    void removeRow();
    QStandardItemModel* model();
    QTableView *table();
    QMenu *contextMenu();

signals:
    void changeState(AccountState);

public slots:
    void customMenuRequested(QPoint pos);
    
private:
    Ui::TableWidget *ui;

    QStandardItemModel *mod;
    QMenu *cntxtMenu;
    QString removeTitle, removeQuestion;
};

#endif // TABLEWIDGET_H
