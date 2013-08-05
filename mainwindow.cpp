#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageview.h"
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->loadImages("/home/tuomasjjrasanen/.sxiv/cache");
}

MainWindow::~MainWindow()
{
    delete ui;
}
