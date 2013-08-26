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
    QAction *sortOldestFirstAction = viewMenu->addAction("&Sort oldest first");
    sortOldestFirstAction->setShortcut(QKeySequence(Qt::Key_Less, Qt::Key_C));
    QAction *sortOldestLastAction = viewMenu->addAction("&Sort oldest last");
    sortOldestLastAction->setShortcut(QKeySequence(Qt::Key_Greater, Qt::Key_C));
    QAction *sortLastModifiedFirstAction = viewMenu->addAction("&Sort last modified first");
    sortLastModifiedFirstAction->setShortcut(QKeySequence(Qt::Key_Less, Qt::Key_M));
    QAction *sortLastModifiedLastAction = viewMenu->addAction("&Sort last modified last");
    sortLastModifiedLastAction->setShortcut(QKeySequence(Qt::Key_Greater, Qt::Key_M));
    menuBar()->addMenu(viewMenu);

    m_imagePreparer = new QFutureWatcher<Image>(this);

    connect(m_imagePreparer, SIGNAL(resultReadyAt(int)), SLOT(imagePreparedAt(int)));
    connect(openDirAction, SIGNAL(triggered(bool)), SLOT(openDir()));
    connect(quitAction, SIGNAL(triggered(bool)), SLOT(close()));
    m_imageBrowser->connect(sortOldestFirstAction, SIGNAL(triggered(bool)),
                               SLOT(sortOldestFirst()));
    m_imageBrowser->connect(sortOldestLastAction, SIGNAL(triggered(bool)),
                               SLOT(sortOldestLast()));
    m_imageBrowser->connect(sortLastModifiedFirstAction, SIGNAL(triggered(bool)),
                               SLOT(sortLastModifiedFirst()));
    m_imageBrowser->connect(sortLastModifiedLastAction, SIGNAL(triggered(bool)),
                               SLOT(sortLastModifiedLast()));
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
    image.setModificationTime(imageFileInfo.lastModified().toString("yyyy-MM-ddThh:mm:ss"));

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
