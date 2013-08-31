#ifndef IMAGEBROWSER_H
#define IMAGEBROWSER_H

#include <QListView>
#include <QHash>
#include <QMap>
#include <QStandardItemModel>

#include "imageinfowidget.hh"

class ImageBrowser : public QListView
{
    Q_OBJECT
    
public:
    explicit ImageBrowser(QWidget *parent = 0);
    ~ImageBrowser();

    void addImage(QMap<QString, QString> imageinfo);

signals:
    void currentImageChanged(QMap<QString, QString> imageInfo);

public slots:
    void sortOldestFirst();
    void sortOldestLast();
    void sortLastModifiedFirst();
    void sortLastModifiedLast();

protected:
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    QHash<QString, QMap<QString, QString> > m_imageInfoMap;
    ImageInfoWidget *m_imageInfoWidget;
};

#endif // IMAGEBROWSER_H
