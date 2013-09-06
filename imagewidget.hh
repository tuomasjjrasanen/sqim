#ifndef IMAGEWIDGET_HH
#define IMAGEWIDGET_HH

#include <QLabel>
#include <QMap>
#include <QString>
#include <QWidget>

class ImageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageWidget(QWidget *parent = 0);
    ~ImageWidget();

public slots:
    void setImage(QMap<QString, QString> imageInfo);

private:
    QLabel *m_imageLabel;
};

#endif
