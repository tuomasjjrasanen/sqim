#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWidget>
#include <QFutureWatcher>
#include <QImage>
#include <QListView>
#include <QStandardItemModel>

class ImageView : public QWidget
{
    Q_OBJECT
    
public:
    explicit ImageView(QWidget *parent = 0);
    ~ImageView();
    void loadImages(QStringList const &imagePaths);

private slots:
    void loadIcon(int i);

private:
    QFutureWatcher<QString> *m_iconCreator;
    QListView *m_iconView;
    QStandardItemModel *m_iconModel;
};

#endif // IMAGEVIEW_H
