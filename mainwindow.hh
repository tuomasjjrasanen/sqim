#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFutureWatcher>
#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void importDir();
    void setupImageView();

private:
    QFutureWatcher<QStringList> *m_imageFinder;
};

#endif // MAINWINDOW_H
