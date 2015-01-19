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

#include "common.hh"
#include "mainwindow.hh"
#include "metadata.hh"
#include "imageitemdelegate.hh"

static QStringList findFiles(QString dir, bool recursive)
{
    QStringList retval;

    QStringList findArgs;
    findArgs << dir;
    if (!recursive) {
      findArgs << "-mindepth" << "1";
      findArgs << "-maxdepth" << "1";
    }
    findArgs << "-type" << "f" << "-o" << "-type" << "l";
    QProcess find;
    find.start("find", findArgs);
    if (!find.waitForStarted())
        return retval;
    if (!find.waitForFinished())
        return retval;

    QByteArray findOutput = find.readAllStandardOutput();
    QList<QByteArray> lines = findOutput.split('\n');
    foreach (QByteArray line, lines) {
        if (line.isEmpty())
            continue;
        QString filePath(line);
        QFileInfo fileInfo(filePath);
        retval.append(fileInfo.canonicalFilePath());
    }

    return retval;
}

static bool makeThumbnail(const QString& filePath, Metadata& metadata)
{
    QFileInfo imageFileInfo(filePath);
    QFileInfo thumbnailFileInfo(cacheDir(filePath), "thumbnail.png");
    QSize thumbnailSize(80, 80);

    metadata.insert("thumbnailFilePath", thumbnailFileInfo.absoluteFilePath());
    metadata.insert("thumbnailImageSize", thumbnailSize);

    if (thumbnailFileInfo.exists()
        && thumbnailFileInfo.lastModified() >= imageFileInfo.lastModified()) {
        return true;
    }

    QImage image(filePath);
    if (!image.format()) {
        qWarning() << filePath << " has unknown image format";
        return false;
    }

    QImage thumbnail(thumbnailSize, QImage::Format_ARGB32);
    thumbnail.fill(Qt::transparent);

    QPainter thumbnailPainter(&thumbnail);
    QImage smallImage(image.scaled(80, 80, Qt::KeepAspectRatio));
    thumbnailPainter.drawImage(QPoint((80 - smallImage.width()) / 2,
                                      (80 - smallImage.height()) / 2),
                               smallImage);
    if (thumbnail.isNull()) {
        qWarning() << "failed to create a thumbnail image from "
                   << filePath;
        return false;
    }

    if (!thumbnail
        .transformed(exifTransform(metadata))
        .save(thumbnailFileInfo.filePath())) {
        qWarning() << "failed to save the thumbnail image to "
                   << thumbnailFileInfo.filePath();
        return false;
    }

    return true;
}

static Metadata import(const QString& filePath)
{
    makeCacheDir(filePath);

    Metadata metadata = getMetadata(filePath);
    if (metadata.isEmpty()) {
        qCritical() << "failed to parse metadata";
        metadata.clear();
        return metadata;
    }

    if (!makeThumbnail(filePath, metadata)) {
        qWarning() << "failed to make a thumbnail";
        metadata.clear();
        return metadata;
    }

    return metadata;
}

MainWindow::MainWindow(QWidget *const parent)
    :QMainWindow(parent)
    ,m_importCount()
    ,m_importer(new QFutureWatcher<Metadata>(this))
    ,m_cancelImportButton(new QPushButton(this))

    ,m_imageListView(new ImageListView(this))
    ,m_imageView(new ImageView(this))
    ,m_metadataWidget(new MetadataWidget(this))

    ,m_metadataDockWidget(new QDockWidget(this))

    ,m_tagModel(new QSqlQueryModel(this))
    ,m_imageModel(new QSqlTableModel(this))

    ,m_aboutAction(new QAction(this))
    ,m_editAction(new QAction(this))
    ,m_importDirAction(new QAction(this))
    ,m_quitAction(new QAction(this))
    ,m_sortAscDateAction(new QAction(this))
    ,m_sortDescDateAction(new QAction(this))
    ,m_tagAction(new QAction(this))
    ,m_rotateLeftAction(new QAction(this))
    ,m_rotateRightAction(new QAction(this))
    ,m_zoomInAction(new QAction(this))
    ,m_zoomOutAction(new QAction(this))
    ,m_zoomToFitAction(new QAction(this))
    ,m_zoomTo100Action(new QAction(this))
    ,m_singleViewModeAction(new QAction(this))
    ,m_listViewModeAction(new QAction(this))

    ,m_sortActionGroup(new QActionGroup(this))
    ,m_viewModeActionGroup(new QActionGroup(this))

{
    setupActions();
    setupToolBars();
    setupCentralWidget();
    setupDockWidgets();
    setupStatusBar();
    setupMenus();

    loadTags();
    loadSettings();

    connectSignals();

    triggerSortAscDate();
    m_imageListView->setCurrentIndex(m_imageModel->index(0, 8));
}

void MainWindow::cancelImport()
{
    statusBar()->removeWidget(m_cancelImportButton);
    statusBar()->showMessage("Canceling import...");
    m_importer->cancel();
}

MainWindow::~MainWindow()
{
    m_importer->cancel();
    m_importer->waitForFinished();
}

void MainWindow::about()
{
    static QString aboutText = QString::fromUtf8(
        "<h1>Simple Qt Image Manager </h1>"
        "<p>Version " VERSION "</p>"
        "<p>Copyright © 2014 <a href=\"http://tjjr.fi\">Tuomas Räsänen</a></p>"
        "<p>This program is free software: you can redistribute it and/or "
        "modify it under the terms of the GNU General Public License as "
        "published by the Free Software Foundation, either version 3 of the "
        "License, or (at your option) any later version.</p>"
        "<p>This program is distributed in the hope that it will be useful, but"
        " WITHOUT ANY WARRANTY; without even the implied warranty of "
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.</p>"
        "<p>See the <a href=\"https://www.gnu.org/licenses/gpl-3.0.html\">GNU "
        "General Public License</a> for more details.</p>");
    QMessageBox::about(this, "About SQIM", aboutText);
}

void MainWindow::importDir(QString dir, bool recursive)
{
    if (dir.isEmpty())
        return;

    const QStringList filePaths(findFiles(dir, recursive));
    importFiles(filePaths);
}

void MainWindow::importDir(QString dir)
{
    importDir(dir, true);
}

void MainWindow::importDir()
{
    const QString dir(
        QFileDialog::getExistingDirectory(
            this, "Import images from a directory and its subdirectories"));

    importDir(dir);
}

void MainWindow::importFiles(const QStringList& filePaths)
{
    m_importDirAction->setEnabled(false);
    m_importCount = 0;
    QSqlDatabase::database().transaction();
    m_importer->setFuture(QtConcurrent::mapped(filePaths, import));
    statusBar()->addPermanentWidget(m_cancelImportButton);
    m_cancelImportButton->show();
    statusBar()->showMessage(
        QString("Importing %1 images...").arg(filePaths.size()));
}

void MainWindow::importPaths(const QStringList& paths, bool recursive)
{
    QStringList filePaths;

    foreach (QString path, paths) {
        QFileInfo fileInfo(path);
        if (fileInfo.isDir()) {
            filePaths.append(findFiles(path, recursive));
        } else {
            filePaths.append(path);
        }
    }
    if (!filePaths.isEmpty())
        importFiles(filePaths);
}

void MainWindow::importReadyAt(const int i)
{
    Metadata metadata = m_importer->resultAt(i);
    if (metadata.isEmpty())
        return;

    QString filePath = metadata.value("filePath").toString();

    QSqlRecord record(m_imageModel->record());
    record.setValue(1, metadata.value("filePath"));
    record.setValue(2, metadata.value("fileSize"));
    record.setValue(3, metadata.value("modificationTime"));
    QSize imageSize = metadata.value("imageSize").toSize();
    record.setValue(4, imageSize.width());
    record.setValue(5, imageSize.height());
    record.setValue(6, metadata.value("timestamp"));
    record.setValue(7, metadata.value("orientation"));
    record.setValue(8, metadata.value("thumbnailFilePath"));
    QSize thumbnailSize = metadata.value("thumbnailImageSize").toSize();
    record.setValue(9, thumbnailSize.width());
    record.setValue(10, thumbnailSize.height());
    m_imageModel->insertRecord(-1, record);
    if (m_imageModel->submitAll())
        m_importCount.fetchAndAddOrdered(1);
}

void MainWindow::importFinished()
{
    QSqlDatabase::database().commit();
    QString msg = QString("Imported %1 images").arg(m_importCount);
    statusBar()->removeWidget(m_cancelImportButton);
    statusBar()->showMessage(msg);
    m_importDirAction->setEnabled(true);
    triggerSortAscDate();
    m_imageListView->setCurrentIndex(m_imageModel->index(0, 8));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();

    event->accept();
}

void MainWindow::tagSelectedImages()
{
    QStringList tags;
    for (int i = 0; i < m_tagModel->rowCount(); ++i) {
        tags << m_tagModel->record(i).value(0).toString();
    }
    QString tag = QInputDialog::getItem(this, "Add tag to selected images",
                                        "Tag", tags);

    QItemSelectionModel *selectionModel = m_imageListView->selectionModel();
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();

    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();

    foreach (QModelIndex index, selectedIndexes) {
        QString filePath = index.sibling(index.row(), 1).data().toString();
        QSqlQuery query;
        if (!query.prepare("INSERT INTO Tagging(file_path, tag) VALUES(?, ?)"))
            continue;

        query.addBindValue(filePath);
        query.addBindValue(tag);

        query.exec();
    }

    db.commit();
    loadTags();
}

void MainWindow::loadTags()
{
    QSqlQuery query;
    query.exec("SELECT DISTINCT(tag) FROM Tagging ORDER BY tag;");
    m_tagModel->setQuery(query);
}

void MainWindow::sortAscDate()
{
    m_imageModel->sort(6, Qt::AscendingOrder);
}

void MainWindow::sortDescDate()
{
    m_imageModel->sort(6, Qt::DescendingOrder);
}

void MainWindow::triggerSortAscDate()
{
    m_sortAscDateAction->trigger();
}

void MainWindow::triggerSortDescDate()
{
    m_sortDescDateAction->trigger();
}

void MainWindow::editSelectedImages()
{
    QModelIndex currentIndex = m_imageListView->currentIndex();
    QItemSelectionModel *selectionModel = m_imageListView->selectionModel();
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();
    QStringList filePaths;

    foreach (QModelIndex index, selectedIndexes) {
        QString filePath = index.sibling(index.row(), 1).data().toString();
        filePaths.append(filePath);
    }

    QProcess::startDetached("gimp", filePaths);
}


void MainWindow::connectSignals()
{
    connect(m_sortAscDateAction, SIGNAL(triggered(bool)),
            SLOT(sortAscDate()));
    connect(m_sortDescDateAction, SIGNAL(triggered(bool)),
            SLOT(sortDescDate()));
    connect(m_editAction, SIGNAL(triggered(bool)),
            SLOT(editSelectedImages()));
    connect(m_tagAction, SIGNAL(triggered(bool)),
            SLOT(tagSelectedImages()));

    connect(m_importer, SIGNAL(finished()),
            SLOT(importFinished()));
    connect(m_importer, SIGNAL(resultReadyAt(int)),
            SLOT(importReadyAt(int)));
    connect(m_importDirAction, SIGNAL(triggered(bool)),
            SLOT(importDir()));
    connect(m_quitAction, SIGNAL(triggered(bool)),
            SLOT(close()));
    m_metadataWidget->connect(
        m_imageListView,
        SIGNAL(currentImageChanged(const QModelIndex&, const QModelIndex&)),
        SLOT(setMetadata(const QModelIndex&)));
    m_imageView->connect(
        m_imageListView,
        SIGNAL(currentImageChanged(const QModelIndex&, const QModelIndex&)),
        SLOT(setImage(const QModelIndex&)));
    connect(m_aboutAction, SIGNAL(triggered(bool)), SLOT(about()));
    connect(m_cancelImportButton, SIGNAL(clicked()),
            SLOT(cancelImport()));

    m_imageView->connect(m_zoomInAction, SIGNAL(triggered(bool)),
                         SLOT(zoomIn()));
    m_imageView->connect(m_zoomOutAction, SIGNAL(triggered(bool)),
                         SLOT(zoomOut()));
    m_imageView->connect(m_zoomToFitAction, SIGNAL(triggered(bool)),
                         SLOT(zoomToFit()));
    m_imageView->connect(m_zoomTo100Action, SIGNAL(triggered(bool)),
                         SLOT(zoomTo100()));
    m_imageView->connect(m_rotateLeftAction, SIGNAL(triggered(bool)),
                         SLOT(rotateLeft()));
    m_imageView->connect(m_rotateRightAction, SIGNAL(triggered(bool)),
                         SLOT(rotateRight()));

    m_imageListView->connect(m_singleViewModeAction, SIGNAL(triggered(bool)),
                             SLOT(hide()));
    m_imageView->connect(m_singleViewModeAction, SIGNAL(triggered(bool)),
                         SLOT(show()));

    m_imageListView->connect(m_listViewModeAction, SIGNAL(triggered(bool)),
                             SLOT(show()));
    m_imageView->connect(m_listViewModeAction, SIGNAL(triggered(bool)),
                         SLOT(hide()));
}
void MainWindow::setupDockWidgets()
{
    m_metadataDockWidget->setWindowTitle("&Metadata");

    m_metadataDockWidget->setWidget(m_metadataWidget);
    addDockWidget(Qt::BottomDockWidgetArea, m_metadataDockWidget);
}

void MainWindow::setupMenus()
{
    setMenuBar(new QMenuBar());

    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(m_importDirAction);
    fileMenu->addAction(m_quitAction);
    fileMenu->addSeparator();

    QMenu *windowsMenu = menuBar()->addMenu("&Windows");
    windowsMenu->addAction(m_metadataDockWidget->toggleViewAction());

    QMenu *helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(m_aboutAction);
}

void MainWindow::loadSettings()
{
    QSettings settings;
    m_metadataDockWidget->setVisible(
        settings.value("metadataDockWidget/visible", true).toBool());
}

void MainWindow::saveSettings()
{
    QSettings settings;

    settings.setValue("metadataDockWidget/visible",
                      m_metadataDockWidget->isVisible());

}

void MainWindow::setupToolBars()
{
    QToolBar* toolBar = addToolBar("Tool bar");
    foreach (QAction* action, actions())
        toolBar->addAction(action);
}

void MainWindow::setupCentralWidget()
{
    m_imageModel->setTable("Image");
    m_imageModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_imageModel->select();
    m_imageListView->setSpacing(10);
    m_imageListView->setObjectName("ImageListView");
    m_imageListView->setItemDelegate(new ImageItemDelegate(m_imageListView,
                                                           this));
    m_imageListView->setViewMode(QListView::IconMode);
    m_imageListView->setMovement(QListView::Static);
    m_imageListView->setSelectionMode(QListView::ExtendedSelection);
    m_imageListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_imageListView->setResizeMode(QListView::Adjust);
    m_imageListView->setIconSize(QSize(80, 80));
    m_imageListView->setUniformItemSizes(true);
    m_imageListView->setModel(m_imageModel);
    m_imageListView->setModelColumn(8);

    QLayout* layout = new QVBoxLayout();
    layout->addWidget(m_imageView);
    layout->addWidget(m_imageListView);
    QWidget* widget = new QWidget(this);
    widget->setLayout(layout);

    m_imageView->hide();
    m_imageListView->show();
    m_listViewModeAction->setChecked(true);

    setCentralWidget(widget);
}

void MainWindow::setupActions()
{
    m_aboutAction->setText("&About");
    m_editAction->setText("Edit");
    m_importDirAction->setText("&Import from directory...");
    m_quitAction->setText("&Quit");
    m_sortAscDateAction->setText("&Ascending time order");
    m_sortDescDateAction->setText("&Descending time order");
    m_tagAction->setText("Add tag");
    m_rotateLeftAction->setText("Rotate left");
    m_rotateRightAction->setText("Rotate right");
    m_zoomInAction->setText("&Zoom in");
    m_zoomOutAction->setText("&Zoom out");
    m_zoomToFitAction->setText("&Zoom to fit");
    m_zoomTo100Action->setText("&Zoom to 100%");
    m_singleViewModeAction->setText("Single view");
    m_listViewModeAction->setText("List view");

    m_sortAscDateAction->setIcon(QIcon(":/icons/sort_asc_date.png"));
    m_sortDescDateAction->setIcon(QIcon(":/icons/sort_desc_date.png"));
    m_rotateLeftAction->setIcon(QIcon(":/icons/rotate_left.png"));
    m_rotateRightAction->setIcon(QIcon(":/icons/rotate_right.png"));
    m_zoomInAction->setIcon(QIcon(":/icons/zoom_in.png"));
    m_zoomOutAction->setIcon(QIcon(":/icons/zoom_out.png"));
    m_zoomToFitAction->setIcon(QIcon(":/icons/zoom_to_fit.png"));
    m_zoomTo100Action->setIcon(QIcon(":/icons/zoom_to_100.png"));

    m_sortAscDateAction->setCheckable(true);
    m_sortDescDateAction->setCheckable(true);

    m_sortActionGroup->addAction(m_sortAscDateAction);
    m_sortActionGroup->addAction(m_sortDescDateAction);

    m_singleViewModeAction->setCheckable(true);
    m_listViewModeAction->setCheckable(true);

    m_viewModeActionGroup->addAction(m_singleViewModeAction);
    m_viewModeActionGroup->addAction(m_listViewModeAction);

    m_editAction->setShortcut(
        QKeySequence(Qt::Key_E));
    m_metadataDockWidget->toggleViewAction()->setShortcut(
        QKeySequence(Qt::Key_M));
    m_importDirAction->setShortcut(
        QKeySequence(Qt::Key_O));
    m_quitAction->setShortcut(
        QKeySequence(Qt::Key_Q));
    m_sortAscDateAction->setShortcut(
        QKeySequence(Qt::Key_Less, Qt::Key_T));
    m_sortDescDateAction->setShortcut(
        QKeySequence(Qt::Key_Greater, Qt::Key_T));
    m_rotateLeftAction->setShortcut(
        QKeySequence(Qt::Key_R, Qt::Key_Left));
    m_rotateRightAction->setShortcut(
        QKeySequence(Qt::Key_R, Qt::Key_Right));
    m_zoomInAction->setShortcut(
        QKeySequence(Qt::Key_Plus));
    m_zoomOutAction->setShortcut(
        QKeySequence(Qt::Key_Minus));
    m_zoomToFitAction->setShortcut(
        QKeySequence(Qt::Key_Equal));

    addAction(m_editAction);
    addAction(m_sortAscDateAction);
    addAction(m_sortDescDateAction);
    addAction(m_tagAction);
    addAction(m_zoomInAction);
    addAction(m_zoomOutAction);
    addAction(m_zoomToFitAction);
    addAction(m_zoomTo100Action);
    addAction(m_rotateLeftAction);
    addAction(m_rotateRightAction);
    addAction(m_singleViewModeAction);
    addAction(m_listViewModeAction);
}

void MainWindow::setupStatusBar()
{
    m_cancelImportButton->setText("Cancel import");
    m_cancelImportButton->hide();

    setStatusBar(new QStatusBar());
}

void MainWindow::switchToSingleView(const QModelIndex& current)
{
    m_imageListView->hide();
    m_imageView->setImage(current);
    m_imageView->show();
}
