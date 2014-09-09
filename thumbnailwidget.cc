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

#include <QIcon>
#include <QVBoxLayout>

#include "thumbnaildelegate.hh"
#include "thumbnailwidget.hh"

ThumbnailWidget::ThumbnailWidget(QWidget* parent)
    :QWidget(parent)
    ,m_thumbnailView(new ThumbnailView(this))
    ,m_toolBar(new QToolBar(this))
    ,m_imageFilePaths()
    ,m_sortAscTimeOrderAction(new QAction(QIcon(":/icons/sort_asc_date.png"),
                                          "&Ascending time order", this))
    ,m_sortDescTimeOrderAction(new QAction(QIcon(":/icons/sort_desc_date.png"),
                                           "&Descending time order", this))
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
    m_thumbnailView->setModel(new QStandardItemModel(this));

    QLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_toolBar);
    layout->addWidget(m_thumbnailView);
    setLayout(layout);

    m_sortAscTimeOrderAction->setShortcut(
        QKeySequence(Qt::Key_Less, Qt::Key_T));
    m_sortDescTimeOrderAction->setShortcut(
        QKeySequence(Qt::Key_Greater, Qt::Key_T));

    m_sortAscTimeOrderAction->setEnabled(false);
    m_sortDescTimeOrderAction->setEnabled(false);

    addAction(m_sortAscTimeOrderAction);
    addAction(m_sortDescTimeOrderAction);

    foreach (QAction* action, actions()) {
        m_toolBar->addAction(action);
    }

    m_thumbnailView->setItemDelegate(
        new ThumbnailDelegate(m_thumbnailView, this));

    connect(m_thumbnailView, SIGNAL(currentThumbnailChanged(Metadata)),
            SIGNAL(currentThumbnailChanged(Metadata)));
    connect(m_thumbnailView, SIGNAL(currentThumbnailActivated(Metadata)),
            SIGNAL(currentThumbnailActivated(Metadata)));

    connect(m_sortAscTimeOrderAction, SIGNAL(triggered(bool)),
            SLOT(sortAscTimeOrder()));
    connect(m_sortDescTimeOrderAction, SIGNAL(triggered(bool)),
            SLOT(sortDescTimeOrder()));

}

ThumbnailWidget::~ThumbnailWidget()
{
}

bool ThumbnailWidget::addThumbnail(const Metadata metadata)
{
    QString filePath = metadata.value("filePath").toString();

    if (m_imageFilePaths.contains(filePath))
        return false;

    if (m_thumbnailView->addThumbnail(metadata)) {
        m_imageFilePaths << filePath;
        m_sortAscTimeOrderAction->setEnabled(true);
        m_sortDescTimeOrderAction->setEnabled(true);
        return true;
    }

    return false;
}

void ThumbnailWidget::clear()
{
    qobject_cast<QStandardItemModel*>(m_thumbnailView->model())->clear();
    m_imageFilePaths.clear();
}

void ThumbnailWidget::sortAscTimeOrder()
{
    qobject_cast<QStandardItemModel*>(m_thumbnailView->model())->setSortRole(TimestampRole);
    m_thumbnailView->model()->sort(0, Qt::AscendingOrder);
}

void ThumbnailWidget::sortDescTimeOrder()
{
    qobject_cast<QStandardItemModel*>(m_thumbnailView->model())->setSortRole(TimestampRole);
    m_thumbnailView->model()->sort(0, Qt::DescendingOrder);
}
