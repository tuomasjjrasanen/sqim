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

#include <QAction>
#include <QLabel>
#include <QScrollArea>
#include <QWheelEvent>

class ImageWidget : public QScrollArea
{
    Q_OBJECT

public:
    explicit ImageWidget(QWidget *parent = 0);
    ~ImageWidget();

    QAction* rotateLeftAction() const;
    QAction* rotateRightAction() const;
    QAction* zoomInAction() const;
    QAction* zoomOutAction() const;
    QAction* zoomToFitAction() const;

    virtual QSize sizeHint() const;

public slots:
    void setImage(const QString& filePath);
    void zoomIn();
    void zoomOut();
    void zoomIn(const QPoint &focalPoint);
    void zoomOut(const QPoint &focalPoint);
    void zoomBy(qreal zoomFactor);
    void zoomBy(qreal zoomFactor, const QPoint &focalPoint);
    void zoomToFit();
    void zoomTo(qreal zoomLevel);
    void zoomTo(qreal zoomLevel, const QPoint &focalPoint);
    void rotateRight();
    void rotateLeft();

protected:
    virtual void wheelEvent(QWheelEvent *event);
    virtual void hideEvent(QHideEvent *event);
    virtual void showEvent(QShowEvent *event);

private:
    void adjustScrollBars(const QPoint &focalPoint);
    const QPoint viewportCenter() const;
    void rotate(qreal degrees);

    QLabel *m_imageLabel;
    qreal m_zoomLevel;

    QAction *m_rotateLeftAction;
    QAction *m_rotateRightAction;
    QAction *m_zoomInAction;
    QAction *m_zoomOutAction;
    QAction *m_zoomToFitAction;

    void disableActions();
    void enableActions();
};

#endif // IMAGEWIDGET_HH
