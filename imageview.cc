#include "imageview.hh"
#include <QGridLayout>

ImageView::ImageView(QWidget *parent) :
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

ImageView::~ImageView()
{
}

void ImageView::loadImage(const QString &imagePath)
{
    QStandardItem *item = new QStandardItem();
    item->setIcon(QIcon(imagePath));
    m_iconModel->appendRow(item);
}
