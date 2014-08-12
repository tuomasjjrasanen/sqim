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

#include <QDebug>
#include <QScrollBar>
#include <QImage>
#include <QPoint>

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

    zoomToFit();
}

const QPoint ImageWidget::viewportCenter() const {
    return QPoint(viewport()->width() / 2, viewport()->height() / 2);
}

void ImageWidget::zoomIn()
{
    zoomIn(viewportCenter());
}

void ImageWidget::zoomOut()
{
    zoomOut(viewportCenter());
}

void ImageWidget::zoomIn(const QPoint &focalPoint)
{
    zoomBy(1.25, focalPoint);
}

void ImageWidget::zoomOut(const QPoint &focalPoint)
{
    zoomBy(0.8, focalPoint);
}

void ImageWidget::zoomBy(const double zoomFactor)
{
    zoomTo(m_zoomLevel * zoomFactor);
}

void ImageWidget::zoomBy(const double zoomFactor, const QPoint &focalPoint)
{
    zoomTo(m_zoomLevel * zoomFactor, focalPoint);
}

void ImageWidget::adjustScrollBars(const QPoint &focalPoint)
{
    const QSizeF viewportSizeF(viewport()->size());
    const QPointF focalPointF(focalPoint);
    
    const double horizontalFactor = focalPointF.x() / viewportSizeF.width();
    const double verticalFactor = focalPointF.y() / viewportSizeF.height();

    horizontalScrollBar()->setValue(int(horizontalFactor
                                        * horizontalScrollBar()->maximum()));
    verticalScrollBar()->setValue(int(verticalFactor
                                      * verticalScrollBar()->maximum()));
}

void ImageWidget::zoomToFit()
{
    QSizeF a(m_imageLabel->pixmap()->size());
    QSizeF b(a);
    b.scale(maximumViewportSize(), Qt::KeepAspectRatio);
    zoomTo(qMin(1.0, qMin(b.width() / a.width(), b.height() / a.height())));
}

void ImageWidget::zoomTo(const double zoomLevel)
{
    zoomTo(zoomLevel, viewportCenter());
}

void ImageWidget::zoomTo(const double zoomLevel, const QPoint &focalPoint)
{
    if (!m_imageLabel->pixmap()) {
        // Image has not been set yet.
        return;
    }

    m_zoomLevel = qMax(0.1, qMin(3.0, zoomLevel));
    m_imageLabel->resize(m_zoomLevel * m_imageLabel->pixmap()->size());

    adjustScrollBars(focalPoint);
}

void ImageWidget::rotate(qreal degrees)
{
    m_imageLabel->setPixmap(m_imageLabel->pixmap()->transformed(
                                QTransform().rotate(degrees)));
    zoomTo(m_zoomLevel * 1.0);
}

void ImageWidget::rotateLeft()
{
    rotate(-90);
}

void ImageWidget::rotateRight()
{
    rotate(90);
}

void ImageWidget::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier
        && event->orientation() == Qt::Vertical
        && event->buttons() == Qt::NoButton) {
        if (event->delta() > 0) {
            zoomIn(event->pos());
        } else {
            zoomOut(event->pos());
        }
        event->accept();
        return;
    }
    QScrollArea::wheelEvent(event);
}
