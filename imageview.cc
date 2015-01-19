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

#include "imageview.hh"

ImageView::ImageView(QWidget *parent)
    :QScrollArea(parent)
    ,m_imageLabel(new QLabel(this))
    ,m_transform()
{
    m_imageLabel->setScaledContents(true);
    setWidget(m_imageLabel);

    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

ImageView::~ImageView()
{
}

void ImageView::setImage(const QModelIndex& current)
{
    QString filePath = current.sibling(current.row(), 1).data().toString();
    int orientation = current.sibling(current.row(), 7).data().toInt();

    m_imageFilePath = filePath;
    m_imageOrientation = orientation;
    m_isImageLoaded = false;

    loadImage();
}

void ImageView::loadImage()
{
    if (!isVisible())
        return;

    if (m_isImageLoaded)
        return;

    if (m_imageFilePath.isEmpty())
        return;

    m_imageReader.setFileName(m_imageFilePath);
    m_imageSize = m_imageReader.size();
    m_imageReader.setScaledSize(m_imageReader.size() * 0.2);
    m_transform = exifTransform(m_imageOrientation);

    QPixmap pixmap;
    if (!QPixmapCache::find(m_imageFilePath, &pixmap)) {
        pixmap = QPixmap::fromImageReader(&m_imageReader)
            .transformed(m_transform);
        QPixmapCache::insert(m_imageFilePath, pixmap);
    }
    m_imageLabel->setPixmap(pixmap);

    m_isImageLoaded = true;

    zoomToFit();
}

const QPoint ImageView::viewportCenter() const {
    return QPoint(viewport()->width() / 2, viewport()->height() / 2);
}

void ImageView::zoomIn()
{
    zoomIn(viewportCenter());
}

void ImageView::zoomOut()
{
    zoomOut(viewportCenter());
}

void ImageView::zoomIn(const QPoint &focalPoint)
{
    zoomBy(1.25, focalPoint);
}

void ImageView::zoomOut(const QPoint &focalPoint)
{
    zoomBy(0.8, focalPoint);
}

void ImageView::zoomBy(const qreal zoomFactor)
{
    zoomTo(m_zoomLevel * zoomFactor);
}

void ImageView::zoomBy(const qreal zoomFactor, const QPoint &focalPoint)
{
    zoomTo(m_zoomLevel * zoomFactor, focalPoint);
}

void ImageView::adjustScrollBars(const QPoint &focalPoint)
{
    const QSizeF viewportSizeF(viewport()->size());
    const QPointF focalPointF(focalPoint);
    
    const qreal horizontalFactor = focalPointF.x() / viewportSizeF.width();
    const qreal verticalFactor = focalPointF.y() / viewportSizeF.height();

    horizontalScrollBar()->setValue(int(horizontalFactor
                                        * horizontalScrollBar()->maximum()));
    verticalScrollBar()->setValue(int(verticalFactor
                                      * verticalScrollBar()->maximum()));
}

void ImageView::zoomToFit()
{
    if (!m_imageLabel->pixmap()) {
        // Image has not been set yet.
        return;
    }

    QSizeF a(m_imageLabel->pixmap()->size());
    QSizeF b(a);
    b.scale(maximumViewportSize(), Qt::KeepAspectRatio);
    zoomTo(qMin(1.0, qMin(b.width() / a.width(), b.height() / a.height())));
}

void ImageView::zoomTo100()
{
    zoomTo(1.0);
}

void ImageView::zoomTo(const qreal zoomLevel)
{
    zoomTo(zoomLevel, viewportCenter());
}

void ImageView::zoomTo(const qreal zoomLevel, const QPoint &focalPoint)
{
    if (!m_imageLabel->pixmap()) {
        // Image has not been set yet.
        return;
    }

    m_zoomLevel = qMax(0.1, qMin(3.0, zoomLevel));
    m_imageLabel->resize(m_zoomLevel * m_imageLabel->pixmap()->size());

    QSize currentSize(m_imageLabel->size());
    QSize pixmapSize(m_imageLabel->pixmap()->size());

    // If the current zoom exceeds the loaded pixmap, load the full size image
    // first and then zoom. This needs to be done, because initial images are
    // down-scaled versions to make the UI snappier. Here we load the full size
    // image Just-In-Time.
    if (currentSize.width() > pixmapSize.width() ||
        currentSize.height() > pixmapSize.height()) {
        if (m_imageSize != pixmapSize) {
            QPixmap pixmap(m_imageReader.fileName());
            m_imageLabel->setPixmap(pixmap.transformed(m_transform));
            zoomTo(currentSize.width() / qreal(pixmap.size().width()),
                   focalPoint);
        }
    }

    adjustScrollBars(focalPoint);
}

void ImageView::rotate(qreal degrees)
{
    if (!m_imageLabel->pixmap()) {
        // Image has not been set yet.
        return;
    }

    m_transform = m_transform.rotate(degrees);
    m_imageLabel->setPixmap(m_imageLabel->pixmap()->transformed(
                                QTransform().rotate(degrees)));
    zoomTo(m_zoomLevel * 1.0);
}

void ImageView::rotateLeft()
{
    rotate(-90);
}

void ImageView::rotateRight()
{
    rotate(90);
}

void ImageView::showEvent(QShowEvent *event)
{
    QScrollArea::showEvent(event);
    loadImage();
}

void ImageView::wheelEvent(QWheelEvent *event)
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

QSize ImageView::sizeHint() const
{
    return QSize(640, 480);
}
