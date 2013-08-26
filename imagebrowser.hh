#ifndef IMAGEBROWSER_H
#define IMAGEBROWSER_H

#include <QListView>
#include <QHash>
#include <QStandardItemModel>

#include "image.hh"

class ImageBrowser : public QWidget
{
    Q_OBJECT
    
public:
    explicit ImageBrowser(QWidget *parent = 0);
    ~ImageBrowser();

    void addImage(const Image &image);

public slots:
    void sortOldestFirst();
    void sortOldestLast();

private:
    QListView *m_iconView;
    QStandardItemModel *m_iconModel;
    QHash<QString, QStandardItem*> m_itemMap;
};

#endif // IMAGEBROWSER_H
