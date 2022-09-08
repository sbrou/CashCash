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
    setWindowIcon(QIcon(":/images/images/euro_money_48px.png"));

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

    QString loadFilename = filename.isEmpty() ? QFileDialog::getOpenFileName(this, tr("Open File"),
                                                                             "D:/sopie/Documents",
                                                                             tr("BSX files (*.bsx)")) : filename;

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
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    const QIcon newIcon = QIcon(":/images/images/new_file_48px.png");
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::createFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon(":/images/images/open_document_48px.png");
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon(":/images/images/save_48px.png");
    saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    saveAsAct = new QAction(saveAsIcon, tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    fileMenu->addAction(saveAsAct);

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon(":/images/images/close_window_48px.png");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));

    fileToolBar->addSeparator();

    addOpAct = new QAction(QIcon(":/images/images/add_48px.png"), tr("&New Operation"), this);
    addOpAct->setStatusTip(tr("Add an operation"));
    addOpAct->setEnabled(false);
    fileToolBar->addAction(addOpAct);

    removeOpAct = new QAction(QIcon(":/images/images/Remove_48px.png"), tr("&Delete operations"), this);
    removeOpAct->setEnabled(false);
    removeOpAct->setStatusTip(tr("Delete the selected operations"));
    fileToolBar->addAction(removeOpAct);

    editOpAct = new QAction(QIcon(":/images/images/edit_48px.png"), tr("&Edit operation"), this);
    editOpAct->setEnabled(false);
    editOpAct->setStatusTip(tr("Edit the selected operation"));
    fileToolBar->addAction(editOpAct);

    importOpAct = new QAction(QIcon(":/images/images/load_from_file_48px.png"), tr("&Import operations"), this);
    importOpAct->setStatusTip(tr("Import operations from a ofx file"));
    importOpAct->setEnabled(false);
    fileToolBar->addAction(importOpAct);

    fileToolBar->addSeparator();

    catAct = new QAction(QIcon(":/images/images/category_48px.png"), tr("&Manage categories"), this);
    catAct->setStatusTip(tr("Create, edit or delete categories"));
    catAct->setEnabled(false);
    fileToolBar->addAction(catAct);

    tagAct = new QAction(QIcon(":/images/images/tag_window_48px.png"), tr("&Manage tags"), this);
    tagAct->setStatusTip(tr("Create, edit or delete tags"));
    tagAct->setEnabled(false);
    fileToolBar->addAction(tagAct);

    goalAct = new QAction(QIcon(":/images/images/goal_48px.png"), tr("&Manage goals"), this);
    goalAct->setStatusTip(tr("Create, edit or delete budget goals"));
    goalAct->setEnabled(false);
    fileToolBar->addAction(goalAct);

    statsAct = new QAction(QIcon(":/images/images/graph_48px.png"), tr("&Show stats"), this);
    statsAct->setStatusTip(tr("Show statistics"));
    statsAct->setEnabled(false);
    fileToolBar->addAction(statsAct);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));
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
    if (_account != nullptr && _account->state() != Account::Empty)
    {
        if (_account->state() == Account::Modified) {
            QMessageBox::StandardButton choice = QMessageBox::question(this, tr("Sauvegarder"),
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
    removeOpAct->setEnabled(true);
    editOpAct->setEnabled(false);
    importOpAct->setEnabled(true);
    catAct->setEnabled(true);
    tagAct->setEnabled(true);
    goalAct->setEnabled(true);
    statsAct->setEnabled(true);
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
