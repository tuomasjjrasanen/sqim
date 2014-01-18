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

#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QtCore>
#include <QtDebug>

#include <exiv2/exiv2.hpp>

#include "mainwindow.hh"

static QStringList findFiles(QString dir)
{
    QStringList retval;

    QStringList findArgs;
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

void MainWindow::setupCentralWidget()
{
    m_imageWidget = new ImageWidget(this);

    setCentralWidget(m_imageWidget);
}

void MainWindow::setupDockWidgets()
{
    m_infoDockWidget = new QDockWidget("&Image info", this);
    m_infoWidget = new ImageInfoWidget(m_infoDockWidget);
    m_infoDockWidget->setWidget(m_infoWidget);
    addDockWidget(Qt::BottomDockWidgetArea, m_infoDockWidget);

    m_thumbnailDockWidget = new QDockWidget("&Thumbnails", this);
    m_thumbnailView = new ThumbnailView(m_thumbnailDockWidget);
    m_thumbnailDockWidget->setWidget(m_thumbnailView);
    addDockWidget(Qt::LeftDockWidgetArea, m_thumbnailDockWidget);
}

void MainWindow::setupStatusBar()
{
    QStatusBar *statusBar = new QStatusBar(this);

    setStatusBar(statusBar);
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = new QMenuBar(this);

    QMenu *fileMenu = new QMenu("&File", menuBar);
    m_openDirAction = fileMenu->addAction("&Open directory...");
    m_openDirAction->setShortcut(QKeySequence(Qt::Key_O));
    fileMenu->addSeparator();
    m_quitAction = fileMenu->addAction("&Quit");
    m_quitAction->setShortcut(QKeySequence(Qt::Key_Q));
    menuBar->addMenu(fileMenu);

    QMenu *thumbnailsMenu = new QMenu("&Thumbnails", menuBar);
    m_sortAscTimeOrderAction = thumbnailsMenu->addAction("&Ascending time order");
    m_sortAscTimeOrderAction->setShortcut(QKeySequence(Qt::Key_Less, Qt::Key_T));
    m_sortDescTimeOrderAction = thumbnailsMenu->addAction("&Descending time order");
    m_sortDescTimeOrderAction->setShortcut(QKeySequence(Qt::Key_Greater, Qt::Key_T));
    menuBar->addMenu(thumbnailsMenu);

    QMenu *imageMenu = new QMenu("&Image", menuBar);
    m_zoomInAction = imageMenu->addAction("&Zoom in");
    m_zoomInAction->setShortcut(QKeySequence(Qt::Key_Plus));
    m_zoomOutAction = imageMenu->addAction("&Zoom out");
    m_zoomOutAction->setShortcut(QKeySequence(Qt::Key_Minus));
    menuBar->addMenu(imageMenu);

    QMenu *windowsMenu = new QMenu("&Windows", menuBar);
    windowsMenu->addAction(m_thumbnailDockWidget->toggleViewAction());
    m_thumbnailDockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_T));
    windowsMenu->addAction(m_infoDockWidget->toggleViewAction());
    m_infoDockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_I));
    menuBar->addMenu(windowsMenu);

    QMenu *helpMenu = new QMenu("&Help", menuBar);
    m_aboutAction = helpMenu->addAction("&About");
    menuBar->addMenu(helpMenu);

    setMenuBar(menuBar);
}

void MainWindow::connectSignals()
{
    connect(m_imagePreparer, SIGNAL(started()), SLOT(imagePreparationStarted()));
    connect(m_imagePreparer, SIGNAL(finished()), SLOT(imagePreparationFinished()));
    connect(m_imagePreparer, SIGNAL(resultReadyAt(int)), SLOT(imagePreparedAt(int)));
    connect(m_openDirAction, SIGNAL(triggered(bool)), SLOT(openDir()));
    connect(m_quitAction, SIGNAL(triggered(bool)), SLOT(close()));
    m_thumbnailView->connect(m_sortAscTimeOrderAction, SIGNAL(triggered(bool)),
                             SLOT(sortAscTimeOrder()));
    m_thumbnailView->connect(m_sortDescTimeOrderAction, SIGNAL(triggered(bool)),
                             SLOT(sortDescTimeOrder()));
    m_infoWidget->connect(m_thumbnailView, SIGNAL(currentThumbnailChanged(QMap<QString, QString>)),
                          SLOT(setImageInfo(QMap<QString, QString>)));
    m_imageWidget->connect(m_thumbnailView, SIGNAL(currentThumbnailChanged(QMap<QString, QString>)),
                           SLOT(setImage(QMap<QString, QString>)));
    m_imageWidget->connect(m_zoomInAction, SIGNAL(triggered(bool)),
                           SLOT(zoomIn()));
    m_imageWidget->connect(m_zoomOutAction, SIGNAL(triggered(bool)),
                           SLOT(zoomOut()));
    connect(m_aboutAction, SIGNAL(triggered(bool)), SLOT(about()));
}

MainWindow::MainWindow(QWidget *const parent)
    : QMainWindow(parent)
{

    m_imagePreparer = new QFutureWatcher<QMap<QString, QString> >(this);

    setupCentralWidget();
    setupDockWidgets();
    setupStatusBar();
    setupMenuBar();
    connectSignals();
}

MainWindow::~MainWindow()
{
    m_imagePreparer->cancel();
    m_imagePreparer->waitForFinished();
}

static QString fileSizeToString(const qint64 bytes)
{
    static double KiB = 1024;
    static double MiB = KiB * KiB;
    static double GiB = MiB * KiB;
    if (bytes > GiB) {
        return QString::number(bytes / GiB, 'f', 1) + " GiB (" + QString::number(bytes) + " B)";
    }
    if (bytes > MiB) {
        return QString::number(bytes / MiB, 'f', 1) + " MiB (" + QString::number(bytes) + " B)";
    }
    if (bytes > KiB) {
        return QString::number(bytes / KiB, 'f', 1) + " KiB (" + QString::number(bytes) + " B)";
    }
    return QString::number(bytes) + " B";
}

static bool fillWithMetadata(const QString &filepath,
                             QMap<QString, QString> &imageInfo)
{
    try {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filepath.toStdString());
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
                             exifData["Exif.Photo.DateTimeOriginal"].toString()));

        return true;
    } catch (Exiv2::AnyError& e) {
        qWarning() << "failed to retrieve metadata from " << filepath << ": " << e.what();
    }
    return false;
}

static QString makeThumbnail(const QFileInfo &imageFileInfo)
{
    QFileInfo thumbnailFileInfo(QDir::homePath()
                                + "/.cache/sqim"
                                + imageFileInfo.canonicalFilePath()
                                + "/thumbnail.png");

    // Ensure the thumbnail directory exists.
    thumbnailFileInfo.dir().mkpath(".");

    if (!thumbnailFileInfo.exists()
        || thumbnailFileInfo.lastModified() < imageFileInfo.lastModified()) {
        QImage image(imageFileInfo.filePath());
        if (!image.format()) {
            qWarning() << imageFileInfo.filePath() << " has unknown image format";
            return "";
        }

        QImage thumbnail = image.scaled(50, 50, Qt::KeepAspectRatio);
        if (thumbnail.isNull()) {
            qWarning() << "failed to create a thumbnail image from "
                       << imageFileInfo.filePath();
            return "";
        }

        if (!thumbnail.save(thumbnailFileInfo.filePath())) {
            qWarning() << "failed to save thumbnail to "
                       << thumbnailFileInfo.filePath();
            return "";
        }
    }

    return thumbnailFileInfo.filePath();
}

static QMap<QString, QString> prepareImage(const QString &filepath)
{
    QMap<QString, QString> imageInfo;
    QFileInfo imageFileInfo(filepath);

    QString thumbnailFilepath = makeThumbnail(imageFileInfo);
    if (thumbnailFilepath.isEmpty()) {
        qWarning() << "failed to create a thumbnail from " << filepath;
        return imageInfo;
    }

    imageInfo.insert("filepath", imageFileInfo.canonicalFilePath());
    imageInfo.insert("modificationTime", imageFileInfo.lastModified().toString("yyyy-MM-ddThh:mm:ss"));
    imageInfo.insert("fileSize", fileSizeToString(imageFileInfo.size()));
    imageInfo.insert("thumbnailFilepath", thumbnailFilepath);
    fillWithMetadata(filepath, imageInfo);

    return imageInfo;
}

void MainWindow::openDir()
{
    const QString dir(QFileDialog::getExistingDirectory(this,
                                                        "Open images from a directory and its subdirectories"));
    if (dir.isEmpty())
        return;

    const QStringList filePaths(findFiles(dir));
    m_openDirAction->setEnabled(false);
    m_imagePreparer->setFuture(QtConcurrent::mapped(filePaths, prepareImage));
}

void MainWindow::imagePreparedAt(const int i)
{
    const QMap<QString, QString> imageInfo(m_imagePreparer->resultAt(i));

    if (imageInfo.empty()) {
        return;
    }

    m_thumbnailView->addThumbnail(imageInfo);
}

void MainWindow::imagePreparationStarted()
{
    statusBar()->showMessage("Opening files");
}

void MainWindow::imagePreparationFinished()
{
    QString msg("Opened " + QString::number(m_imagePreparer->future().resultCount()) + " images");
    statusBar()->showMessage(msg, 3500);
    m_openDirAction->setEnabled(true);
}
