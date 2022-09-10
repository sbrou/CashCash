#ifndef GOALSVIEW_H
#define GOALSVIEW_H

#include <QWidget>
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QApplication>
#include <QStandardItemModel>
#include <QTableView>
#include <QVBoxLayout>

#include "coloredprogressbar.h"
#include "defines.h"

class GoalsViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
};

class MyStandardItemModel : public QStandardItemModel
{
    Q_OBJECT;
public:
    MyStandardItemModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};


class GoalsView : public QWidget
{
    Q_OBJECT
public:
    explicit GoalsView(const QString &accountTitle, QWidget *parent = nullptr);
    void addGoal(Goal newGoal);
    void updateGoals();

public slots:
    void customMenuRequested(QPoint pos);

private slots:
    void EditGoal();
    void RemoveGoal();

private:
    // Methodes
    void createCustomContextMenu();
    void updateGoalProgress(int goalIndex, double amount = -1);

    // Attributs
    QVBoxLayout *mainLayout;
    QString databaseName;

    QStandardItemModel *goals_model;
    QTableView *tableView;
    QMenu* contextMenu;
    int currentGoal;

    GoalsViewDelegate *delegate;
};

#endif // GOALSVIEW_H
