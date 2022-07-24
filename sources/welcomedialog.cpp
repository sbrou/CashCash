#include "welcomedialog.h"

welcomeDialog::welcomeDialog(QWidget *parent) : QDialog(parent)
{
    newFile = new QPushButton(tr("Create a new account"), this);
    lastFile = new QPushButton(tr("Load last opened account"), this);
    oldFile = new QPushButton(tr("Load another account"), this);

    group = new QButtonGroup(this);
    group->addButton(newFile, 1);
    group->addButton(lastFile, 2);
    group->addButton(oldFile, 3);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(newFile);
    mainLayout->addWidget(lastFile);
    mainLayout->addWidget(oldFile);

    connect(group, SIGNAL(idClicked(int)), this, SLOT(done(int)));
}
