#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFutureWatcher>
#include <QMainWindow>
#include <QStringList>

#include "thumbnailview.hh"

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
    QFutureWatcher<QMap<QString, QString> > *m_imagePreparer;
    ThumbnailView *m_thumbnailView;
    QAction *m_openDirAction;
};

#endif // MAINWINDOW_H
