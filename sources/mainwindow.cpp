#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QScreen>
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>

#include "welcomedialog.h"
#include "newaccountdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _account(nullptr)
{
    ui->setupUi(this);

    setWindowTitle("MoulagApp");
    setWindowIcon(QIcon(":/images/images/euro_logo.png"));

    readSettings();
    createActions();

    QObject::connect(this, &MainWindow::quitApp, QCoreApplication::instance(), &QCoreApplication::quit, Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    if (_account != nullptr)
        delete _account;

    delete ui;
}

void MainWindow::updateWindowTitle(bool modified)
{
    QString title = windowTitle();
    if (modified) {
        if (!title.endsWith('*'))
            title += "*";
    }
    else {
        title.remove('*');
    }

    setWindowTitle(title);
}

void MainWindow::createFile()
{
    if (!maybeSave())
        return;

    initFile(true);
}

void MainWindow::openFile(const QString & filename)
{
    if (!maybeSave())
        return;

    QString loadFilename = filename.isEmpty() ? QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier MoulagApp"),
                                                                             "D:/sopie/Documents",
                                                                             tr("fichiers BSX (*.bsx)")) : filename;

    if (loadFilename.isEmpty())
        return;

    initFile(false, loadFilename);
}

void MainWindow::initFile(bool newFile, const QString & filename)
{
    NewAccountDialog newAcc(this);
    if (newFile && !newAcc.exec())
        return;

    if (_account != nullptr)
    {
        QString connectionName = _account->title();
        delete _account;

        QSqlDatabase::database(connectionName).close();
        QSqlDatabase::removeDatabase(connectionName);
    }

    _account = new Account(this);
    setCentralWidget(_account);

    connect(saveAct, &QAction::triggered, _account, &Account::saveFile);
    connect(saveAsAct, SIGNAL(triggered()), _account, SLOT(saveAsFile()));
    connect(addOpAct, SIGNAL(triggered()), _account, SLOT(addOperation()));
    connect(removeOpAct, SIGNAL(triggered()), _account, SLOT(removeOperation()));
    connect(editOpAct, SIGNAL(triggered()), _account, SLOT(editOperation()));
    connect(importOpAct, SIGNAL(triggered()), _account, SLOT(importFile()));
    connect(catAct, SIGNAL(triggered()), _account, SLOT(showCategories()));
    connect(tagAct, SIGNAL(triggered()), _account, SLOT(showTags()));
    connect(goalAct, SIGNAL(triggered()), _account, SLOT(manageGoals()));
    connect(statsAct, SIGNAL(triggered()), _account, SLOT(showStats()));
    connect(rulesAct, SIGNAL(triggered()), _account, SLOT(manageRules()));
    connect(_account, SIGNAL(selectionChanged(QItemSelection)), this, SLOT(updateAccountActions(QItemSelection)));

    connect(_account, SIGNAL(accountReady(const QString&)), this, SLOT(enableAccountActions(const QString&)));
    connect(_account, SIGNAL(stateChanged(bool)), this, SLOT(updateWindowTitle(bool)));

    if (newFile)
        _account->createFile(newAcc.title(), newAcc.balance());
    else
        _account->loadFile(filename);
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&Fichier"));
    QToolBar *fileToolBar = addToolBar(tr("Fichier"));
    const QIcon newIcon = QIcon(":/images/images/new.png");
    QAction *newAct = new QAction(newIcon, tr("&Nouveau"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Créer un nouveau compte"));
    connect(newAct, &QAction::triggered, this, &MainWindow::createFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon(":/images/images/open.png");
    QAction *openAct = new QAction(openIcon, tr("&Ouvrir..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Ouvrir un fichier existant"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon(":/images/images/save.png");
    saveAct = new QAction(saveIcon, tr("&Enregistrer"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Enregistrer le compte"));
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    saveAsAct = new QAction(saveAsIcon, tr("Enregistrer &Sous..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Enregistrer le compte sous un nouveau nom"));
    fileMenu->addAction(saveAsAct);

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon(":/images/images/cancel.png");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("Q&uitter"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Quitter MoulagApp"));

    fileToolBar->addSeparator();

    addOpAct = new QAction(ADD_ICON, tr("&Nouvelle Opération"), this);
    addOpAct->setStatusTip(tr("Ajouter une opération"));
    addOpAct->setEnabled(false);
    fileToolBar->addAction(addOpAct);

    removeOpAct = new QAction(REMOVE_ICON, tr("&Supprimer des opérations"), this);
    removeOpAct->setEnabled(false);
    removeOpAct->setStatusTip(tr("Supprimer les opérations sélectionnées"));
    fileToolBar->addAction(removeOpAct);

    editOpAct = new QAction(EDIT_ICON, tr("&Éditer une opération"), this);
    editOpAct->setEnabled(false);
    editOpAct->setStatusTip(tr("Éditer l'opération sélectionée"));
    fileToolBar->addAction(editOpAct);

    importOpAct = new QAction(QIcon(":/images/images/upload.png"), tr("&Importer des opérations"), this);
    importOpAct->setStatusTip(tr("Importer des opérations"));
    importOpAct->setEnabled(false);
    fileToolBar->addAction(importOpAct);

    fileToolBar->addSeparator();

    catAct = new QAction(QIcon(":/images/images/category.png"), tr("&Gérer les catégories"), this);
    catAct->setStatusTip(tr("Créer, éditer ou supprimer des catégories"));
    catAct->setEnabled(false);
    fileToolBar->addAction(catAct);

    tagAct = new QAction(QIcon(":/images/images/tag.png"), tr("&Gérer les tags"), this);
    tagAct->setStatusTip(tr("Créer, éditer ou supprimer des tags"));
    tagAct->setEnabled(false);
    fileToolBar->addAction(tagAct);

    goalAct = new QAction(QIcon(":/images/images/target.png"), tr("&Gérer les objectifs"), this);
    goalAct->setStatusTip(tr("Créer, éditer ou supprimer des objectifs"));
    goalAct->setEnabled(false);
    fileToolBar->addAction(goalAct);

    rulesAct = new QAction(QIcon(":/images/images/ruler.png"), tr("&Gérer les affectations"), this);
    rulesAct->setStatusTip(tr("Créer, éditer ou supprimer les règles d'affections automatiques"));
    rulesAct->setEnabled(false);
    fileToolBar->addAction(rulesAct);

    statsAct = new QAction(QIcon(":/images/images/analysis.png"), tr("&Statistiques"), this);
    statsAct->setStatusTip(tr("Montrer les statistiques"));
    statsAct->setEnabled(false);
    fileToolBar->addAction(statsAct);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Aide"));
    helpMenu->addAction(tr("&À propos"), this, &MainWindow::about);
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> example demonstrates how to "
               "write modern GUI applications using Qt, with a menu bar, "
               "toolbars, and a status bar."));
}

bool MainWindow::maybeSave()
{
    if (_account != nullptr && _account->state() != Empty)
    {
        if (_account->state() == Modified) {
            QMessageBox::StandardButton choice = QMessageBox::question(this, tr("Enregistrer"),
                                                                       tr("Voulez-vous enregistrer vos modifications ?"),
                                                                       QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard, QMessageBox::Save);
            if (choice == QMessageBox::Save)
                _account->saveFile();
            else if (choice == QMessageBox::Cancel) {
                return false;
            }
        }
        writeSettings();
    }

    return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty())
    {
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect  screenGeometry = screen->availableGeometry();
        int height = screenGeometry.height();
        int width = screenGeometry.width();
        resize(width, height);
    }
    else
        restoreGeometry(geometry);
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
    _account->saveSettings();
}

void MainWindow::enableAccountActions(const QString& title)
{
    if (!title.isEmpty())
        setWindowTitle(QString("MoulagApp - %1").arg(title));

    addOpAct->setEnabled(true);
    removeOpAct->setEnabled(false);
    editOpAct->setEnabled(false);
    importOpAct->setEnabled(true);
    catAct->setEnabled(true);
    tagAct->setEnabled(true);
    goalAct->setEnabled(true);
    statsAct->setEnabled(true);
    rulesAct->setEnabled(true);
}

void MainWindow::updateAccountActions(const QItemSelection& selected)
{
    QModelIndexList indexes = selected.indexes();

    if (!indexes.isEmpty()) {
        removeOpAct->setEnabled(true);
        if (indexes.size() == 7) // ou 6
            editOpAct->setEnabled(true);
    } else {
        removeOpAct->setEnabled(false);
        editOpAct->setEnabled(false);
    }
}

void MainWindow::showWelcomeDialog()
{
    welcomeDialog welcome(this);
    int r = welcome.exec();
    QSettings settings;

    switch (r)
    {
    case 1:
        createFile();
        break;

    case 2:
        openFile(settings.value("lastFile", QVariant()).toString());
        break;

    case 3:
        openFile();
        break;
    case 4:
        emit quitApp();
        break;
    }
}
