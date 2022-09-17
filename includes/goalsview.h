#ifndef GOALSVIEW_H
#define GOALSVIEW_H

#include <QItemDelegate>
#include <QApplication>

#include "coloredprogressbar.h"
#include "tablewidget.h"

class GoalsViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
};


class GoalsView : public TableWidget
{
    Q_OBJECT
public:
    explicit GoalsView(const QString &accountTitle, QWidget *parent = nullptr);
    void addGoal(Goal newGoal);
    void updateGoals();

private slots:
    void EditGoal();
    void RemoveGoal();

private:
    // Methodes
    void updateGoalProgress(int goalIndex, float amount = -1);

    // Attributs
    QString databaseName;
    GoalsViewDelegate *delegate;
};

#endif // GOALSVIEW_H
