#ifndef GOALSVIEW_H
#define GOALSVIEW_H

#include <QWidget>
#include <QItemDelegate>
#include <QApplication>
#include <QStandardItemModel>
#include <QTableView>
#include <QVBoxLayout>

class GoalsViewDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    GoalsViewDelegate(QWidget *parent);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
};


class GoalsView : public QWidget
{
    Q_OBJECT
public:
    explicit GoalsView(QWidget *parent = nullptr);

signals:

private:
    // Methodes
    void init();

    // Attributs
    QVBoxLayout *mainLayout;

    QStandardItemModel *goals_model;
    QTableView *tableView;

    GoalsViewDelegate *delegate;
};

#endif // GOALSVIEW_H
