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
    void fileToLoad(const QString& account_file);

private slots:
    void about();
    void enableAccountActions();
    void updateAccountActions(const QItemSelection& selected);

protected:
    void closeEvent(QCloseEvent *event) override;
    void readSettings();
    void writeSettings();

private:
    void createActions();
//    void createStatusBar();


    Ui::MainWindow *ui;

    Account* _account;

    QAction *addOpAct;
    QAction *removeOpAct;
    QAction *editOpAct;
    QAction *importOpAct;
    QAction *catAct;
    QAction *tagAct;
    QAction *goalAct;
};
#endif // MAINWINDOW_H
