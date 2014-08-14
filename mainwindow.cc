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

#include <QDataStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QtCore>

#include <exiv2/exiv2.hpp>

#include "mainwindow.hh"

static QStringList findFiles(QString dir, bool recursive)
{
    QStringList retval;

    QStringList findArgs;
    if (!recursive) {
      findArgs << "-mindepth" << "1";
      findArgs << "-maxdepth" << "1";
    }
    findArgs << dir << "-type" << "f";
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
        retval.append(QString(line));
    }

    return retval;
}

static QString fileSizeToString(const qint64 bytes)
{
    static qreal KiB = 1024;
    static qreal MiB = KiB * KiB;
    static qreal GiB = MiB * KiB;
    if (bytes > GiB) {
        return QString::number(bytes / GiB, 'f', 1)
            + " GiB (" + QString::number(bytes) + " B)";
    }
    if (bytes > MiB) {
        return QString::number(bytes / MiB, 'f', 1)
            + " MiB (" + QString::number(bytes) + " B)";
    }
    if (bytes > KiB) {
        return QString::number(bytes / KiB, 'f', 1)
            + " KiB (" + QString::number(bytes) + " B)";
    }
    return QString::number(bytes) + " B";
}

static bool fillWithMetadata(const QFileInfo metadataFileInfo,
                             const QFileInfo imageFileInfo,
                             QMap<QString, QString> &imageInfo)
{
    if (metadataFileInfo.exists()
        && metadataFileInfo.lastModified() >= imageFileInfo.lastModified()) {
        QFile metadataFile(metadataFileInfo.filePath());
        if (!metadataFile.open(QIODevice::ReadOnly)) {
            qWarning() << "failed to open metadata file for reading";
            return false;
        }
        QDataStream in(&metadataFile);
        in >> imageInfo;
        if (in.status() != QDataStream::Ok) {
            qWarning() << "failed to read metadata file";
            return false;
        }
        return true;
    }

    QString filepath = imageFileInfo.filePath();

    static QMutex mutex;
    QMutexLocker locker(&mutex);
    try {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(
            filepath.toStdString());
        if (image.get() == 0) {
            qWarning() << filepath << " is not recognized as a valid image file";
            return false;
        }
        image->readMetadata();

        const int w = image->pixelWidth();
        const int h = image->pixelHeight();
        imageInfo.insert("imageSize",
                         QString("%1 x %2 (%3 megapixels)")
                         .arg(w).arg(h).arg(w * h / 1000000.0, 0, 'f', 1));

        Exiv2::ExifData &exifData = image->exifData();
        if (exifData.empty()) {
            qWarning() << filepath << " does not have EXIF data";
            return false;
        }

        imageInfo.insert("timestamp",
                         QString::fromStdString(
                             exifData["Exif.Photo.DateTimeOriginal"]
                             .toString()));
    } catch (Exiv2::AnyError& e) {
        qWarning() << "failed to retrieve metadata from " 
                   << filepath << ": " << e.what();
    }

    imageInfo.insert("filepath", imageFileInfo.canonicalFilePath());
    imageInfo.insert("modificationTime",
                     imageFileInfo.lastModified()
                     .toString("yyyy-MM-ddThh:mm:ss"));
    imageInfo.insert("fileSize", fileSizeToString(imageFileInfo.size()));

    QFile metadataFile(metadataFileInfo.filePath());
    if (!metadataFile.open(QIODevice::WriteOnly)) {
        qWarning() << "failed to open metadata file for writing";
        return false;
    }
    QDataStream out(&metadataFile);
    out << imageInfo;
    if (out.status() != QDataStream::Ok) {
        qWarning() << "failed to write to the metadata file";
        return false;
    }

    return true;
}

static bool makeThumbnail(const QFileInfo imageFileInfo,
                          const QFileInfo thumbnailFileInfo)
{
    if (thumbnailFileInfo.exists()
        && thumbnailFileInfo.lastModified() >= imageFileInfo.lastModified()) {
        return true;
    }

    QImage image(imageFileInfo.filePath());
    if (!image.format()) {
        qWarning() << imageFileInfo.filePath() << " has unknown image format";
        return false;
    }

    QImage thumbnail = image.scaled(50, 50, Qt::KeepAspectRatio);
    if (thumbnail.isNull()) {
        qWarning() << "failed to create a thumbnail image from "
                   << imageFileInfo.filePath();
        return false;
    }

    if (!thumbnail.save(thumbnailFileInfo.filePath())) {
        qWarning() << "failed to save the thumbnail image to "
                   << thumbnailFileInfo.filePath();
        return false;
    }

    return true;
}

static QMap<QString, QString> prepareImage(const QString &filepath)
{
    QFileInfo imageFileInfo(filepath);
    QDir cacheDir(QDir::homePath()
                  + "/.cache/sqim"
                  + imageFileInfo.canonicalFilePath());

    if (!cacheDir.exists()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        // Ensure the cache directory exists.
        if (!cacheDir.mkpath(".")) {
            qWarning() << "failed to create the cache directory";
            return QMap<QString, QString>();
        }
        locker.unlock();
    }

    QFileInfo thumbnailFileInfo(cacheDir, "thumbnail.png");
    if (!makeThumbnail(imageFileInfo, thumbnailFileInfo)) {
        qWarning() << "failed to make a thumbnail";
        return QMap<QString, QString>();
    }

    QMap<QString, QString> imageInfo;
    QFileInfo metadataFileInfo(cacheDir, "meta.dat");
    if (!fillWithMetadata(metadataFileInfo, imageFileInfo, imageInfo)) {
        qWarning() << "failed to fill metadata";
        return QMap<QString, QString>();
    }

    return imageInfo;
}

MainWindow::MainWindow(QWidget *const parent)
    :QMainWindow(parent)
    ,m_imagePreparer(new QFutureWatcher<QMap<QString, QString> >(this))
    ,m_imageWidget(new ImageWidget(this))
    ,m_infoDockWidget(new QDockWidget("&Image info", this))
    ,m_infoWidget(new ImageInfoWidget(m_infoDockWidget))
    ,m_thumbnailDockWidget(new QDockWidget("&Thumbnails", this))
    ,m_thumbnailView(new ThumbnailView(m_thumbnailDockWidget))
    ,m_openDirAction(new QAction("&Open directory...", this))
    ,m_quitAction(new QAction("&Quit", this))
    ,m_sortAscTimeOrderAction(new QAction("&Ascending time order", this))
    ,m_sortDescTimeOrderAction(new QAction("&Descending time order", this))
    ,m_aboutAction(new QAction("&About", this))
    ,m_openCount()
{
    m_sortAscTimeOrderAction->setShortcut(
        QKeySequence(Qt::Key_Less, Qt::Key_T));
    m_sortDescTimeOrderAction->setShortcut(
        QKeySequence(Qt::Key_Greater, Qt::Key_T));
    m_thumbnailDockWidget->toggleViewAction()->setShortcut(
        QKeySequence(Qt::Key_T));
    m_infoDockWidget->toggleViewAction()->setShortcut(
        QKeySequence(Qt::Key_I));
    m_openDirAction->setShortcut(QKeySequence(Qt::Key_O));
    m_quitAction->setShortcut(QKeySequence(Qt::Key_Q));

    setCentralWidget(m_imageWidget);

    m_infoDockWidget->setWidget(m_infoWidget);
    addDockWidget(Qt::BottomDockWidgetArea, m_infoDockWidget);

    m_thumbnailDockWidget->setWidget(m_thumbnailView);
    addDockWidget(Qt::LeftDockWidgetArea, m_thumbnailDockWidget);

    setStatusBar(new QStatusBar());

    setMenuBar(new QMenuBar());

    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(m_openDirAction);
    fileMenu->addAction(m_quitAction);
    fileMenu->addSeparator();

    QMenu *thumbnailsMenu = menuBar()->addMenu("&Thumbnails");
    thumbnailsMenu->addAction(m_sortAscTimeOrderAction);
    thumbnailsMenu->addAction(m_sortDescTimeOrderAction);

    QMenu *imageMenu = menuBar()->addMenu("&Image");
    imageMenu->addAction(m_imageWidget->zoomInAction());
    imageMenu->addAction(m_imageWidget->zoomOutAction());
    imageMenu->addAction(m_imageWidget->zoomToFitAction());

    QMenu *windowsMenu = menuBar()->addMenu("&Windows");
    windowsMenu->addAction(m_thumbnailDockWidget->toggleViewAction());
    windowsMenu->addAction(m_infoDockWidget->toggleViewAction());

    QMenu *helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(m_aboutAction);

    QToolBar *toolBar = addToolBar("Image operations");
    toolBar->addAction(m_imageWidget->zoomToFitAction());
    toolBar->addAction(m_imageWidget->rotateLeftAction());
    toolBar->addAction(m_imageWidget->rotateRightAction());

    connect(m_imagePreparer, SIGNAL(started()),
            SLOT(imagePreparationStarted()));
    connect(m_imagePreparer, SIGNAL(finished()),
            SLOT(imagePreparationFinished()));
    connect(m_imagePreparer, SIGNAL(resultReadyAt(int)),
            SLOT(imagePreparedAt(int)));
    connect(m_openDirAction, SIGNAL(triggered(bool)),
            SLOT(openDir()));
    connect(m_quitAction, SIGNAL(triggered(bool)),
            SLOT(close()));
    m_sortAscTimeOrderAction->connect(m_thumbnailDockWidget->toggleViewAction(),
                                      SIGNAL(triggered(bool)),
                                      SLOT(setEnabled(bool)));
    m_sortDescTimeOrderAction->connect(m_thumbnailDockWidget->toggleViewAction(),
                                       SIGNAL(triggered(bool)),
                                       SLOT(setEnabled(bool)));
    m_thumbnailView->connect(m_sortAscTimeOrderAction,
                             SIGNAL(triggered(bool)),
                             SLOT(sortAscTimeOrder()));
    m_thumbnailView->connect(m_sortDescTimeOrderAction,
                             SIGNAL(triggered(bool)),
                             SLOT(sortDescTimeOrder()));
    m_infoWidget->connect(m_thumbnailView,
                          SIGNAL(currentThumbnailChanged(QMap<QString, QString>)),
                          SLOT(setImageInfo(QMap<QString, QString>)));
    m_imageWidget->connect(m_thumbnailView,
                           SIGNAL(currentThumbnailChanged(QMap<QString, QString>)),
                           SLOT(setImage(QMap<QString, QString>)));
    connect(m_aboutAction, SIGNAL(triggered(bool)), SLOT(about()));
}

MainWindow::~MainWindow()
{
    m_imagePreparer->cancel();
    m_imagePreparer->waitForFinished();
}

void MainWindow::about()
{
    static QString aboutText = QString::fromUtf8(
        "<h1>Simply Qute Image Manager</h1>"
        "<p>Copyright © 2013 <a href=\"http://tjjr.fi\">Tuomas Räsänen</a></p>"
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
    m_openDirAction->setEnabled(false);
    m_openCount = 0;
    m_imagePreparer->setFuture(QtConcurrent::mapped(filePaths, prepareImage));
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

void MainWindow::imagePreparedAt(const int i)
{
    const QMap<QString, QString> imageInfo(m_imagePreparer->resultAt(i));

    if (imageInfo.empty()) {
        return;
    }

    if (m_thumbnailView->addThumbnail(imageInfo)) {
        m_openCount.fetchAndAddOrdered(1);
    }
}

void MainWindow::imagePreparationStarted()
{
    statusBar()->showMessage("Opening files");
}

void MainWindow::imagePreparationFinished()
{
    QString msg = QString("Opened %1/%2 images")
        .arg(m_openCount)
        .arg(m_imagePreparer->progressMaximum());
    statusBar()->showMessage(msg);
    m_openDirAction->setEnabled(true);
}
