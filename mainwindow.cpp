#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setCentralWidget(ui->openGLWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionDraw_triggered()
{
    ui->openGLWidget->DrawRectangle();
}


void MainWindow::on_actionClear_triggered()
{
    ui->openGLWidget->Clear();
}



void MainWindow::on_actionWire_triggered(bool checked)
{
    ui->openGLWidget->SetPolygonMode(checked);
}

