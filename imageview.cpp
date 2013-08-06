#include "imageview.h"
#include <QGridLayout>
#include <QProcess>
#include <QtCore>

static QImage loadImage(QString imagePath)
{
    return QImage(imagePath);
}

static QStringList findImages(QString dir)
{
    QStringList result;

    QStringList findArgs;
    findArgs << dir << "-type" << "f";
    QProcess find;
    find.start("find", findArgs);
    if (!find.waitForStarted())
        return result;
    if (!find.waitForFinished())
        return result;

    QByteArray findOutput = find.readAllStandardOutput();
    QList<QByteArray> lines = findOutput.split('\n');
    foreach (QByteArray line, lines) {
        result.append(QString(line));
    }

    return result;
}

ImageView::ImageView(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    m_imageView = new QListView(this);
    m_imageView->hide();
    m_progressBar = new QProgressBar(this);

    layout->addWidget(m_imageView);
    layout->addWidget(m_progressBar);

    m_imageView->setViewMode(QListView::IconMode);
    m_imageView->setMovement(QListView::Static);
    m_imageView->setSelectionMode(QListView::SingleSelection);
    m_imageView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_imageView->setResizeMode(QListView::Adjust);
    m_imageView->setIconSize(QSize(50, 50));

    m_imageModel = new QStandardItemModel(this);
    m_imageView->setModel(m_imageModel);

    m_imageLoader = new QFutureWatcher<QImage>(this);
    connect(m_imageLoader, SIGNAL(resultReadyAt(int)), SLOT(showImage(int)));
    connect(m_imageLoader, SIGNAL(finished()), SLOT(loadFinished()));

    m_imageFinder = new QFutureWatcher<QStringList>(this);
    connect(m_imageFinder, SIGNAL(finished()), SLOT(findFinished()));
}

ImageView::~ImageView()
{
    m_imageLoader->cancel();
    m_imageFinder->cancel();

    m_imageLoader->waitForFinished();
    m_imageFinder->waitForFinished();
}

void ImageView::loadImages(QString dir)
{
    m_imageFinder->setFuture(QtConcurrent::run(findImages, dir));
}

void ImageView::showImage(int i)
{
    QStandardItem *imageItem = new QStandardItem();
    imageItem->setIcon(QIcon(QPixmap::fromImage(m_imageLoader->resultAt(i))));
    m_imageModel->appendRow(imageItem);
    m_progressBar->setValue(m_imageModel->rowCount());
}

void ImageView::findFinished()
{
    QStringList imagePaths = m_imageFinder->result();
    m_progressBar->setMaximum(imagePaths.count());
    m_imageLoader->setFuture(QtConcurrent::mapped(imagePaths, loadImage));
}

void ImageView::loadFinished()
{
    m_progressBar->hide();
    m_imageView->show();
}
