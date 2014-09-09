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

enum {
    MetadataRole = Qt::UserRole + 1,
    TimestampRole,
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

bool ThumbnailView::addThumbnail(const Metadata metadata)
{
    QString filePath = metadata.value("filePath").toString();

    if (m_imageFilePaths.contains(filePath))
        return false;

    QStandardItem* item = new QStandardItem();
    item->setIcon(QIcon(cacheDir(filePath).filePath("thumbnail.png")));
    item->setData(metadata, MetadataRole);
    item->setData(metadata.value("timestamp").toDateTime(), TimestampRole);

    ((QStandardItemModel *)model())->appendRow(item);

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
    qobject_cast<QStandardItemModel*>(model())->setSortRole(TimestampRole);
    model()->sort(0, Qt::AscendingOrder);
}

void ThumbnailView::sortDescTimeOrder()
{
    qobject_cast<QStandardItemModel*>(model())->setSortRole(TimestampRole);
    model()->sort(0, Qt::DescendingOrder);
}

void ThumbnailView::emitCurrentThumbnailActivated(const QModelIndex &current)
{
    QStandardItemModel *m = (QStandardItemModel*) model();
    QVariant data = m->item(current.row())->data();
    emit currentThumbnailActivated(data.toHash());
}

void ThumbnailView::currentChanged(const QModelIndex &current,
                                   const QModelIndex &previous)
{
    QListView::currentChanged(current, previous);

    QStandardItemModel *m = (QStandardItemModel*) model();
    QVariant data = m->item(current.row())->data();
    emit currentThumbnailChanged(data.toHash());
}

void ThumbnailView::clear()
{
    ((QStandardItemModel*) model())->clear();
    m_imageFilePaths.clear();
}