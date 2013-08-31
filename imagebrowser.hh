#ifndef IMAGEBROWSER_H
#define IMAGEBROWSER_H

#include <QListView>
#include <QHash>
#include <QStandardItemModel>

#include "image.hh"
#include "imageinfowidget.hh"

class ImageBrowser : public QListView
{
    Q_OBJECT
    
public:
    explicit ImageBrowser(QWidget *parent = 0);
    ~ImageBrowser();

    void addImage(const Image &image);

signals:
    void currentImageChanged(Image image);

public slots:
    void sortOldestFirst();
    void sortOldestLast();
    void sortLastModifiedFirst();
    void sortLastModifiedLast();

protected:
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    QHash<QString, Image> m_itemMap;
    ImageInfoWidget *m_imageInfoWidget;
};

#endif // IMAGEBROWSER_H
