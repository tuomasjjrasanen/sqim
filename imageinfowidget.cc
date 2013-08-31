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

void ImageInfoWidget::setImageInfo(QMap<QString, QString> imageInfo)
{
    m_filepathLabel->setText(imageInfo.value("filepath"));
    m_timestampLabel->setText(imageInfo.value("timestamp"));
    m_modificationTimeLabel->setText(imageInfo.value("modificationTime"));
}
