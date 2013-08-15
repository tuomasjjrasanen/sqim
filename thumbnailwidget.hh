#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include <QWidget>
#include <QFutureWatcher>
#include <QImage>
#include <QListView>
#include <QStandardItemModel>

class ThumbnailWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ThumbnailWidget(QWidget *parent = 0);
    ~ThumbnailWidget();

    void loadImage(const QString &imagePath);

private:
    QListView *m_iconView;
    QStandardItemModel *m_iconModel;
};

#endif // THUMBNAILWIDGET_H
