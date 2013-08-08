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
    m_listWidget = new QListWidget(this);
    m_listWidget->setViewMode(QListView::IconMode);
    m_listWidget->setMovement(QListView::Static);
    m_listWidget->setSelectionMode(QListView::SingleSelection);
    m_listWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listWidget->setResizeMode(QListView::Adjust);
    m_listWidget->setIconSize(QSize(50, 50));
    m_listWidget->setStyleSheet("background: grey;");

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(m_listWidget);

    m_iconCreator = new QFutureWatcher<QString>(this);
    connect(m_iconCreator, SIGNAL(resultReadyAt(int)), SLOT(loadIcon(int)));
}

ImageView::~ImageView()
{
    m_iconCreator->cancel();
    m_iconCreator->waitForFinished();
}

void ImageView::loadImages(QStringList const &imagePaths)
{
    m_iconCreator->setFuture(QtConcurrent::mapped(imagePaths, createIcon));
}

void ImageView::loadIcon(int i)
{
    QString iconPath = m_iconCreator->resultAt(i);
    if (iconPath.isEmpty())
        return;
    QListWidgetItem *item = new QListWidgetItem();
    item->setIcon(QIcon(iconPath));
    m_listWidget->addItem(item);
}
