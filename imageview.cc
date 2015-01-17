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

#include <QPixmapCache>
#include <QScrollBar>

#include "imageview.hh"

ImageView::ImageView(QWidget *parent)
    :QScrollArea(parent)
    ,m_imageLabel(new QLabel(this))
    ,m_rotateLeftAction(new QAction(QIcon(":/icons/rotate_left.png"),
                                    "Rotate left", this))
    ,m_rotateRightAction(new QAction(QIcon(":/icons/rotate_right.png"),
                                     "Rotate right", this))
    ,m_zoomInAction(new QAction(QIcon(":/icons/zoom_in.png"),
                                "&Zoom in", this))
    ,m_zoomOutAction(new QAction(QIcon(":/icons/zoom_out.png"),
                                 "&Zoom out", this))
    ,m_zoomToFitAction(new QAction(QIcon(":/icons/zoom_to_fit.png"),
                                   "&Zoom to fit", this))
    ,m_zoomTo100Action(new QAction(QIcon(":/icons/zoom_to_100.png"),
                                    "&Zoom to 100%", this))
    ,m_transform()
{
    m_imageLabel->setScaledContents(true);
    setWidget(m_imageLabel);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    m_rotateLeftAction->setShortcut(QKeySequence(Qt::Key_R, Qt::Key_Left));
    m_rotateRightAction->setShortcut(QKeySequence(Qt::Key_R, Qt::Key_Right));
    m_zoomInAction->setShortcut(QKeySequence(Qt::Key_Plus));
    m_zoomOutAction->setShortcut(QKeySequence(Qt::Key_Minus));
    m_zoomToFitAction->setShortcut(QKeySequence(Qt::Key_Equal));

    addAction(m_zoomInAction);
    addAction(m_zoomOutAction);
    addAction(m_zoomToFitAction);
    addAction(m_zoomTo100Action);
    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    addAction(separator);
    addAction(m_rotateLeftAction);
    addAction(m_rotateRightAction);

    connect(m_zoomInAction, SIGNAL(triggered(bool)), SLOT(zoomIn()));
    connect(m_zoomOutAction, SIGNAL(triggered(bool)), SLOT(zoomOut()));
    connect(m_zoomToFitAction, SIGNAL(triggered(bool)), SLOT(zoomToFit()));
    connect(m_zoomTo100Action, SIGNAL(triggered(bool)), SLOT(zoomTo100()));
    connect(m_rotateLeftAction, SIGNAL(triggered(bool)), SLOT(rotateLeft()));
    connect(m_rotateRightAction, SIGNAL(triggered(bool)), SLOT(rotateRight()));

    // Image operations are disabled by default, because image has not
    // been set yet.
    m_zoomInAction->setEnabled(false);
    m_zoomOutAction->setEnabled(false);
    m_zoomToFitAction->setEnabled(false);
    m_zoomTo100Action->setEnabled(false);
    m_rotateLeftAction->setEnabled(false);
    m_rotateRightAction->setEnabled(false);
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

    // Actions are enabled only if an image is exists. Actions are not
    // meaningful without an image.
    m_rotateLeftAction->setEnabled(m_imageLabel->pixmap());
    m_rotateRightAction->setEnabled(m_imageLabel->pixmap());
    m_zoomInAction->setEnabled(m_imageLabel->pixmap());
    m_zoomOutAction->setEnabled(m_imageLabel->pixmap());
    m_zoomToFitAction->setEnabled(m_imageLabel->pixmap());
    m_zoomTo100Action->setEnabled(m_imageLabel->pixmap());
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