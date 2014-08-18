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

#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QDockWidget>
#include <QFutureWatcher>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>

#include "imagearea.hh"
#include "metadatawidget.hh"
#include "thumbnailview.hh"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    void openDir(QString dir);
    void openDir(QString dir, bool recursive);
    ~MainWindow();

private slots:
    void openDir();
    void importReadyAt(int i);
    void importFinished();
    void about();

private:
    QFutureWatcher<QString> *m_importer;
    QDockWidget *m_metadataDockWidget;
    MetadataWidget *m_metadataWidget;
    QDockWidget *m_imageDockWidget;
    ImageArea *m_imageArea;
    ThumbnailView *m_thumbnailView;

    QAction *m_openDirAction;
    QAction *m_quitAction;
    QAction *m_aboutAction;

    QAtomicInt m_openCount;
};

#endif // MAINWINDOW_HH
