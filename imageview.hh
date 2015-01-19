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

#ifndef IMAGEVIEW_HH
#define IMAGEVIEW_HH

#include <QtGui>

#include "metadata.hh"

class ImageView : public QScrollArea
{
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = 0);
    ~ImageView();

    virtual QSize sizeHint() const;

public slots:
    void setImage(const QModelIndex& current);
    void zoomIn();
    void zoomOut();
    void zoomIn(const QPoint &focalPoint);
    void zoomOut(const QPoint &focalPoint);
    void zoomBy(qreal zoomFactor);
    void zoomBy(qreal zoomFactor, const QPoint &focalPoint);
    void zoomToFit();
    void zoomTo100();
    void zoomTo(qreal zoomLevel);
    void zoomTo(qreal zoomLevel, const QPoint &focalPoint);
    void rotateRight();
    void rotateLeft();

protected:
    virtual void wheelEvent(QWheelEvent *event);
    virtual void showEvent(QShowEvent *event);

private:
    void adjustScrollBars(const QPoint &focalPoint);
    const QPoint viewportCenter() const;
    void rotate(qreal degrees);
    void loadImage();

    QLabel *m_imageLabel;
    qreal m_zoomLevel;

    bool m_isImageLoaded;
    QString m_imageFilePath;
    int m_imageOrientation;

    QImageReader m_imageReader;
    QSize m_imageSize;

    QTransform m_transform;
};

#endif // IMAGEVIEW_HH
