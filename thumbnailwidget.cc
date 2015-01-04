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
#include <QIcon>
#include <QProcess>
#include <QVBoxLayout>

#include "common.hh"
#include "thumbnaildelegate.hh"
#include "thumbnailwidget.hh"

ThumbnailWidget::ThumbnailWidget(QWidget* parent)
    :QWidget(parent)
    ,m_thumbnailView(new ThumbnailView(this))
    ,m_toolBar(new QToolBar(this))
    ,m_imageFilePaths()
    ,m_sortActionGroup(new QActionGroup(m_toolBar))
    ,m_sortAscTimeOrderAction(new QAction(QIcon(":/icons/sort_asc_date.png"),
                                          "&Ascending time order",
                                          m_sortActionGroup))
    ,m_sortDescTimeOrderAction(new QAction(QIcon(":/icons/sort_desc_date.png"),
                                           "&Descending time order",
                                           m_sortActionGroup))
    ,m_thumbnailModel(new QStandardItemModel(this))
    ,m_editAction(new QAction("Edit", this))
{
    m_thumbnailView->setViewMode(QListView::IconMode);
    m_thumbnailView->setMovement(QListView::Static);
    m_thumbnailView->setSelectionMode(QListView::ExtendedSelection);
    m_thumbnailView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_thumbnailView->setResizeMode(QListView::Adjust);
    m_thumbnailView->setIconSize(QSize(50, 50));
    m_thumbnailView->setStyleSheet("QListView {background-color: grey}");
    m_thumbnailView->setUniformItemSizes(true);
    m_thumbnailView->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_thumbnailView->setModel(m_thumbnailModel);

    QLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_toolBar);
    layout->addWidget(m_thumbnailView);
    setLayout(layout);

    m_sortActionGroup->setExclusive(true);

    m_sortAscTimeOrderAction->setShortcut(
        QKeySequence(Qt::Key_Less, Qt::Key_T));
    m_sortDescTimeOrderAction->setShortcut(
        QKeySequence(Qt::Key_Greater, Qt::Key_T));
    m_editAction->setShortcut(QKeySequence(Qt::Key_E));

    m_sortAscTimeOrderAction->setEnabled(false);
    m_sortDescTimeOrderAction->setEnabled(false);
    m_editAction->setEnabled(false);

    m_sortAscTimeOrderAction->setCheckable(true);
    m_sortDescTimeOrderAction->setCheckable(true);

    addAction(m_sortAscTimeOrderAction);
    addAction(m_sortDescTimeOrderAction);
    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    addAction(separator);
    addAction(m_editAction);

    foreach (QAction* action, actions()) {
        m_toolBar->addAction(action);
    }

    m_thumbnailView->setItemDelegate(
        new ThumbnailDelegate(m_thumbnailView, this));

    connect(m_thumbnailView, SIGNAL(currentThumbnailChanged(Metadata)),
            SIGNAL(currentThumbnailChanged(Metadata)));
    connect(m_thumbnailView, SIGNAL(activated(const QModelIndex&)),
            this, SLOT(emitCurrentThumbnailActivated(const QModelIndex&)));

    connect(m_sortAscTimeOrderAction, SIGNAL(triggered(bool)),
            SLOT(sortAscTimeOrder()));
    connect(m_sortDescTimeOrderAction, SIGNAL(triggered(bool)),
            SLOT(sortDescTimeOrder()));
    connect(m_editAction, SIGNAL(triggered(bool)),
            SLOT(editSelectedThumbnails()));
}

ThumbnailWidget::~ThumbnailWidget()
{
}

bool ThumbnailWidget::addThumbnail(const Metadata metadata)
{
    QString filePath = metadata.value("filePath").toString();

    if (m_imageFilePaths.contains(filePath))
        return false;

    QStandardItem* item = new QStandardItem();
    item->setIcon(QIcon(cacheDir(filePath).filePath("thumbnail.png")));
    item->setData(metadata, MetadataRole);
    item->setData(metadata.value("timestamp").toDateTime(), TimestampRole);

    m_thumbnailModel->appendRow(item);

    m_imageFilePaths << filePath;

    m_sortAscTimeOrderAction->setEnabled(true);
    m_sortDescTimeOrderAction->setEnabled(true);
    m_editAction->setEnabled(true);

    if (m_thumbnailModel->rowCount() == 1) {
        m_thumbnailView->setCurrentIndex(m_thumbnailModel->index(0, 0));
    }

    return true;
}

void ThumbnailWidget::clear()
{
    m_thumbnailModel->clear();
    m_imageFilePaths.clear();
    m_editAction->setEnabled(false);
}

void ThumbnailWidget::sortAscTimeOrder()
{
    m_thumbnailModel->setSortRole(TimestampRole);
    m_thumbnailModel->sort(0, Qt::AscendingOrder);
}

void ThumbnailWidget::sortDescTimeOrder()
{
    m_thumbnailModel->setSortRole(TimestampRole);
    m_thumbnailModel->sort(0, Qt::DescendingOrder);
}

void ThumbnailWidget::emitCurrentThumbnailActivated(const QModelIndex& current)
{
    QVariant data = m_thumbnailModel->item(current.row())->data();
    emit currentThumbnailActivated(data.toHash());
}

void ThumbnailWidget::triggerSortAscTimeOrder()
{
    m_sortAscTimeOrderAction->trigger();
}

void ThumbnailWidget::triggerSortDescTimeOrder()
{
    m_sortDescTimeOrderAction->trigger();
}

void ThumbnailWidget::setCurrentIndex(int index)
{
    m_thumbnailView->setCurrentIndex(m_thumbnailModel->index(index, 0));
}

void ThumbnailWidget::editSelectedThumbnails()
{
    QModelIndex currentIndex = m_thumbnailView->currentIndex();
    QItemSelectionModel *selectionModel = m_thumbnailView->selectionModel();
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();
    QStringList filePaths;

    foreach (QModelIndex index, selectedIndexes) {
        Metadata metadata = m_thumbnailModel->data(index,
                                                   MetadataRole).toHash();
        filePaths.append(metadata.value("filePath").toString());
    }

    QProcess::startDetached("gimp", filePaths);
}
