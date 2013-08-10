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

static bool importImage(QString const &imageFilePath)
{
    static QMutex mkpathMutex;
    QFileInfo imageFileInfo(imageFilePath);

    if (!imageFileInfo.isAbsolute())
        return false;

    QString imageFileName = imageFileInfo.fileName();
    QString imageDirPath = imageFileInfo.canonicalPath();

    // It seems that QDir::mkpath() fails sometimes when trying to
    // simultaneously create several paths with partly overlapping
    // components. Protecting mkpath() with a mutex fixes that.
    mkpathMutex.lock();
    QDir dbDir = QDir(QDir::homePath() + "/.qpicman/db" + imageFileInfo.canonicalFilePath());
    if (!dbDir.mkpath(".")) {
        mkpathMutex.unlock();
        return false;
    }
    mkpathMutex.unlock();

    QString iconFilePath = dbDir.filePath("icon");
    if (dbDir.exists("icon"))
        return true;

    QStringList args;
    args << "-background" << "Gray"
         << "-thumbnail" << "50x50>"
         << "-extent" << "50x50"
         << "-gravity" << "center"
         << imageFilePath << iconFilePath;
    QProcess process;
    process.start("convert", args);
    if (!process.waitForStarted())
        return false;

    if (!process.waitForFinished())
        return false;

    return true;
}

void MainWindow::importDir()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    "Import images from a directory and its subdirectories");
    if (dir.isEmpty()) {
        statusBar()->showMessage("Import canceled");
        return;
    }
    statusBar()->showMessage("Importing " + dir);
    QStringList filePaths = findFiles(dir);
    statusBar()->showMessage("Found " + QString::number(filePaths.count()) + " files");
    m_importWatcher->setFuture(QtConcurrent::filtered(filePaths, importImage));
}

void MainWindow::importReadyAt(const int i)
{
    QString imageFilePath = m_importWatcher->resultAt(i);
    QString iconPath = QDir::homePath() + "/.qpicman/db" + imageFilePath + "/icon";
    ((ImageView*) centralWidget())->loadImage(iconPath);
}
