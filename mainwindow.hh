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

#include <QtSql>
#include <QtGui>

#include "imageview.hh"
#include "metadatawidget.hh"
#include "imagelistview.hh"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    void openDir(QString dir);
    void openDir(QString dir, bool recursive);
    void openFiles(const QStringList& filePaths);
    void openPaths(const QStringList& paths, bool recursive);
    void triggerSortAscDate();
    void triggerSortDescDate();
    ~MainWindow();

public slots:
    void sortAscDate();
    void sortDescDate();
    void editSelectedImages();
    void tagSelectedImages();
    void loadTags();

protected:
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void openDir();
    void importReadyAt(int i);
    void importFinished();
    void about();
    void cancelImport();

private:
    void connectSignals();
    void loadSettings();
    void saveSettings();
    void setupActions();
    void setupCentralWidget();
    void setupDockWidgets();
    void setupMenus();
    void setupStatusBar();
    void setupToolBars();

    void resetImageListView();

    QAtomicInt m_openCount;
    QFutureWatcher<Metadata>* m_importer;
    QPushButton* m_cancelImportButton;

    ImageListView* m_imageListView;
    ImageView* m_imageView;
    MetadataWidget* m_metadataWidget;

    QDockWidget* m_imageDockWidget;
    QDockWidget* m_metadataDockWidget;

    QSqlQueryModel* m_tagModel;
    QSqlTableModel* m_imageModel;

    QAction* m_aboutAction;
    QAction* m_editAction;
    QAction* m_openDirAction;
    QAction* m_quitAction;
    QAction* m_sortAscDateAction;
    QAction* m_sortDescDateAction;
    QAction* m_tagAction;
    QActionGroup* m_sortActionGroup;

};

#endif // MAINWINDOW_HH
