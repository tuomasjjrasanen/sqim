#include <QtCore>
#include <QFileDialog>
#include <QGridLayout>
#include <QMenuBar>
#include <QProcess>
#include <QStatusBar>

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
    m_imageFinder = new QFutureWatcher<QStringList>(this);

    QMenu *fileMenu = new QMenu("&File", menuBar());
    QAction *importDirAction = fileMenu->addAction("&Import directory...");
    importDirAction->setShortcut(QKeySequence(Qt::Key_I));
    fileMenu->addSeparator();
    QAction *quitAction = fileMenu->addAction("&Quit");
    quitAction->setShortcut(QKeySequence(Qt::Key_Q));
    menuBar()->addMenu(fileMenu);

    connect(importDirAction, SIGNAL(triggered(bool)), SLOT(importDir()));
    connect(quitAction, SIGNAL(triggered(bool)), SLOT(close()));
    connect(m_imageFinder, SIGNAL(finished()), SLOT(setupImageView()));
    statusBar()->showMessage("Initialized");
}

MainWindow::~MainWindow()
{
    m_imageFinder->cancel();
    m_imageFinder->waitForFinished();
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
    m_imageFinder->setFuture(QtConcurrent::run(findFiles, dir));
}

void MainWindow::setupImageView()
{
    QStringList imagePaths = m_imageFinder->result();
    ((ImageView*) centralWidget())->loadImages(imagePaths);
}
