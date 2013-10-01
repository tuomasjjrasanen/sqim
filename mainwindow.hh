// sqim - simply qute image manager
// Copyright (C) 2013 Tuomas Räsänen <tuomasjjrasanen@tjjr.fi>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.

// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QDockWidget>
#include <QFutureWatcher>
#include <QMainWindow>
#include <QMap>
#include <QMenuBar>
#include <QStatusBar>
#include <QStringList>
#include <QStringList>
#include <QWidget>

#include "imageinfowidget.hh"
#include "imagewidget.hh"
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
    ImageInfoWidget *m_infoWidget;
    ImageWidget *m_imageWidget;
    QDockWidget *m_infoDockWidget;
    QDockWidget *m_thumbnailDockWidget;
    QAction *m_quitAction;
    QAction *m_sortOldestFirstAction;
    QAction *m_sortOldestLastAction;
    QAction *m_sortLastModifiedFirstAction;
    QAction *m_sortLastModifiedLastAction;
    QAction *m_showThumbnailViewAction;
    QAction *m_zoomInAction;
    QAction *m_zoomOutAction;

    void setupDockWidgets();
    void setupCentralWidget();
    void setupStatusBar();
    void setupMenuBar();
    void connectSignals();
};

#endif // MAINWINDOW_H
