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

#ifndef THUMBNAILVIEW_HH
#define THUMBNAILVIEW_HH

#include <QtGui>

#include "metadata.hh"

class ThumbnailView : public QListView
{
    Q_OBJECT
    
public:
    explicit ThumbnailView(QWidget* parent = 0);
    ~ThumbnailView();

signals:
    void currentThumbnailChanged(const QModelIndex& current,
                                 const QModelIndex& previous);

protected:
    virtual void currentChanged(const QModelIndex& current,
                                const QModelIndex& previous);

};

#endif // THUMBNAILVIEW_HH
