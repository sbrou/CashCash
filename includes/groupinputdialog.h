#ifndef GROUPINPUTDIALOG_H
#define GROUPINPUTDIALOG_H

#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
class GroupInputDialog;
}

class GroupInputDialog : public QDialog
{
    Q_OBJECT


public:
    explicit GroupInputDialog(QWidget *parent, Qt::WindowFlags flags = Qt::WindowFlags());
    ~GroupInputDialog();

    static int getIndex(QWidget *parent, const QString &title, const QString &label,
                        QSqlTableModel *mod, bool *ok = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

protected:
    void setLabelText(const QString & label);
    void setComboBoxModel(QSqlTableModel *model);
    int getComboBoxIndex();

private:
    Ui::GroupInputDialog *ui;
};

#endif // GROUPINPUTDIALOG_H
