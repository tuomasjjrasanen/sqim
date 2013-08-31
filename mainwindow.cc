#include <QtCore>
#include <QDockWidget>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QMenuBar>
#include <QProcess>
#include <QStatusBar>

#include "imageinfowidget.hh"
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

MainWindow::MainWindow(QWidget *const parent) :
    QMainWindow(parent)
{
    QDockWidget *dockWidget = new QDockWidget("&Image info", this);
    m_imageBrowser = new ImageBrowser(this);

    ImageInfoWidget *infoWidget = new ImageInfoWidget();

    dockWidget->setWidget(infoWidget);
    addDockWidget(Qt::BottomDockWidgetArea, dockWidget);

    setMenuBar(new QMenuBar(this));
    setCentralWidget(m_imageBrowser);
    setStatusBar(new QStatusBar(this));

    QMenu *fileMenu = new QMenu("&File", menuBar());
    m_openDirAction = fileMenu->addAction("&Open directory...");
    m_openDirAction->setShortcut(QKeySequence(Qt::Key_O));
    fileMenu->addSeparator();
    QAction *quitAction = fileMenu->addAction("&Quit");
    quitAction->setShortcut(QKeySequence(Qt::Key_Q));
    menuBar()->addMenu(fileMenu);

    QMenu *viewMenu = new QMenu("&View", menuBar());
    viewMenu->addAction(dockWidget->toggleViewAction());
    dockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_I));
    viewMenu->addSeparator();
    QAction *sortOldestFirstAction = viewMenu->addAction("&Sort oldest first");
    sortOldestFirstAction->setShortcut(QKeySequence(Qt::Key_Less, Qt::Key_C));
    QAction *sortOldestLastAction = viewMenu->addAction("&Sort oldest last");
    sortOldestLastAction->setShortcut(QKeySequence(Qt::Key_Greater, Qt::Key_C));
    QAction *sortLastModifiedFirstAction = viewMenu->addAction("&Sort last modified first");
    sortLastModifiedFirstAction->setShortcut(QKeySequence(Qt::Key_Less, Qt::Key_M));
    QAction *sortLastModifiedLastAction = viewMenu->addAction("&Sort last modified last");
    sortLastModifiedLastAction->setShortcut(QKeySequence(Qt::Key_Greater, Qt::Key_M));
    menuBar()->addMenu(viewMenu);

    m_imagePreparer = new QFutureWatcher<ImageInfo>(this);
    connect(m_imagePreparer, SIGNAL(started()), SLOT(imagePreparationStarted()));
    connect(m_imagePreparer, SIGNAL(finished()), SLOT(imagePreparationFinished()));
    connect(m_imagePreparer, SIGNAL(resultReadyAt(int)), SLOT(imagePreparedAt(int)));
    connect(m_openDirAction, SIGNAL(triggered(bool)), SLOT(openDir()));
    connect(quitAction, SIGNAL(triggered(bool)), SLOT(close()));
    m_imageBrowser->connect(sortOldestFirstAction, SIGNAL(triggered(bool)),
                               SLOT(sortOldestFirst()));
    m_imageBrowser->connect(sortOldestLastAction, SIGNAL(triggered(bool)),
                               SLOT(sortOldestLast()));
    m_imageBrowser->connect(sortLastModifiedFirstAction, SIGNAL(triggered(bool)),
                               SLOT(sortLastModifiedFirst()));
    m_imageBrowser->connect(sortLastModifiedLastAction, SIGNAL(triggered(bool)),
                               SLOT(sortLastModifiedLast()));
    infoWidget->connect(m_imageBrowser, SIGNAL(currentImageChanged(ImageInfo)),
                        SLOT(setImageInfo(ImageInfo)));
    statusBar()->showMessage("Initialized");
}

MainWindow::~MainWindow()
{
    m_imagePreparer->cancel();
    m_imagePreparer->waitForFinished();
}

static ImageInfo prepareImage(const QString &filepath)
{
    ImageInfo imageInfo;
    QFileInfo imageFileInfo(filepath);

    imageInfo.setFilepath(imageFileInfo.canonicalFilePath());
    imageInfo.setModificationTime(imageFileInfo.lastModified().toString("yyyy-MM-ddThh:mm:ss"));

    QStringList args;
    args << imageFileInfo.canonicalFilePath();

    QProcess cmdMakeThumbnail;
    cmdMakeThumbnail.start(SQIM_CMD_MAKE_THUMBNAIL, args);
    if (!cmdMakeThumbnail.waitForStarted())
        return imageInfo;

    QProcess cmdParseDatetime;
    cmdParseDatetime.start(SQIM_CMD_PARSE_DATETIME, args);
    if (!cmdParseDatetime.waitForStarted())
        return imageInfo;

    QTextStream cmdMakeThumbnailOut(&cmdMakeThumbnail);
    QTextStream cmdParseDatetimeOut(&cmdParseDatetime);

    if (!cmdParseDatetime.waitForFinished())
        return imageInfo;

    if (cmdParseDatetime.exitCode())
        return imageInfo;

    imageInfo.setTimestamp(cmdParseDatetimeOut.readLine());

    if (!cmdMakeThumbnail.waitForFinished())
        return imageInfo;

    if (cmdMakeThumbnail.exitCode())
        return imageInfo;

    imageInfo.setThumbnail(QImage(cmdMakeThumbnailOut.readLine()));

    return imageInfo;
}

void MainWindow::openDir()
{
    const QString dir(QFileDialog::getExistingDirectory(this,
                                                        "Open images from a directory and its subdirectories"));
    if (dir.isEmpty())
        return;

    statusBar()->showMessage("Searching " + dir + " and its subdirectories for images");
    const QStringList filePaths(findFiles(dir));
    m_openDirAction->setEnabled(false);
    m_imagePreparer->setFuture(QtConcurrent::mapped(filePaths, prepareImage));
}

void MainWindow::imagePreparedAt(const int i)
{
    const ImageInfo imageInfo(m_imagePreparer->resultAt(i));

    if (!imageInfo.isValid())
        return;
    
    m_imageBrowser->addImage(imageInfo);
}

void MainWindow::imagePreparationStarted()
{
    statusBar()->showMessage("Opening files");
}

void MainWindow::imagePreparationFinished()
{
    statusBar()->showMessage("Opened " + QString::number(m_imagePreparer->future().resultCount()) + " images");
    m_openDirAction->setEnabled(true);
}
