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
#include <QInputDialog>
#include <QProcess>
#include <QSqlQuery>
#include <QSqlRecord>
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
    ,m_removeAction(new QAction("Remove", this))
    ,m_tagAction(new QAction("Add tag", this))
    ,m_tagModel(new QSqlQueryModel(this))
    ,m_tagView(new QListView(this))
{
    updateTags();

    m_tagView->setModel(m_tagModel);
    m_tagView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tagView->setSelectionMode(QAbstractItemView::MultiSelection);
    m_tagView->setViewMode(QListView::IconMode);
    m_tagView->setMovement(QListView::Static);
    m_tagView->setSelectionRectVisible(false);
    m_tagView->setSpacing(10);
    m_tagView->setResizeMode(QListView::Adjust);

    m_thumbnailView->setViewMode(QListView::IconMode);
    m_thumbnailView->setMovement(QListView::Static);
    m_thumbnailView->setSelectionMode(QListView::ExtendedSelection);
    m_thumbnailView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_thumbnailView->setResizeMode(QListView::Adjust);
    m_thumbnailView->setIconSize(QSize(80, 80));
    m_thumbnailView->setUniformItemSizes(true);
    m_thumbnailView->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_thumbnailView->setModel(m_thumbnailModel);

    setContextMenuPolicy(Qt::ActionsContextMenu);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_toolBar);
    layout->addWidget(m_thumbnailView);
    layout->addWidget(m_tagView);
    layout->setStretch(0, 1);
    layout->setStretch(1, 10);
    layout->setStretch(2, 2);
    setLayout(layout);

    m_sortActionGroup->setExclusive(true);

    m_sortAscTimeOrderAction->setShortcut(
        QKeySequence(Qt::Key_Less, Qt::Key_T));
    m_sortDescTimeOrderAction->setShortcut(
        QKeySequence(Qt::Key_Greater, Qt::Key_T));
    m_editAction->setShortcut(QKeySequence(Qt::Key_E));
    m_removeAction->setShortcut(QKeySequence(Qt::Key_Delete));

    setActionsEnabled(false);

    m_sortAscTimeOrderAction->setCheckable(true);
    m_sortDescTimeOrderAction->setCheckable(true);

    addAction(m_sortAscTimeOrderAction);
    addAction(m_sortDescTimeOrderAction);
    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    addAction(separator);
    addAction(m_editAction);
    addAction(m_removeAction);
    addAction(m_tagAction);

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
    connect(m_removeAction, SIGNAL(triggered(bool)),
            SLOT(removeSelectedThumbnails()));
    connect(m_tagAction, SIGNAL(triggered(bool)),
            SLOT(tagSelectedThumbnails()));
}

ThumbnailWidget::~ThumbnailWidget()
{
}

void ThumbnailWidget::setActionsEnabled(bool enabled)
{
    m_sortAscTimeOrderAction->setEnabled(enabled);
    m_sortDescTimeOrderAction->setEnabled(enabled);
    m_editAction->setEnabled(enabled);
    m_removeAction->setEnabled(enabled);
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

    setActionsEnabled(true);

    if (m_thumbnailModel->rowCount() == 1) {
        m_thumbnailView->setCurrentIndex(m_thumbnailModel->index(0, 0));
    }

    return true;
}

void ThumbnailWidget::removeSelectedThumbnails()
{
    m_thumbnailView->setUpdatesEnabled(false);

    QModelIndex currentIndex = m_thumbnailView->currentIndex();
    QItemSelectionModel *selectionModel = m_thumbnailView->selectionModel();
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();

    qSort(selectedIndexes.begin(), selectedIndexes.end());

    while (selectedIndexes.size()) {
        QModelIndex index = selectedIndexes.takeLast();
        Metadata metadata = m_thumbnailModel->data(index,
                                                   MetadataRole).toHash();
        m_imageFilePaths.remove(metadata.value("filePath").toString());
        m_thumbnailModel->removeRow(index.row());
    }

    if (currentIndex.row() < m_thumbnailModel->rowCount()) {
        m_thumbnailView->setCurrentIndex(currentIndex);
    } else {
        QModelIndex nextCurrentIndex = m_thumbnailModel->index(
            m_thumbnailModel->rowCount() - 1,
            m_thumbnailModel->columnCount() - 1);
        m_thumbnailView->setCurrentIndex(nextCurrentIndex);
    }

    if (m_thumbnailModel->rowCount() == 0)
        setActionsEnabled(false);

    m_thumbnailView->setUpdatesEnabled(true);
}

void ThumbnailWidget::tagSelectedThumbnails()
{
    QStringList tags;
    for (int i = 0; i < m_tagModel->rowCount(); ++i) {
        tags << m_tagModel->record(i).value(0).toString();
    }
    QString tag = QInputDialog::getItem(this, "Add tag to selected images",
                                        "Tag", tags);

    QItemSelectionModel *selectionModel = m_thumbnailView->selectionModel();
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();

    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();

    foreach (QModelIndex index, selectedIndexes) {
        Metadata metadata = m_thumbnailModel->data(index,
                                                   MetadataRole).toHash();
        QSqlQuery query;
        if (!query.prepare("INSERT INTO Tagging(file_path, tag) VALUES(?, ?)"))
            continue;

        query.addBindValue(metadata.value("filePath").toString());
        query.addBindValue(tag);

        query.exec();
    }

    db.commit();
    updateTags();
}

void ThumbnailWidget::updateTags()
{
    QSqlQuery query;
    query.exec("SELECT DISTINCT(tag) FROM Tagging ORDER BY tag;");
    m_tagModel->setQuery(query);
}

void ThumbnailWidget::clear()
{
    m_thumbnailModel->clear();
    m_imageFilePaths.clear();
    setActionsEnabled(false);
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
