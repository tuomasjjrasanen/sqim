// sqim - simply qute image manager
// Copyright (C) 2013 Tuomas Räsänen <tuomasjjrasanen@tjjr.fi>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.

// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <QFormLayout>

#include "imageinfowidget.hh"

ImageInfoWidget::ImageInfoWidget(QWidget *parent)
    :QWidget(parent)
    ,m_filepathLabel(new QLabel(this))
    ,m_timestampLabel(new QLabel(this))
    ,m_modificationTimeLabel(new QLabel(this))
    ,m_fileSizeLabel(new QLabel(this))
    ,m_imageSizeLabel(new QLabel(this))
{
    m_filepathLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_timestampLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_modificationTimeLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_fileSizeLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_imageSizeLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);

    QFormLayout *infoLayout = new QFormLayout(this);
    infoLayout->addRow("Filepath", m_filepathLabel);
    infoLayout->addRow("Timestamp", m_timestampLabel);
    infoLayout->addRow("Last modified", m_modificationTimeLabel);
    infoLayout->addRow("File size", m_fileSizeLabel);
    infoLayout->addRow("Image size", m_imageSizeLabel);
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
    m_fileSizeLabel->setText(imageInfo.value("fileSize"));
    m_imageSizeLabel->setText(imageInfo.value("imageSize"));
}
