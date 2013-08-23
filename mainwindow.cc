#include <QtCore>
#include <QFileDialog>
#include <QGridLayout>
#include <QMenuBar>
#include <QProcess>
#include <QStatusBar>

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
    m_thumbnailWidget = new ThumbnailWidget(this);

    setMenuBar(new QMenuBar(this));
    setCentralWidget(m_thumbnailWidget);
    setStatusBar(new QStatusBar(this));

    QMenu *fileMenu = new QMenu("&File", menuBar());
    QAction *openDirAction = fileMenu->addAction("&Open directory...");
    openDirAction->setShortcut(QKeySequence(Qt::Key_O));
    fileMenu->addSeparator();
    QAction *quitAction = fileMenu->addAction("&Quit");
    quitAction->setShortcut(QKeySequence(Qt::Key_Q));
    menuBar()->addMenu(fileMenu);

    QMenu *viewMenu = new QMenu("&View", menuBar());
    QAction *sortOlderFirstAction = viewMenu->addAction("&Sort older first");
    sortOlderFirstAction->setShortcut(QKeySequence(Qt::Key_Less));
    QAction *sortNewerFirstAction = viewMenu->addAction("&Sort newer first");
    sortNewerFirstAction->setShortcut(QKeySequence(Qt::Key_Greater));
    menuBar()->addMenu(viewMenu);

    m_cacheWatcher = new QFutureWatcher<QStringList>(this);

    connect(m_cacheWatcher, SIGNAL(resultReadyAt(int)), SLOT(cacheReadyAt(int)));
    connect(openDirAction, SIGNAL(triggered(bool)), SLOT(openDir()));
    connect(quitAction, SIGNAL(triggered(bool)), SLOT(close()));
    m_thumbnailWidget->connect(sortOlderFirstAction, SIGNAL(triggered(bool)),
                               SLOT(sortOlderFirst()));
    m_thumbnailWidget->connect(sortNewerFirstAction, SIGNAL(triggered(bool)),
                               SLOT(sortNewerFirst()));
    statusBar()->showMessage("Initialized");
}

MainWindow::~MainWindow()
{
    m_cacheWatcher->cancel();
    m_cacheWatcher->waitForFinished();
}

static QStringList cacheImageInfo(const QString &imageFilePath)
{
    QStringList result;
    QFileInfo imageFileInfo(imageFilePath);

    result.insert(COL_IMAGE_FILEPATH, imageFileInfo.canonicalFilePath());

    QStringList args;
    args << imageFileInfo.canonicalFilePath();

    QProcess cmdMakeThumbnail;
    cmdMakeThumbnail.start(SQIM_CMD_MAKE_THUMBNAIL, args);
    if (!cmdMakeThumbnail.waitForStarted())
        return result;

    QProcess cmdParseDatetime;
    cmdParseDatetime.start(SQIM_CMD_PARSE_DATETIME, args);
    if (!cmdParseDatetime.waitForStarted())
        return result;

    QTextStream cmdMakeThumbnailOut(&cmdMakeThumbnail);
    QTextStream cmdParseDatetimeOut(&cmdParseDatetime);

    if (!cmdParseDatetime.waitForFinished())
        return result;

    if (cmdParseDatetime.exitCode())
        return result;

    result.insert(COL_IMAGE_DATETIME, cmdParseDatetimeOut.readLine());

    if (!cmdMakeThumbnail.waitForFinished())
        return result;

    if (cmdMakeThumbnail.exitCode())
        return result;

    result.insert(COL_THUMB_FILEPATH, cmdMakeThumbnailOut.readLine());

    return result;
}

void MainWindow::openDir()
{
    const QString dir(QFileDialog::getExistingDirectory(this,
                                                        "Open images from a directory and its subdirectories"));
    if (dir.isEmpty())
        return;

    statusBar()->showMessage("Searching " + dir + " and its subdirectories for images");
    const QStringList filePaths(findFiles(dir));
    statusBar()->showMessage("Found " + QString::number(filePaths.count()) + " files");
    m_cacheWatcher->setFuture(QtConcurrent::mapped(filePaths, cacheImageInfo));
}

void MainWindow::cacheReadyAt(const int i)
{
    const QStringList results(m_cacheWatcher->resultAt(i));

    if (results.count() != COLS)
        return;
    
   m_thumbnailWidget->addThumbnail(results);
}
