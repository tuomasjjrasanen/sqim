#include "imagewidget.hh"

#include <QImage>
#include <QScrollArea>
#include <QVBoxLayout>

ImageWidget::ImageWidget(QWidget *parent)
    :QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QScrollArea *scrollArea = new QScrollArea(this);
    m_imageLabel = new QLabel(this);
    m_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_imageLabel->setScaledContents(true);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(m_imageLabel);
    scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    layout->addWidget(scrollArea);
    setLayout(layout);
}

ImageWidget::~ImageWidget()
{
}

void ImageWidget::setImage(QMap<QString, QString> imageInfo)
{
    QImage image(imageInfo.value("filepath"));
    QPixmap pixmap(QPixmap::fromImage(image));
    m_imageLabel->setPixmap(pixmap);
    m_imageLabel->adjustSize();
    m_zoomLevel = 1.0;
}

void ImageWidget::zoomIn()
{
    zoomBy(1.25);
}

void ImageWidget::zoomOut()
{
    zoomBy(0.8);
}

void ImageWidget::zoomBy(const double zoomFactor)
{
    zoomTo(m_zoomLevel * zoomFactor);
}

void ImageWidget::zoomTo(const double zoomLevel)
{
    m_zoomLevel = qMax(0.1, qMin(3.0, zoomLevel));
    m_imageLabel->resize(m_zoomLevel * m_imageLabel->pixmap()->size());
}
