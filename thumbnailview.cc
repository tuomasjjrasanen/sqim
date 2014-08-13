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

#include <QDir>
#include <QFileInfo>
#include <QPixmap>

#include "thumbnailview.hh"

enum {
    COL_THUMBNAILFILEPATH,
    COL_FILEPATH,
    COL_TIMESTAMP,
    COL_MTIME,
    COL_FILESIZE,
    COL_IMGSIZE,
    COLS
};

ThumbnailView::ThumbnailView(QWidget *parent) :
    QListView(parent)
{
    setViewMode(QListView::IconMode);
    setMovement(QListView::Static);
    setSelectionMode(QListView::SingleSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setResizeMode(QListView::Adjust);
    setIconSize(QSize(50, 50));
    setStyleSheet("background: grey;");
    setUniformItemSizes(true);

    setModel(new QStandardItemModel(this));
}

ThumbnailView::~ThumbnailView()
{
}

bool ThumbnailView::addThumbnail(QMap<QString, QString> imageInfo)
{
    if (m_imageInfoMap.contains(imageInfo.value("filepath")))
        return false;

    QList<QStandardItem*> items;
    QStandardItem *item;

    QFileInfo imageFileInfo(imageInfo.value("filepath"));
    QDir cacheDir(QDir::homePath()
                  + "/.cache/sqim"
                  + imageFileInfo.canonicalFilePath());
    QFileInfo thumbnailFileInfo(cacheDir, "thumbnail.png");

    item = new QStandardItem();
    item->setIcon(QIcon(thumbnailFileInfo.filePath()));
    items.insert(COL_THUMBNAILFILEPATH, item);

    item = new QStandardItem();
    item->setText(imageInfo.value("filepath"));
    items.insert(COL_FILEPATH, item);

    item = new QStandardItem();
    item->setText(imageInfo.value("timestamp"));
    items.insert(COL_TIMESTAMP, item);

    item = new QStandardItem();
    item->setText(imageInfo.value("modificationTime"));
    items.insert(COL_MTIME, item);

    item = new QStandardItem();
    item->setText(imageInfo.value("fileSize"));
    items.insert(COL_FILESIZE, item);

    item = new QStandardItem();
    item->setText(imageInfo.value("imageSize"));
    items.insert(COL_IMGSIZE, item);

    ((QStandardItemModel *)model())->appendRow(items);

    m_imageInfoMap.insert(imageInfo.value("filepath"), imageInfo);

    if (((QStandardItemModel *)model())->rowCount() == 1) {
        setCurrentIndex(((QStandardItemModel *)model())->index(0, 0));
    }

    return true;
}

void ThumbnailView::sortAscTimeOrder()
{
    model()->sort(COL_TIMESTAMP, Qt::AscendingOrder);
}

void ThumbnailView::sortDescTimeOrder()
{
    model()->sort(COL_TIMESTAMP, Qt::DescendingOrder);
}

void ThumbnailView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    QListView::currentChanged(current, previous);

    QStandardItemModel *m = (QStandardItemModel*) model();
    QMap<QString, QString> imageInfo = m_imageInfoMap.value(m->item(current.row(), COL_FILEPATH)->text());
    emit currentThumbnailChanged(imageInfo);
}
