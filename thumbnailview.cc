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

#include <QDir>
#include <QFileInfo>
#include <QPixmap>

#include "metadata.hh"
#include "common.hh"
#include "thumbnailview.hh"

enum {
    COL_THUMBNAIL,
    COL_FILEPATH,
    COL_TIMESTAMP,
    COL_MTIME,
    COL_FILESIZE,
    COL_IMGSIZE,
    COLS
};

ThumbnailView::ThumbnailView(QWidget *parent) :
    QListView(parent)
    ,m_sortAscTimeOrderAction(new QAction(QIcon(":/icons/sort_asc_date.png"),
                                          "&Ascending time order", this))
    ,m_sortDescTimeOrderAction(new QAction(QIcon(":/icons/sort_desc_date.png"),
                                           "&Descending time order", this))
{
    setViewMode(QListView::IconMode);
    setMovement(QListView::Static);
    setSelectionMode(QListView::ExtendedSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setResizeMode(QListView::Adjust);
    setIconSize(QSize(50, 50));
    setStyleSheet("QListView {background-color: grey}");
    setUniformItemSizes(true);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    setModel(new QStandardItemModel(this));

    m_sortAscTimeOrderAction->setShortcut(
        QKeySequence(Qt::Key_Less, Qt::Key_T));
    m_sortDescTimeOrderAction->setShortcut(
        QKeySequence(Qt::Key_Greater, Qt::Key_T));

    addAction(m_sortAscTimeOrderAction);
    addAction(m_sortDescTimeOrderAction);

    connect(m_sortAscTimeOrderAction, SIGNAL(triggered(bool)),
            SLOT(sortAscTimeOrder()));
    connect(m_sortDescTimeOrderAction, SIGNAL(triggered(bool)),
            SLOT(sortDescTimeOrder()));
    connect(this, SIGNAL(activated(const QModelIndex&)),
            this, SLOT(emitCurrentThumbnailActivated(const QModelIndex&)));

    m_sortAscTimeOrderAction->setEnabled(false);
    m_sortDescTimeOrderAction->setEnabled(false);
}

ThumbnailView::~ThumbnailView()
{
}

void ThumbnailView::hideEvent(QHideEvent *event)
{
    m_sortAscTimeOrderAction->setEnabled(false);
    m_sortDescTimeOrderAction->setEnabled(false);
    QListView::hideEvent(event);
}

void ThumbnailView::showEvent(QShowEvent *event)
{
    m_sortAscTimeOrderAction->setEnabled(!m_imageFilePaths.isEmpty());
    m_sortDescTimeOrderAction->setEnabled(!m_imageFilePaths.isEmpty());
    QListView::showEvent(event);
}

bool ThumbnailView::addThumbnail(const QString& filePath)
{
    if (m_imageFilePaths.contains(filePath))
        return false;

    Metadata metadata;

    readMetadata(filePath, metadata);

    QList<QStandardItem*> items;
    QStandardItem *item;

    item = new QStandardItem();
    item->setIcon(QIcon(cacheDir(filePath).filePath("thumbnail.png")));
    items.insert(COL_THUMBNAIL, item);

    item = new QStandardItem();
    item->setText(filePath);
    items.insert(COL_FILEPATH, item);

    item = new QStandardItem();
    item->setText(metadata.value("timestamp"));
    items.insert(COL_TIMESTAMP, item);

    item = new QStandardItem();
    item->setText(metadata.value("modificationTime"));
    items.insert(COL_MTIME, item);

    item = new QStandardItem();
    item->setText(metadata.value("fileSize"));
    items.insert(COL_FILESIZE, item);

    item = new QStandardItem();
    item->setText(metadata.value("imageSize"));
    items.insert(COL_IMGSIZE, item);

    ((QStandardItemModel *)model())->appendRow(items);

    m_imageFilePaths << filePath;

    m_sortAscTimeOrderAction->setEnabled(true);
    m_sortDescTimeOrderAction->setEnabled(true);

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

void ThumbnailView::emitCurrentThumbnailActivated(const QModelIndex &current)
{
    QStandardItemModel *m = (QStandardItemModel*) model();
    QString filePath = m->item(current.row(), COL_FILEPATH)->text();
    emit currentThumbnailActivated(filePath);
}

void ThumbnailView::currentChanged(const QModelIndex &current,
                                   const QModelIndex &previous)
{
    QListView::currentChanged(current, previous);

    QStandardItemModel *m = (QStandardItemModel*) model();
    QString filePath = m->item(current.row(), COL_FILEPATH)->text();
    emit currentThumbnailChanged(filePath);
}
