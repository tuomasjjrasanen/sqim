#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFutureWatcher>
#include <QMainWindow>
#include <QStringList>

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
    QFutureWatcher<QStringList> *m_cacheWatcher;

};

#endif // MAINWINDOW_H
