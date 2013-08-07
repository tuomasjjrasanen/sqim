#include "imageview.hh"
#include <QFileInfo>
#include <QGridLayout>
#include <QProcess>
#include <QtCore>

static QString createIcon(QString const &imageFilePath)
{
    QFileInfo imageFileInfo(imageFilePath);

    if (!imageFileInfo.isAbsolute())
        return "";

    QString imageFileName = imageFileInfo.fileName();
    QString imagePath = imageFileInfo.canonicalPath();

    QDir iconDir = QDir(QDir::homePath() + "/.qpicman/icons" + imagePath);
    if (!iconDir.mkpath("."))
        return "";

    QString iconFilePath = iconDir.filePath(imageFileName);
    if (iconDir.exists(imageFileName))
        return iconFilePath;

    QStringList args;
    args << "-background" << "Gray"
         << "-thumbnail" << "50x50>"
         << "-extent" << "50x50"
         << "-gravity" << "center"
         << imageFilePath << iconFilePath;
    QProcess process;
    process.start("convert", args);
    if (!process.waitForStarted())
        return "";
    if (!process.waitForFinished())
        return "";

    return iconFilePath;
}

ImageView::ImageView(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    m_iconView = new QListView(this);
    m_progressBar = new QProgressBar(this);
    m_progressBar->hide();

    layout->addWidget(m_iconView);
    layout->addWidget(m_progressBar);

    m_iconView->setViewMode(QListView::IconMode);
    m_iconView->setMovement(QListView::Static);
    m_iconView->setSelectionMode(QListView::SingleSelection);
    m_iconView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_iconView->setResizeMode(QListView::Adjust);
    m_iconView->setIconSize(QSize(50, 50));
    m_iconView->setStyleSheet("background: grey;");

    m_iconModel = new QStandardItemModel(this);
    m_iconView->setModel(m_iconModel);

    m_iconCreator = new QFutureWatcher<QString>(this);
    connect(m_iconCreator, SIGNAL(resultReadyAt(int)), SLOT(loadIcon(int)));
    connect(m_iconCreator, SIGNAL(finished()), SLOT(showIcons()));
}

ImageView::~ImageView()
{
    m_iconCreator->cancel();
    m_iconCreator->waitForFinished();
}

void ImageView::loadImages(QStringList const &imagePaths)
{
    m_iconView->hide();
    m_progressBar->show();
    m_progressBar->setMaximum(imagePaths.count());
    m_iconCreator->setFuture(QtConcurrent::mapped(imagePaths, createIcon));
}

void ImageView::loadIcon(int i)
{
    QString iconPath = m_iconCreator->resultAt(i);
    if (iconPath.isEmpty())
        return;
    QStandardItem *item = new QStandardItem();
    item->setIcon(QIcon(iconPath));
    m_iconModel->appendRow(item);
    m_progressBar->setValue(m_iconModel->rowCount());
}

void ImageView::showIcons()
{
    m_progressBar->hide();
    m_iconView->show();
}
