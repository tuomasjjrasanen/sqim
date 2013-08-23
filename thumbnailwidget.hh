#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include <QListView>
#include <QHash>
#include <QStandardItemModel>

enum {
    COL_THUMB_FILEPATH,
    COL_IMAGE_FILEPATH,
    COL_IMAGE_DATETIME,
    COLS
};

class ThumbnailWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ThumbnailWidget(QWidget *parent = 0);
    ~ThumbnailWidget();

    void addThumbnail(const QStringList columns);

public slots:
    void sortOlderFirst();
    void sortNewerFirst();

private:
    QListView *m_iconView;
    QStandardItemModel *m_iconModel;
    QHash<QString, QStandardItem*> m_itemMap;
};

#endif // THUMBNAILWIDGET_H
