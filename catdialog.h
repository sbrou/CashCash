#ifndef CATDIALOG_H
#define CATDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QSqlRecord>

class GroupDialog : public QDialog
{
    Q_OBJECT
public:
    GroupDialog();
    ~GroupDialog();

    QString name();
    QString color();
    int type();

    void setFields(QSqlRecord record);

private slots:
    void choose_color();

private:
    QVBoxLayout *mainLayout;
    QGridLayout *inputLayout;

    QLabel *qlName;
    QLineEdit *qleName;

    QLabel *qlColor;
    QColor cat_color;
    QPixmap *pixmap;
    QPushButton *qpbColor;

    QGroupBox *qgbType;
    QRadioButton *expenses;
    QRadioButton *earnings;

    QDialogButtonBox *buttonBox;
};

///// CatDialog

class CatDialog : public GroupDialog
{
public:
    CatDialog();
};

///// TagDialog

class TagDialog : public GroupDialog
{
public:
    TagDialog();
};

#endif // CATDIALOG_H
