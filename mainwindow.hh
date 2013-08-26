#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFutureWatcher>
#include <QMainWindow>
#include <QStringList>

#include "image.hh"
#include "imagebrowser.hh"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openDir();
    void imagePreparedAt(int i);
    void imagePreparationStarted();
    void imagePreparationFinished();

private:
    QFutureWatcher<Image> *m_imagePreparer;
    ImageBrowser *m_imageBrowser;
};

#endif // MAINWINDOW_H
