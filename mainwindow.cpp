#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QScreen>
#include <QPushButton>
#include <QVBoxLayout>

#include "welcomedialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("MoulagApp");
    setWindowIcon(QIcon(":/images/images/euro_money_48px.png"));

    readSettings();

    _account = new Account("CCP");
    setCentralWidget(_account);

    createActions();
//    createStatusBar();

    connect(_account, &Account::accountReady, this, &MainWindow::enableAccountActions);
    connect(this, SIGNAL(newFileToCreate()), _account, SLOT(createFile()));
    connect(this, SIGNAL(fileToLoad(const QString&)), _account, SLOT(loadFile(const QString&)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    const QIcon newIcon = QIcon(":/images/images/new_file_48px.png");
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
//    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon(":/images/images/open_document_48px.png");
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), _account, SLOT(loadFile()));
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon(":/images/images/save_48px.png");
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, _account, &Account::saveFile);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

//    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
//    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Save &As..."), this, &MainWindow::saveAs);
//    saveAsAct->setShortcuts(QKeySequence::SaveAs);
//    saveAsAct->setStatusTip(tr("Save the document under a new name"));

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
    connect(addOpAct, SIGNAL(triggered()), _account, SLOT(addOperation()));

    removeOpAct = new QAction(QIcon(":/images/images/Remove_48px.png"), tr("&Delete operations"), this);
    removeOpAct->setEnabled(false);
    removeOpAct->setStatusTip(tr("Delete the selected operations"));
    fileToolBar->addAction(removeOpAct);
    connect(removeOpAct, SIGNAL(triggered()), _account, SLOT(removeOperation()));

    editOpAct = new QAction(QIcon(":/images/images/edit_48px.png"), tr("&Edit operation"), this);
    editOpAct->setEnabled(false);
    editOpAct->setStatusTip(tr("Edit the selected operation"));
    fileToolBar->addAction(editOpAct);
    connect(editOpAct, SIGNAL(triggered()), _account, SLOT(editOperation()));

    importOpAct = new QAction(QIcon(":/images/images/load_from_file_48px.png"), tr("&Import operations"), this);
    importOpAct->setStatusTip(tr("Import operations from a ofx file"));
    importOpAct->setEnabled(false);
    fileToolBar->addAction(importOpAct);
    connect(importOpAct, SIGNAL(triggered()), _account, SLOT(importFile()));

    fileToolBar->addSeparator();

    catAct = new QAction(QIcon(":/images/images/category_48px.png"), tr("&Manage categories"), this);
    catAct->setStatusTip(tr("Create, edit or delete categories"));
    catAct->setEnabled(false);
    fileToolBar->addAction(catAct);
    connect(catAct, SIGNAL(triggered()), _account, SLOT(showCategories()));

    tagAct = new QAction(QIcon(":/images/images/tag_window_48px.png"), tr("&Manage tags"), this);
    tagAct->setStatusTip(tr("Create, edit or delete tags"));
    tagAct->setEnabled(false);
    fileToolBar->addAction(tagAct);
    connect(tagAct, SIGNAL(triggered()), _account, SLOT(showTags()));

    goalAct = new QAction(QIcon(":/images/images/goal_48px.png"), tr("&Manage goals"), this);
    goalAct->setStatusTip(tr("Create, edit or delete budget goals"));
    goalAct->setEnabled(false);
    fileToolBar->addAction(goalAct);

    statsAct = new QAction(QIcon(":/images/images/graph_48px.png"), tr("&Show stats"), this);
    statsAct->setStatusTip(tr("Show statistics"));
    statsAct->setEnabled(false);
    fileToolBar->addAction(statsAct);


    connect(_account, SIGNAL(selectionChanged(QItemSelection)), this, SLOT(updateAccountActions(QItemSelection)));


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

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
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

void MainWindow::enableAccountActions()
{
    addOpAct->setEnabled(true);
//    removeOpAct->setEnabled(true);
//    editOpAct->setEnabled(false);
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
    qDebug() << r;
    QSettings settings;

    switch (r)
    {
    case 1:
        emit newFileToCreate();
        break;

    case 2:
        emit fileToLoad(settings.value("lastFile", QVariant()).toString());
        break;

    case 3:
        emit fileToLoad(QFileDialog::getOpenFileName(this, tr("Open File"),
                                     "D:/sopie/Documents",
                                     tr("BSX files (*.bsx)")));
        break;
    }
}
