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

#ifndef THUMBNAILWIDGET_HH
#define THUMBNAILWIDGET_HH

#include <QAction>
#include <QSet>
#include <QStandardItemModel>
#include <QString>
#include <QToolBar>

#include "thumbnailview.hh"

class ThumbnailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ThumbnailWidget(QWidget* parent = 0);
    ~ThumbnailWidget();

public slots:
    bool addThumbnail(const Metadata metadata);
    void clear();
    void sortAscTimeOrder();
    void sortDescTimeOrder();

signals:
    void currentThumbnailChanged(Metadata metadata);
    void currentThumbnailActivated(Metadata metadata);

private slots:
    void emitCurrentThumbnailActivated(const QModelIndex &current);

private:
    ThumbnailView* m_thumbnailView;
    QToolBar* m_toolBar;

    QSet<QString> m_imageFilePaths;

    QActionGroup* m_sortActionGroup;
    QAction* m_sortAscTimeOrderAction;
    QAction* m_sortDescTimeOrderAction;

    QStandardItemModel* m_thumbnailModel;
};

#endif // THUMBNAILWIDGET_HH
