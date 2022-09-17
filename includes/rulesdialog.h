#ifndef RULESDIALOG_H
#define RULESDIALOG_H

#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
class RulesDialog;
}

class RulesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RulesDialog(QSqlTableModel * mod_cats, QSqlTableModel * mod_tags, QWidget *parent = nullptr);
    ~RulesDialog();

    QString expression() const;
    QRegularExpression regExpression() const;
    void setExpression(const QRegularExpression & regex);
    int categoryId() const;
    QString categoryName() const;
    void setCategory(int id);
    int tagId() const;
    QString tagName() const;
    void setTag(int id);

private:
    Ui::RulesDialog *ui;
};

#endif // RULESDIALOG_H
