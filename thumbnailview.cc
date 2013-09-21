#include <QPixmap>

#include "thumbnailview.hh"

enum {
    COL_THUMBNAILFILEPATH,
    COL_FILEPATH,
    COL_TIMESTAMP,
    COL_MTIME,
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

void ThumbnailView::addThumbnail(QMap<QString, QString> imageInfo)
{
    if (m_imageInfoMap.contains(imageInfo.value("filepath")))
        return;

    QList<QStandardItem*> items;
    QStandardItem *item;

    item = new QStandardItem();
    item->setIcon(QIcon(imageInfo.value("thumbnailFilepath")));
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

    ((QStandardItemModel *)model())->appendRow(items);

    m_imageInfoMap.insert(imageInfo.value("filepath"), imageInfo);
}

void ThumbnailView::sortOldestFirst()
{
    model()->sort(COL_TIMESTAMP, Qt::AscendingOrder);
}

void ThumbnailView::sortOldestLast()
{
    model()->sort(COL_TIMESTAMP, Qt::DescendingOrder);
}

void ThumbnailView::sortLastModifiedFirst()
{
    model()->sort(COL_MTIME, Qt::DescendingOrder);
}

void ThumbnailView::sortLastModifiedLast()
{
    model()->sort(COL_MTIME, Qt::AscendingOrder);
}

void ThumbnailView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    QListView::currentChanged(current, previous);

    QStandardItemModel *m = (QStandardItemModel*) model();
    QMap<QString, QString> imageInfo = m_imageInfoMap.value(m->item(current.row(), COL_FILEPATH)->text());
    emit currentThumbnailChanged(imageInfo);
}
