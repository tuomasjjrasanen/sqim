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

#include <QScrollBar>

#include "imagearea.hh"

ImageArea::ImageArea(QWidget *parent)
    :QScrollArea(parent)
    ,m_imageLabel(new QLabel(this))
    ,m_rotateLeftAction(new QAction("Rotate left", this))
    ,m_rotateRightAction(new QAction("Rotate right", this))
    ,m_zoomInAction(new QAction("&Zoom in", this))
    ,m_zoomOutAction(new QAction("&Zoom out", this))
    ,m_zoomToFitAction(new QAction("&Zoom to fit", this))
{
    m_imageLabel->setScaledContents(true);
    setWidget(m_imageLabel);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    setStyleSheet("QScrollArea, QLabel {background-color: grey}");
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    m_zoomInAction->setShortcut(QKeySequence(Qt::Key_Plus));
    m_zoomOutAction->setShortcut(QKeySequence(Qt::Key_Minus));
    m_zoomToFitAction->setShortcut(QKeySequence(Qt::Key_Equal));

    addAction(m_zoomInAction);
    addAction(m_zoomOutAction);
    addAction(m_zoomToFitAction);
    addAction(m_rotateLeftAction);
    addAction(m_rotateRightAction);

    connect(m_zoomInAction, SIGNAL(triggered(bool)), SLOT(zoomIn()));
    connect(m_zoomOutAction, SIGNAL(triggered(bool)), SLOT(zoomOut()));
    connect(m_zoomToFitAction, SIGNAL(triggered(bool)), SLOT(zoomToFit()));
    connect(m_rotateLeftAction, SIGNAL(triggered(bool)), SLOT(rotateLeft()));
    connect(m_rotateRightAction, SIGNAL(triggered(bool)), SLOT(rotateRight()));

    // Image operations are disabled by default, because image has not
    // been set yet.
    m_zoomInAction->setEnabled(false);
    m_zoomOutAction->setEnabled(false);
    m_zoomToFitAction->setEnabled(false);
    m_rotateLeftAction->setEnabled(false);
    m_rotateRightAction->setEnabled(false);
}

ImageArea::~ImageArea()
{
}

void ImageArea::setImage(const QString& filePath)
{
    QImage image(filePath);
    QPixmap pixmap(QPixmap::fromImage(image));
    m_imageLabel->setPixmap(pixmap);

    zoomToFit();

    // Actions are enabled only if an image is exists. Actions are not
    // meaningful without an image.
    m_rotateLeftAction->setEnabled(m_imageLabel->pixmap());
    m_rotateRightAction->setEnabled(m_imageLabel->pixmap());
    m_zoomInAction->setEnabled(m_imageLabel->pixmap());
    m_zoomOutAction->setEnabled(m_imageLabel->pixmap());
    m_zoomToFitAction->setEnabled(m_imageLabel->pixmap());
}

const QPoint ImageArea::viewportCenter() const {
    return QPoint(viewport()->width() / 2, viewport()->height() / 2);
}

void ImageArea::zoomIn()
{
    zoomIn(viewportCenter());
}

void ImageArea::zoomOut()
{
    zoomOut(viewportCenter());
}

void ImageArea::zoomIn(const QPoint &focalPoint)
{
    zoomBy(1.25, focalPoint);
}

void ImageArea::zoomOut(const QPoint &focalPoint)
{
    zoomBy(0.8, focalPoint);
}

void ImageArea::zoomBy(const qreal zoomFactor)
{
    zoomTo(m_zoomLevel * zoomFactor);
}

void ImageArea::zoomBy(const qreal zoomFactor, const QPoint &focalPoint)
{
    zoomTo(m_zoomLevel * zoomFactor, focalPoint);
}

void ImageArea::adjustScrollBars(const QPoint &focalPoint)
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

void ImageArea::zoomToFit()
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

void ImageArea::zoomTo(const qreal zoomLevel)
{
    zoomTo(zoomLevel, viewportCenter());
}

void ImageArea::zoomTo(const qreal zoomLevel, const QPoint &focalPoint)
{
    if (!m_imageLabel->pixmap()) {
        // Image has not been set yet.
        return;
    }

    m_zoomLevel = qMax(0.1, qMin(3.0, zoomLevel));
    m_imageLabel->resize(m_zoomLevel * m_imageLabel->pixmap()->size());

    adjustScrollBars(focalPoint);
}

void ImageArea::rotate(qreal degrees)
{
    if (!m_imageLabel->pixmap()) {
        // Image has not been set yet.
        return;
    }

    m_imageLabel->setPixmap(m_imageLabel->pixmap()->transformed(
                                QTransform().rotate(degrees)));
    zoomTo(m_zoomLevel * 1.0);
}

void ImageArea::rotateLeft()
{
    rotate(-90);
}

void ImageArea::rotateRight()
{
    rotate(90);
}

void ImageArea::wheelEvent(QWheelEvent *event)
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

QSize ImageArea::sizeHint() const
{
    return QSize(480, 360);
}
