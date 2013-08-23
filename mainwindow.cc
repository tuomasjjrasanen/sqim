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

    m_cacheWatcher = new QFutureWatcher<QStringList>(this);

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

static QStringList cacheImageInfo(const QString &imageFilePath)
{
    QStringList result;
    QFileInfo imageFileInfo(imageFilePath);

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

    if (!cmdMakeThumbnail.waitForFinished())
        return result;

    if (cmdParseDatetime.exitCode())
        return result;

    if (cmdMakeThumbnail.exitCode())
        return result;

    result.append(cmdMakeThumbnailOut.readLine());
    result.append(cmdParseDatetimeOut.readLine());
    result.append(imageFileInfo.canonicalFilePath());

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
    const QStringList results(m_cacheWatcher->resultAt(i));;
    if (results.isEmpty())
        return;
    
    ((ThumbnailWidget*) centralWidget())->addThumbnail(results[0]);
}
