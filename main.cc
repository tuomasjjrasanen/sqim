#include <QApplication>

#include "database.hh"
#include "mainwindow.hh"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SQIM::init();

    MainWindow w;
    w.show();

    return app.exec();
}
