#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include <QListView>
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
};

#endif // THUMBNAILWIDGET_H
