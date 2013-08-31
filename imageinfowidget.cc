#include <QFormLayout>

#include "imageinfowidget.hh"

ImageInfoWidget::ImageInfoWidget(QWidget *parent)
    :QWidget(parent)
{
    m_filepathLabel = new QLabel();
    m_timestampLabel = new QLabel();
    m_modificationTimeLabel = new QLabel();

    m_filepathLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_timestampLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_modificationTimeLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);

    QFormLayout *infoLayout = new QFormLayout(this);
    infoLayout->addRow("Filepath", m_filepathLabel);
    infoLayout->addRow("Timestamp", m_timestampLabel);
    infoLayout->addRow("Last modified", m_modificationTimeLabel);
    setLayout(infoLayout);
}

ImageInfoWidget::~ImageInfoWidget()
{
}

void ImageInfoWidget::setImage(Image image)
{
    m_filepathLabel->setText(image.filepath());
    m_timestampLabel->setText(image.timestamp());
    m_modificationTimeLabel->setText(image.modificationTime());
}
