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

#include "metadatawidget.hh"

MetadataWidget::MetadataWidget(QWidget *parent)
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

    QFormLayout *layout = new QFormLayout(this);
    layout->addRow("Filepath", m_filepathLabel);
    layout->addRow("Timestamp", m_timestampLabel);
    layout->addRow("Last modified", m_modificationTimeLabel);
    layout->addRow("File size", m_fileSizeLabel);
    layout->addRow("Image size", m_imageSizeLabel);
    setLayout(layout);
}

MetadataWidget::~MetadataWidget()
{
}

void MetadataWidget::setMetadata(Metadata metadata)
{
    m_filepathLabel->setText(metadata.value("filepath"));
    m_timestampLabel->setText(metadata.value("timestamp"));
    m_modificationTimeLabel->setText(metadata.value("modificationTime"));
    m_fileSizeLabel->setText(metadata.value("fileSize"));
    m_imageSizeLabel->setText(metadata.value("imageSize"));
}
