#ifndef IMAGEBROWSER_H
#define IMAGEBROWSER_H

#include <QListView>
#include <QHash>
#include <QStandardItemModel>

#include "imageinfowidget.hh"

class ImageBrowser : public QListView
{
    Q_OBJECT
    
public:
    explicit ImageBrowser(QWidget *parent = 0);
    ~ImageBrowser();

    void addImage(ImageInfo imageinfo);

signals:
    void currentImageChanged(ImageInfo imageInfo);

public slots:
    void sortOldestFirst();
    void sortOldestLast();
    void sortLastModifiedFirst();
    void sortLastModifiedLast();

protected:
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    QHash<QString, ImageInfo> m_imageInfoMap;
    ImageInfoWidget *m_imageInfoWidget;
};

#endif // IMAGEBROWSER_H
