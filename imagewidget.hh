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

#ifndef IMAGEWIDGET_HH
#define IMAGEWIDGET_HH

#include <QLabel>
#include <QMap>
#include <QScrollArea>
#include <QString>
#include <QWheelEvent>
#include <QWidget>

class ImageWidget : public QScrollArea
{
    Q_OBJECT

public:
    explicit ImageWidget(QWidget *parent = 0);
    ~ImageWidget();

public slots:
    void setImage(QMap<QString, QString> imageInfo);
    void zoomIn();
    void zoomOut();
    void zoomBy(double zoomFactor);
    void zoomTo(double zoomLevel);

protected:
    void wheelEvent(QWheelEvent *event);

private:
    QLabel *m_imageLabel;
    double m_zoomLevel;

};

#endif