#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "account.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showWelcomeDialog();

signals:
    void newFileToCreate();
    void fileToLoad(const QString& account_file);
    void quitApp();

private slots:
    void about();
    void enableAccountActions(const QString& account_title);
    void updateAccountActions(const QItemSelection& selected);
    void updateWindowTitle(bool modified);
    void initFile(bool newFile = true, const QString & filename = "");
    void createFile();
    void openFile(const QString & filename = "");

protected:
    void closeEvent(QCloseEvent *event) override;
    void readSettings();
    void writeSettings();

private:
    void createActions();
    bool maybeSave();


    Ui::MainWindow *ui;

    Account* _account;

    QAction *saveAct;
    QAction *saveAsAct;
    QAction *addOpAct;
    QAction *removeOpAct;
    QAction *editOpAct;
    QAction *importOpAct;
    QAction *catAct;
    QAction *tagAct;
    QAction *goalAct;
    QAction *statsAct;
    QAction *rulesAct;
};
#endif // MAINWINDOW_H
