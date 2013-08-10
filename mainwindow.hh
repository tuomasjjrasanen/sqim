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
    void importReadyAt(int i);

private:
    QFutureWatcher<QString> *m_importWatcher;

};

#endif // MAINWINDOW_H
