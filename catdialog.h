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

class CatDialog : public QDialog
{
    Q_OBJECT
public:
    CatDialog();

    QString name();
    QString color();
    int type();

private slots:
    void setColor();

private:
    QVBoxLayout *mainLayout;
    QGridLayout *inputLayout;

    QLabel *qlName;
    QLineEdit *qleName;

    QLabel *qlColor;
    QLineEdit *qleColor;
    QPushButton *qpbColor;

    QGroupBox *qgbType;
    QRadioButton *expenses;
    QRadioButton *earnings;

    QDialogButtonBox *buttonBox;
};

#endif // CATDIALOG_H
