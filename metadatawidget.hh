// SQIM - Simply Qt Image Manager
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

#ifndef METADATAWIDGET_HH
#define METADATAWIDGET_HH

#include <QLabel>

#include "metadata.hh"

class MetadataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MetadataWidget(QWidget *parent = 0);
    ~MetadataWidget();

public slots:
    bool openMetadata(const QString& filePath);
    void setMetadata(Metadata metadata);

private:
    QLabel *m_filepathLabel;
    QLabel *m_timestampLabel;
    QLabel *m_modificationTimeLabel;
    QLabel *m_fileSizeLabel;
    QLabel *m_imageSizeLabel;

};

#endif
