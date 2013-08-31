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

void ImageBrowser::addImage(const Image &image)
{
    if (m_itemMap.contains(image.filepath()))
        return;

    QList<QStandardItem*> items;
    QStandardItem *item;

    item = new QStandardItem();
    item->setIcon(QIcon(QPixmap::fromImage(image.thumbnail())));
    items.insert(COL_THUMBNAIL, item);

    item = new QStandardItem();
    item->setText(image.filepath());
    items.insert(COL_FILEPATH, item);

    item = new QStandardItem();
    item->setText(image.timestamp());
    items.insert(COL_TIMESTAMP, item);

    item = new QStandardItem();
    item->setText(image.modificationTime());
    items.insert(COL_MTIME, item);

    ((QStandardItemModel *)model())->appendRow(items);

    m_itemMap[image.filepath()] = image;
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
    Image image = m_itemMap[((QStandardItemModel*)model())->item(current.row(), COL_FILEPATH)->text()];
    emit currentImageChanged(image);
}
