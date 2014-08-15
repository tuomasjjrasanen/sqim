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

#ifndef THUMBNAILVIEW_H
#define THUMBNAILVIEW_H

#include <QAction>
#include <QListView>
#include <QHash>
#include <QMap>
#include <QStandardItemModel>

#include "metadatawidget.hh"

class ThumbnailView : public QListView
{
    Q_OBJECT
    
public:
    explicit ThumbnailView(QWidget *parent = 0);
    ~ThumbnailView();

    bool addThumbnail(const QString& filePath);
    QAction* sortAscTimeOrderAction() const;
    QAction* sortDescTimeOrderAction() const;

signals:
    void currentThumbnailChanged(QString filePath);

public slots:
    void sortAscTimeOrder();
    void sortDescTimeOrder();

protected:
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    virtual void hideEvent(QHideEvent *event);
    virtual void showEvent(QShowEvent *event);

private:
    QHash<QString, Metadata> m_metadataMap;

    QAction *m_sortAscTimeOrderAction;
    QAction *m_sortDescTimeOrderAction;

};

#endif // THUMBNAILVIEW_H
