// sqim - simply qute image manager
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

#ifndef IMAGEWIDGET_HH
#define IMAGEWIDGET_HH

#include <QToolBar>

#include "imagearea.hh"

class ImageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageWidget(QWidget* parent = 0);
    ~ImageWidget();

public slots:
    void setImage(const QString& filePath);

private:
    ImageArea* m_imageArea;
    QToolBar* m_toolBar;
};

#endif // IMAGEWIDGET_HH
