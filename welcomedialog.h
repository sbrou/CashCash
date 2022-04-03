#ifndef WELCOMEDIALOG_H
#define WELCOMEDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QPushButton>

class welcomeDialog : public QDialog
{
    Q_OBJECT
public:
    welcomeDialog(QWidget *parent);

private:
    QVBoxLayout *mainLayout;
    QButtonGroup *group;

    QPushButton *newFile;
    QPushButton *lastFile;
    QPushButton *oldFile;
};

#endif // WELCOMEDIALOG_H
