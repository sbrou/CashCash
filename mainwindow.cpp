#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _account = new Account("CCP", ui->centralwidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

