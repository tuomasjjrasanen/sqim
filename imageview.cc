#include "imageview.hh"
#include <QGridLayout>
#include <QProcess>
#include <QtCore>

static QImage loadImage(QString imagePath)
{
    return QImage(imagePath);
}

ImageView::ImageView(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    m_imageView = new QListView(this);
    m_progressBar = new QProgressBar(this);
    m_progressBar->hide();

    layout->addWidget(m_imageView);
    layout->addWidget(m_progressBar);

    m_imageView->setViewMode(QListView::IconMode);
    m_imageView->setMovement(QListView::Static);
    m_imageView->setSelectionMode(QListView::SingleSelection);
    m_imageView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_imageView->setResizeMode(QListView::Adjust);
    m_imageView->setIconSize(QSize(50, 50));
    m_imageView->setStyleSheet("background: grey;");

    m_imageModel = new QStandardItemModel(this);
    m_imageView->setModel(m_imageModel);

    m_imageLoader = new QFutureWatcher<QImage>(this);
    connect(m_imageLoader, SIGNAL(resultReadyAt(int)), SLOT(addIcon(int)));
    connect(m_imageLoader, SIGNAL(finished()), SLOT(showIcons()));
}

ImageView::~ImageView()
{
    m_imageLoader->cancel();
    m_imageLoader->waitForFinished();
}

void ImageView::loadImages(QStringList const &imagePaths)
{
    m_imageView->hide();
    m_progressBar->show();
    m_progressBar->setMaximum(imagePaths.count());
    m_imageLoader->setFuture(QtConcurrent::mapped(imagePaths, loadImage));
}

void ImageView::addIcon(int i)
{
    QStandardItem *imageItem = new QStandardItem();
    imageItem->setIcon(QIcon(QPixmap::fromImage(m_imageLoader->resultAt(i))));
    m_imageModel->appendRow(imageItem);
    m_progressBar->setValue(m_imageModel->rowCount());
}

void ImageView::showIcons()
{
    m_progressBar->hide();
    m_imageView->show();
}
