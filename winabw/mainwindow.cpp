#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createActions();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::exit()
{
    close();
}

void MainWindow::createActions()
{

}
