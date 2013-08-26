#include <QGridLayout>
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
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    m_iconView = new QListView(this);

    layout->addWidget(m_iconView);

    m_iconView->setViewMode(QListView::IconMode);
    m_iconView->setMovement(QListView::Static);
    m_iconView->setSelectionMode(QListView::SingleSelection);
    m_iconView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_iconView->setResizeMode(QListView::Adjust);
    m_iconView->setIconSize(QSize(50, 50));
    m_iconView->setStyleSheet("background: grey;");

    m_iconModel = new QStandardItemModel(this);
    m_iconView->setModel(m_iconModel);
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

    m_iconModel->appendRow(items);

    m_itemMap[image.filepath()] = item;
}

void ImageBrowser::sortOlderFirst()
{
    m_iconModel->sort(COL_TIMESTAMP, Qt::AscendingOrder);
}

void ImageBrowser::sortNewerFirst()
{
    m_iconModel->sort(COL_TIMESTAMP, Qt::DescendingOrder);
}
