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
    void openDir();
    void cacheReadyAt(int i);

private:
    QFutureWatcher<QString> *m_cacheWatcher;

};

#endif // MAINWINDOW_H
