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

#include "imagewidget.hh"

#include <QImage>

ImageWidget::ImageWidget(QWidget *parent)
    :QScrollArea(parent)
{
    m_imageLabel = new QLabel(this);
    m_imageLabel->setScaledContents(true);

    setBackgroundRole(QPalette::Dark);
    setWidget(m_imageLabel);
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

ImageWidget::~ImageWidget()
{
}

void ImageWidget::setImage(QMap<QString, QString> imageInfo)
{
    QImage image(imageInfo.value("filepath"));
    QPixmap pixmap(QPixmap::fromImage(image));
    m_imageLabel->setPixmap(pixmap);

    // Zoom out to fit the viewport.
    QSizeF a(pixmap.size());
    QSizeF b(a);
    b.scale(maximumViewportSize(), Qt::KeepAspectRatio);
    zoomTo(qMin(1.0, qMin(b.width() / a.width(), b.height() / a.height())));
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

void ImageWidget::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier
        && event->orientation() == Qt::Vertical
        && event->buttons() == Qt::NoButton) {
        if (event->delta() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
        event->accept();
        return;
    }
    QScrollArea::wheelEvent(event);
}
