#include <QtCore>
#include <QFileDialog>
#include <QGridLayout>
#include <QMenuBar>
#include <QProcess>
#include <QStatusBar>

#include "thumbnailwidget.hh"
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
    setMenuBar(new QMenuBar(this));
    setCentralWidget(new ThumbnailWidget(this));
    setStatusBar(new QStatusBar(this));

    QMenu *fileMenu = new QMenu("&File", menuBar());
    QAction *openDirAction = fileMenu->addAction("&Open directory...");
    openDirAction->setShortcut(QKeySequence(Qt::Key_O));
    fileMenu->addSeparator();
    QAction *quitAction = fileMenu->addAction("&Quit");
    quitAction->setShortcut(QKeySequence(Qt::Key_Q));
    menuBar()->addMenu(fileMenu);

    m_cacheWatcher = new QFutureWatcher<QString>(this);

    connect(m_cacheWatcher, SIGNAL(resultReadyAt(int)), SLOT(cacheReadyAt(int)));
    connect(openDirAction, SIGNAL(triggered(bool)), SLOT(openDir()));
    connect(quitAction, SIGNAL(triggered(bool)), SLOT(close()));
    statusBar()->showMessage("Initialized");
}

MainWindow::~MainWindow()
{
    m_cacheWatcher->cancel();
    m_cacheWatcher->waitForFinished();
}

static QString cacheImageInfo(const QString &imageFilePath)
{
    QFileInfo imageFileInfo(imageFilePath);

    QStringList args;
    args << imageFileInfo.canonicalFilePath();

    if (QProcess::execute(SQIM_CACHE_SCRIPT, args))
        return "";

    return imageFileInfo.canonicalFilePath();
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
    const QString imageFilePath(m_cacheWatcher->resultAt(i));
    if (imageFilePath.isEmpty())
        return;
    const QString thumbnailFilePath(QDir::homePath()
                                    + "/.cache/sqim"
                                    + imageFilePath
                                    + "/thumbnail.png");
    ((ThumbnailWidget*) centralWidget())->addThumbnail(thumbnailFilePath);
}
