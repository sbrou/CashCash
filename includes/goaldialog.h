#ifndef GOALDIALOG_H
#define GOALDIALOG_H

#include <QDialog>
#include <QSqlTableModel>

#include <defines.h>

namespace Ui {
class GoalDialog;
}

class GoalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GoalDialog(QSqlTableModel * cats, QSqlTableModel * tags, QWidget *parent = nullptr);
    ~GoalDialog();

    Goal goal();

private slots:
    void showCats(bool);
    void showTags(bool);

private:
    // Methodes
    void showGroup(unsigned type);

    // Attributs
    Ui::GoalDialog *ui;
    QSqlTableModel * catsModel;
    QSqlTableModel * tagsModel;
    Goal _goal;
};

#endif // GOALDIALOG_H
