#include "mainwindow.hh"
#include "imageview.hh"
#include <QGridLayout>
#include <QStatusBar>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setStatusBar(new QStatusBar(this));
    setMenuBar(new QMenuBar(this));
    setCentralWidget(new ImageView(this));
}

MainWindow::~MainWindow()
{
}
