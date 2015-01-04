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

#include <QDataStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QProcess>
#include <QtCore>

#include "common.hh"
#include "mainwindow.hh"
#include "metadata.hh"

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

    if (thumbnailFileInfo.exists()
        && thumbnailFileInfo.lastModified() >= imageFileInfo.lastModified()) {
        return true;
    }

    QImage image(filePath);
    if (!image.format()) {
        qWarning() << filePath << " has unknown image format";
        return false;
    }

    QImage thumbnail(50, 50, QImage::Format_ARGB32);
    thumbnail.fill(Qt::transparent);

    QPainter thumbnailPainter(&thumbnail);
    QImage smallImage(image.scaled(50, 50, Qt::KeepAspectRatio));
    thumbnailPainter.drawImage(QPoint((50 - smallImage.width()) / 2,
                                      (50 - smallImage.height()) / 2),
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
        qWarning() << "failed to parse metadata";
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
    ,m_imageWidget(new ImageWidget(m_imageDockWidget))
    ,m_thumbnailWidget(new ThumbnailWidget(this))
    ,m_openDirAction(new QAction("&Open directory...", this))
    ,m_quitAction(new QAction("&Quit", this))
    ,m_aboutAction(new QAction("&About", this))
    ,m_openCount()
{
    m_imageDockWidget->toggleViewAction()->setShortcut(
        QKeySequence(Qt::Key_I));
    m_metadataDockWidget->toggleViewAction()->setShortcut(
        QKeySequence(Qt::Key_M));
    m_openDirAction->setShortcut(QKeySequence(Qt::Key_O));
    m_quitAction->setShortcut(QKeySequence(Qt::Key_Q));

    setCentralWidget(m_thumbnailWidget);

    m_metadataDockWidget->setWidget(m_metadataWidget);
    addDockWidget(Qt::BottomDockWidgetArea, m_metadataDockWidget);

    m_imageDockWidget->setWidget(m_imageWidget);
    addDockWidget(Qt::LeftDockWidgetArea, m_imageDockWidget);

    setStatusBar(new QStatusBar());

    setMenuBar(new QMenuBar());

    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(m_openDirAction);
    fileMenu->addAction(m_quitAction);
    fileMenu->addSeparator();

    QMenu *windowsMenu = menuBar()->addMenu("&Windows");
    windowsMenu->addAction(m_imageDockWidget->toggleViewAction());
    windowsMenu->addAction(m_metadataDockWidget->toggleViewAction());

    QSettings settings;
    m_imageDockWidget->setVisible(settings.value("imageDockWidget/visible", true).toBool());
    m_metadataDockWidget->setVisible(settings.value("metadataDockWidget/visible", true).toBool());

    QMenu *helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(m_aboutAction);

    connect(m_importer, SIGNAL(finished()),
            SLOT(importFinished()));
    connect(m_importer, SIGNAL(resultReadyAt(int)),
            SLOT(importReadyAt(int)));
    connect(m_openDirAction, SIGNAL(triggered(bool)),
            SLOT(openDir()));
    connect(m_quitAction, SIGNAL(triggered(bool)),
            SLOT(close()));
    m_metadataWidget->connect(m_thumbnailWidget,
                          SIGNAL(currentThumbnailChanged(Metadata)),
                          SLOT(setMetadata(Metadata)));
    m_imageWidget->connect(m_thumbnailWidget,
                           SIGNAL(currentThumbnailChanged(Metadata)),
                           SLOT(setImage(Metadata)));
    m_imageDockWidget->connect(m_thumbnailWidget,
                               SIGNAL(currentThumbnailActivated(Metadata)),
                               SLOT(show()));
    connect(m_aboutAction, SIGNAL(triggered(bool)), SLOT(about()));
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
        "<p>This program is free software: you can redistribute it and/or modify "
        "it under the terms of the GNU General Public License as published by "
        "the Free Software Foundation, either version 3 of the License, or (at "
        "your option) any later version.</p>"
        "<p>This program is distributed in the hope that it will be useful, but "
        "WITHOUT ANY WARRANTY; without even the implied warranty of "
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
    m_thumbnailWidget->clear();
    m_importer->setFuture(QtConcurrent::mapped(filePaths, import));
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
    if (m_importer->resultAt(i).isEmpty()) {
        return;
    }

    if (m_thumbnailWidget->addThumbnail(m_importer->resultAt(i))) {
        m_openCount.fetchAndAddOrdered(1);
    }
}

void MainWindow::importFinished()
{
    QString msg = QString("Opened %1 images").arg(m_openCount);
    statusBar()->showMessage(msg);
    m_openDirAction->setEnabled(true);
    m_thumbnailWidget->triggerSortAscTimeOrder();
    m_thumbnailWidget->setCurrentIndex(0);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;

    settings.setValue("imageDockWidget/visible", m_imageDockWidget->isVisible());
    settings.setValue("metadataDockWidget/visible", m_metadataDockWidget->isVisible());

    event->accept();
}