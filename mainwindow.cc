#include <QtCore>
#include <QFileDialog>
#include <QGridLayout>
#include <QMenuBar>
#include <QProcess>
#include <QStatusBar>
#include <QMutex>

#include "imageview.hh"
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
    setCentralWidget(new ImageView(this));
    setStatusBar(new QStatusBar(this));

    QMenu *fileMenu = new QMenu("&File", menuBar());
    QAction *importDirAction = fileMenu->addAction("&Import directory...");
    importDirAction->setShortcut(QKeySequence(Qt::Key_I));
    fileMenu->addSeparator();
    QAction *quitAction = fileMenu->addAction("&Quit");
    quitAction->setShortcut(QKeySequence(Qt::Key_Q));
    menuBar()->addMenu(fileMenu);

    m_importWatcher = new QFutureWatcher<QString>(this);

    connect(m_importWatcher, SIGNAL(resultReadyAt(int)), SLOT(importReadyAt(int)));
    connect(importDirAction, SIGNAL(triggered(bool)), SLOT(importDir()));
    connect(quitAction, SIGNAL(triggered(bool)), SLOT(close()));
    statusBar()->showMessage("Initialized");
}

MainWindow::~MainWindow()
{
    m_importWatcher->cancel();
    m_importWatcher->waitForFinished();
}

static bool cacheImageInfo(const QString &imageFilePath)
{
    QStringList args;
    args << imageFilePath;

    return QProcess::execute(QPICMAN_CACHE_SCRIPT, args) == 0;
}

void MainWindow::importDir()
{
    const QString dir(QFileDialog::getExistingDirectory(this,
                                                        "Import images from a directory and its subdirectories"));
    if (dir.isEmpty())
        return;

    statusBar()->showMessage("Searching " + dir + " and its subdirectories for images");
    const QStringList filePaths(findFiles(dir));
    statusBar()->showMessage("Found " + QString::number(filePaths.count()) + " files");
    m_importWatcher->setFuture(QtConcurrent::filtered(filePaths, cacheImageInfo));
}

void MainWindow::importReadyAt(const int i)
{
    const QString imageFilePath(m_importWatcher->resultAt(i));
    const QString thumbnailPath(QDir::homePath()
                                + "/.cache/qpicman"
                                + imageFilePath
                                + "/thumbnail.png");
    ((ImageView*) centralWidget())->loadImage(thumbnailPath);
}
