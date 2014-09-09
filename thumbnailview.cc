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

#include <QDateTime>
#include <QSize>
#include <QDir>
#include <QFileInfo>
#include <QPixmap>

#include "common.hh"
#include "metadata.hh"
#include "thumbnailview.hh"

ThumbnailView::ThumbnailView(QWidget *parent) :
    QListView(parent)
{
}

ThumbnailView::~ThumbnailView()
{
}

bool ThumbnailView::addThumbnail(const Metadata metadata)
{
    QString filePath = metadata.value("filePath").toString();

    QStandardItem* item = new QStandardItem();
    item->setIcon(QIcon(cacheDir(filePath).filePath("thumbnail.png")));
    item->setData(metadata, MetadataRole);
    item->setData(metadata.value("timestamp").toDateTime(), TimestampRole);

    ((QStandardItemModel *)model())->appendRow(item);

    if (((QStandardItemModel *)model())->rowCount() == 1) {
        setCurrentIndex(((QStandardItemModel *)model())->index(0, 0));
    }

    return true;
}

void ThumbnailView::currentChanged(const QModelIndex &current,
                                   const QModelIndex &previous)
{
    QListView::currentChanged(current, previous);

    QStandardItemModel *m = (QStandardItemModel*) model();
    QVariant data = m->item(current.row())->data();
    emit currentThumbnailChanged(data.toHash());
}
