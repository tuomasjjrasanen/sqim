#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWidget>
#include <QFutureWatcher>
#include <QImage>
#include <QListView>
#include <QStandardItemModel>
#include <QProgressBar>

class ImageView : public QWidget
{
    Q_OBJECT
    
public:
    explicit ImageView(QWidget *parent = 0);
    ~ImageView();
    void loadImages(QString dir);

private slots:
    void showImage(int i);
    void findFinished();
    void loadFinished();

private:
    QFutureWatcher<QStringList> *m_imageFinder;
    QFutureWatcher<QImage> *m_imageLoader;
    QListView *m_imageView;
    QStandardItemModel *m_imageModel;
    QProgressBar *m_progressBar;

};

#endif // IMAGEVIEW_H
