#include <QPixmap>

#include "imagebrowser.hh"

enum {
    COL_THUMBNAIL,
    COL_FILEPATH,
    COL_TIMESTAMP,
    COL_MTIME,
    COLS
};

ImageBrowser::ImageBrowser(QWidget *parent) :
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

ImageBrowser::~ImageBrowser()
{
}

void ImageBrowser::addImage(ImageInfo imageInfo)
{
    if (m_imageInfoMap.contains(imageInfo.filepath()))
        return;

    QList<QStandardItem*> items;
    QStandardItem *item;

    item = new QStandardItem();
    item->setIcon(QIcon(QPixmap::fromImage(imageInfo.thumbnail())));
    items.insert(COL_THUMBNAIL, item);

    item = new QStandardItem();
    item->setText(imageInfo.filepath());
    items.insert(COL_FILEPATH, item);

    item = new QStandardItem();
    item->setText(imageInfo.timestamp());
    items.insert(COL_TIMESTAMP, item);

    item = new QStandardItem();
    item->setText(imageInfo.modificationTime());
    items.insert(COL_MTIME, item);

    ((QStandardItemModel *)model())->appendRow(items);

    m_imageInfoMap[imageInfo.filepath()] = imageInfo;
}

void ImageBrowser::sortOldestFirst()
{
    model()->sort(COL_TIMESTAMP, Qt::AscendingOrder);
}

void ImageBrowser::sortOldestLast()
{
    model()->sort(COL_TIMESTAMP, Qt::DescendingOrder);
}

void ImageBrowser::sortLastModifiedFirst()
{
    model()->sort(COL_MTIME, Qt::DescendingOrder);
}

void ImageBrowser::sortLastModifiedLast()
{
    model()->sort(COL_MTIME, Qt::AscendingOrder);
}

void ImageBrowser::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    QListView::currentChanged(current, previous);

    QStandardItemModel *m = (QStandardItemModel*) model();
    ImageInfo imageInfo = m_imageInfoMap[m->item(current.row(), COL_FILEPATH)->text()];
    emit currentImageChanged(imageInfo);
}
