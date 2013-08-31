#ifndef THUMBNAILVIEW_H
#define THUMBNAILVIEW_H

#include <QListView>
#include <QHash>
#include <QMap>
#include <QStandardItemModel>

#include "imageinfowidget.hh"

class ThumbnailView : public QListView
{
    Q_OBJECT
    
public:
    explicit ThumbnailView(QWidget *parent = 0);
    ~ThumbnailView();

    void addThumbnail(QMap<QString, QString> imageinfo);

signals:
    void currentThumbnailChanged(QMap<QString, QString> imageInfo);

public slots:
    void sortOldestFirst();
    void sortOldestLast();
    void sortLastModifiedFirst();
    void sortLastModifiedLast();

protected:
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    QHash<QString, QMap<QString, QString> > m_imageInfoMap;
};

#endif // THUMBNAILVIEW_H
