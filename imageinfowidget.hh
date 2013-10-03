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

#ifndef IMAGEINFOWIDGET_HH
#define IMAGEINFOWIDGET_HH

#include <QLabel>
#include <QMap>

class ImageInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageInfoWidget(QWidget *parent = 0);
    ~ImageInfoWidget();

public slots:
    void setImageInfo(QMap<QString, QString> imageInfo);

private:
    QLabel *m_filepathLabel;
    QLabel *m_timestampLabel;
    QLabel *m_modificationTimeLabel;
    QLabel *m_fileSizeLabel;
    QLabel *m_imageSizeLabel;

};

#endif
