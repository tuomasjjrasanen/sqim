// SQIM - Simple Qt Image Manager
// Copyright (C) 2014 Tuomas Räsänen <tuomasjjrasanen@tjjr.fi>

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

#include <QDateTime>
#include <QFormLayout>

#include "common.hh"
#include "metadatawidget.hh"

MetadataWidget::MetadataWidget(QWidget *parent)
    :QWidget(parent)
    ,m_filePathLabel(new QLabel(this))
    ,m_timestampLabel(new QLabel(this))
    ,m_modificationTimeLabel(new QLabel(this))
    ,m_fileSizeLabel(new QLabel(this))
    ,m_imageSizeLabel(new QLabel(this))
{
    m_filePathLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_timestampLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_modificationTimeLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_fileSizeLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_imageSizeLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);

    QFormLayout *layout = new QFormLayout(this);
    layout->addRow("Filepath", m_filePathLabel);
    layout->addRow("Exposure time", m_timestampLabel);
    layout->addRow("Last modification time", m_modificationTimeLabel);
    layout->addRow("File size", m_fileSizeLabel);
    layout->addRow("Image size", m_imageSizeLabel);
    setLayout(layout);
}

MetadataWidget::~MetadataWidget()
{
}

bool MetadataWidget::openMetadata(const QString& filePath)
{
    Metadata metadata;
    // Parsing metadata can fail partially or completely, but we don't
    // actually care. We try to display everything we can. In the worst
    // case, the metadata object is just empty.
    bool result = parseMetadata(filePath, metadata);
    setMetadata(metadata);
    return result;
}

void MetadataWidget::setMetadata(Metadata metadata)
{
    m_filePathLabel->setText(metadata.value("filePath").toString());
    m_timestampLabel->setText(
        metadata.value("timestamp").toDateTime().toString());
    m_modificationTimeLabel->setText(
        metadata.value("modificationTime").toDateTime().toString());
    m_fileSizeLabel->setText(
        fileSizeToString(metadata.value("fileSize").toULongLong()));
    m_imageSizeLabel->setText(
        imageSizeToString(metadata.value("imageSize").toSize()));
}
