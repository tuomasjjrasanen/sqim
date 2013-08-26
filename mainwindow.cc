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
    m_imageBrowser = new ImageBrowser(this);

    setMenuBar(new QMenuBar(this));
    setCentralWidget(m_imageBrowser);
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

    m_imagePreparer = new QFutureWatcher<Image>(this);

    connect(m_imagePreparer, SIGNAL(resultReadyAt(int)), SLOT(imagePreparedAt(int)));
    connect(openDirAction, SIGNAL(triggered(bool)), SLOT(openDir()));
    connect(quitAction, SIGNAL(triggered(bool)), SLOT(close()));
    m_imageBrowser->connect(sortOlderFirstAction, SIGNAL(triggered(bool)),
                               SLOT(sortOlderFirst()));
    m_imageBrowser->connect(sortNewerFirstAction, SIGNAL(triggered(bool)),
                               SLOT(sortNewerFirst()));
    statusBar()->showMessage("Initialized");
}

MainWindow::~MainWindow()
{
    m_imagePreparer->cancel();
    m_imagePreparer->waitForFinished();
}

static Image prepareImage(const QString &filepath)
{
    Image image;
    QFileInfo imageFileInfo(filepath);

    image.setFilepath(imageFileInfo.canonicalFilePath());

    QStringList args;
    args << imageFileInfo.canonicalFilePath();

    QProcess cmdMakeThumbnail;
    cmdMakeThumbnail.start(SQIM_CMD_MAKE_THUMBNAIL, args);
    if (!cmdMakeThumbnail.waitForStarted())
        return image;

    QProcess cmdParseDatetime;
    cmdParseDatetime.start(SQIM_CMD_PARSE_DATETIME, args);
    if (!cmdParseDatetime.waitForStarted())
        return image;

    QTextStream cmdMakeThumbnailOut(&cmdMakeThumbnail);
    QTextStream cmdParseDatetimeOut(&cmdParseDatetime);

    if (!cmdParseDatetime.waitForFinished())
        return image;

    if (cmdParseDatetime.exitCode())
        return image;

    image.setTimestamp(cmdParseDatetimeOut.readLine());

    if (!cmdMakeThumbnail.waitForFinished())
        return image;

    if (cmdMakeThumbnail.exitCode())
        return image;

    image.setThumbnail(QImage(cmdMakeThumbnailOut.readLine()));

    return image;
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
    m_imagePreparer->setFuture(QtConcurrent::mapped(filePaths, prepareImage));
}

void MainWindow::imagePreparedAt(const int i)
{
    const Image image(m_imagePreparer->resultAt(i));

    if (!image.isValid())
        return;
    
    m_imageBrowser->addImage(image);
}
