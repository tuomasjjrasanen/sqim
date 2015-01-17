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
    ,m_importer(new QFutureWatcher<Metadata>(this))
    ,m_metadataDockWidget(new QDockWidget("&Metadata", this))
    ,m_metadataWidget(new MetadataWidget(m_metadataDockWidget))
    ,m_imageDockWidget(new QDockWidget("&Image", this))
    ,m_imageView(new ImageView(m_imageDockWidget))
    ,m_imageListView(new ImageListView(this))
    ,m_imageModel(new QSqlTableModel(this))
    ,m_openDirAction(new QAction("&Open directory...", this))
    ,m_quitAction(new QAction("&Quit", this))
    ,m_aboutAction(new QAction("&About", this))
    ,m_openCount()
    ,m_cancelImportButton(new QPushButton("Cancel import", this))
    ,m_sortActionGroup(new QActionGroup(this))
    ,m_sortAscTimeOrderAction(new QAction(QIcon(":/icons/sort_asc_date.png"),
                                          "&Ascending time order",
                                          m_sortActionGroup))
    ,m_sortDescTimeOrderAction(new QAction(QIcon(":/icons/sort_desc_date.png"),
                                           "&Descending time order",
                                           m_sortActionGroup))
    ,m_editAction(new QAction("Edit", this))
    ,m_tagAction(new QAction("Add tag", this))
    ,m_tagModel(new QSqlQueryModel(this))
{
    updateTags();

    setContextMenuPolicy(Qt::ActionsContextMenu);
    m_sortAscTimeOrderAction->setShortcut(
        QKeySequence(Qt::Key_Less, Qt::Key_T));
    m_sortDescTimeOrderAction->setShortcut(
        QKeySequence(Qt::Key_Greater, Qt::Key_T));
    m_editAction->setShortcut(QKeySequence(Qt::Key_E));

    m_sortAscTimeOrderAction->setCheckable(true);
    m_sortDescTimeOrderAction->setCheckable(true);

    addAction(m_sortAscTimeOrderAction);
    addAction(m_sortDescTimeOrderAction);
    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    addAction(separator);
    addAction(m_editAction);
    addAction(m_tagAction);

    QToolBar* toolBar = addToolBar("Hep");
    foreach (QAction* action, m_imageView->actions())
        toolBar->addAction(action);
    toolBar->addAction(m_editAction);
    toolBar->addAction(m_tagAction);
    toolBar->addAction(m_sortAscTimeOrderAction);
    toolBar->addAction(m_sortDescTimeOrderAction);

    connect(m_sortAscTimeOrderAction, SIGNAL(triggered(bool)),
            SLOT(sortAscTimeOrder()));
    connect(m_sortDescTimeOrderAction, SIGNAL(triggered(bool)),
            SLOT(sortDescTimeOrder()));
    connect(m_editAction, SIGNAL(triggered(bool)),
            SLOT(editSelectedImages()));
    connect(m_tagAction, SIGNAL(triggered(bool)),
            SLOT(tagSelectedImages()));

    m_cancelImportButton->hide();

    m_imageDockWidget->toggleViewAction()->setShortcut(
        QKeySequence(Qt::Key_I));
    m_metadataDockWidget->toggleViewAction()->setShortcut(
        QKeySequence(Qt::Key_M));
    m_openDirAction->setShortcut(QKeySequence(Qt::Key_O));
    m_quitAction->setShortcut(QKeySequence(Qt::Key_Q));

    m_imageModel->setTable("Image");
    m_imageModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_imageModel->select();
    m_imageListView->setObjectName("ImageListView");
    m_imageListView->setItemDelegate(new ImageItemDelegate(this));
    m_imageListView->setViewMode(QListView::IconMode);
    m_imageListView->setMovement(QListView::Static);
    m_imageListView->setSelectionMode(QListView::ExtendedSelection);
    m_imageListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_imageListView->setResizeMode(QListView::Adjust);
    m_imageListView->setIconSize(QSize(80, 80));
    m_imageListView->setUniformItemSizes(true);
    m_imageListView->setModel(m_imageModel);
    m_imageListView->setModelColumn(8);
    setCentralWidget(m_imageListView);

    setupDockWidgets();

    setStatusBar(new QStatusBar());

    setupMenus();

    QSettings settings;
    m_imageDockWidget->setVisible(
        settings.value("imageDockWidget/visible", true).toBool());
    m_metadataDockWidget->setVisible(
        settings.value("metadataDockWidget/visible", true).toBool());

    connect(m_importer, SIGNAL(finished()),
            SLOT(importFinished()));
    connect(m_importer, SIGNAL(resultReadyAt(int)),
            SLOT(importReadyAt(int)));
    connect(m_openDirAction, SIGNAL(triggered(bool)),
            SLOT(openDir()));
    connect(m_quitAction, SIGNAL(triggered(bool)),
            SLOT(close()));
    m_metadataWidget->connect(m_imageListView,
                              SIGNAL(currentImageChanged(const QModelIndex&, const QModelIndex&)),
                              SLOT(setMetadata(const QModelIndex&)));
    m_imageView->connect(m_imageListView,
                         SIGNAL(currentImageChanged(const QModelIndex&, const QModelIndex&)),
                         SLOT(setImage(const QModelIndex&)));
    m_imageDockWidget->connect(m_imageListView,
                               SIGNAL(activated(const QModelIndex&)),
                               SLOT(show()));
    connect(m_aboutAction, SIGNAL(triggered(bool)), SLOT(about()));
    connect(m_cancelImportButton, SIGNAL(clicked()),
            SLOT(cancelImport()));

    triggerSortAscTimeOrder();
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

void MainWindow::openDir(QString dir, bool recursive)
{
    if (dir.isEmpty())
        return;

    const QStringList filePaths(findFiles(dir, recursive));
    openFiles(filePaths);
}

void MainWindow::openDir(QString dir)
{
    openDir(dir, true);
}

void MainWindow::openDir()
{
    const QString dir(
        QFileDialog::getExistingDirectory(
            this, "Open images from a directory and its subdirectories"));

    openDir(dir);
}

void MainWindow::openFiles(const QStringList& filePaths)
{
    m_openDirAction->setEnabled(false);
    m_openCount = 0;
    QSqlDatabase::database().transaction();
    m_importer->setFuture(QtConcurrent::mapped(filePaths, import));
    statusBar()->addPermanentWidget(m_cancelImportButton);
    m_cancelImportButton->show();
    statusBar()->showMessage(QString("Importing %1 images...").arg(filePaths.size()));
}

void MainWindow::openPaths(const QStringList& paths, bool recursive)
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
        openFiles(filePaths);
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
        m_openCount.fetchAndAddOrdered(1);
}

void MainWindow::importFinished()
{
    QSqlDatabase::database().commit();
    QString msg = QString("Opened %1 images").arg(m_openCount);
    statusBar()->removeWidget(m_cancelImportButton);
    statusBar()->showMessage(msg);
    m_openDirAction->setEnabled(true);
    triggerSortAscTimeOrder();
    m_imageListView->setCurrentIndex(m_imageModel->index(0, 8));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;

    settings.setValue("imageDockWidget/visible",
                      m_imageDockWidget->isVisible());
    settings.setValue("metadataDockWidget/visible",
                      m_metadataDockWidget->isVisible());

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
    updateTags();
}

void MainWindow::updateTags()
{
    QSqlQuery query;
    query.exec("SELECT DISTINCT(tag) FROM Tagging ORDER BY tag;");
    m_tagModel->setQuery(query);
}

void MainWindow::sortAscTimeOrder()
{
    m_imageModel->sort(6, Qt::AscendingOrder);
}

void MainWindow::sortDescTimeOrder()
{
    m_imageModel->sort(6, Qt::DescendingOrder);
}

void MainWindow::triggerSortAscTimeOrder()
{
    m_sortAscTimeOrderAction->trigger();
}

void MainWindow::triggerSortDescTimeOrder()
{
    m_sortDescTimeOrderAction->trigger();
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

void MainWindow::setupDockWidgets()
{
    m_metadataDockWidget->setWidget(m_metadataWidget);
    addDockWidget(Qt::BottomDockWidgetArea, m_metadataDockWidget);

    m_imageDockWidget->setWidget(m_imageView);
    addDockWidget(Qt::LeftDockWidgetArea, m_imageDockWidget);
}

void MainWindow::setupMenus()
{
    setMenuBar(new QMenuBar());

    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(m_openDirAction);
    fileMenu->addAction(m_quitAction);
    fileMenu->addSeparator();

    QMenu *windowsMenu = menuBar()->addMenu("&Windows");
    windowsMenu->addAction(m_imageDockWidget->toggleViewAction());
    windowsMenu->addAction(m_metadataDockWidget->toggleViewAction());

    QMenu *helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(m_aboutAction);
}
