// SQIM - Simple Qt Image Manager
// Copyright (C) 2014 Tuomas Räsänen <tuomasjjrasanen@tjjr.fi>

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
#include <QPushButton>
#include <QStatusBar>
#include <QToolBar>

#include "imagewidget.hh"
#include "metadatawidget.hh"
#include "thumbnailwidget.hh"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    void openDir(QString dir);
    void openDir(QString dir, bool recursive);
    void openFiles(const QStringList& filePaths);
    void openPaths(const QStringList& paths, bool recursive);
    ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void openDir();
    void importReadyAt(int i);
    void importFinished();
    void about();
    void cancelImport();

private:
    QFutureWatcher<Metadata> *m_importer;
    QDockWidget *m_metadataDockWidget;
    MetadataWidget *m_metadataWidget;
    QDockWidget *m_imageDockWidget;
    ImageWidget *m_imageWidget;
    ThumbnailWidget *m_thumbnailWidget;

    QAction *m_openDirAction;
    QAction *m_quitAction;
    QAction *m_aboutAction;

    QAtomicInt m_openCount;

    QPushButton *m_cancelImportButton;
};

#endif // MAINWINDOW_HH
