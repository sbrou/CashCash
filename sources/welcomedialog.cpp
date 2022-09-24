#include "welcomedialog.h"

welcomeDialog::welcomeDialog(QWidget *parent) : QDialog(parent)
{
    newFile = new QPushButton(tr("Créer un nouveau compte"), this);
    lastFile = new QPushButton(tr("Charger le dernier compte ouvert"), this);
    oldFile = new QPushButton(tr("Charger un autre compte"), this);
    QPushButton *quitApp = new QPushButton(tr("Quitter MoulagApp"), this);

    group = new QButtonGroup(this);
    group->addButton(newFile, 1);
    group->addButton(lastFile, 2);
    group->addButton(oldFile, 3);
    group->addButton(quitApp, 4);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(newFile);
    mainLayout->addWidget(lastFile);
    mainLayout->addWidget(oldFile);
    mainLayout->addWidget(quitApp);

    connect(group, SIGNAL(idClicked(int)), this, SLOT(done(int)));
}
