#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include <QListView>
#include <QHash>
#include <QStandardItemModel>

class ThumbnailWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ThumbnailWidget(QWidget *parent = 0);
    ~ThumbnailWidget();

    void addThumbnail(QString thumbnailFilePath);

private:
    QListView *m_iconView;
    QStandardItemModel *m_iconModel;
    QHash<QString, QStandardItem*> m_itemMap;
};

#endif // THUMBNAILWIDGET_H
