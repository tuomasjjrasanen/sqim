#ifndef IMAGEWIDGET_HH
#define IMAGEWIDGET_HH

#include <QLabel>
#include <QMap>
#include <QScrollArea>
#include <QString>
#include <QWheelEvent>
#include <QWidget>

class ImageWidget : public QScrollArea
{
    Q_OBJECT

public:
    explicit ImageWidget(QWidget *parent = 0);
    ~ImageWidget();

public slots:
    void setImage(QMap<QString, QString> imageInfo);
    void zoomIn();
    void zoomOut();
    void zoomBy(double zoomFactor);
    void zoomTo(double zoomLevel);

protected:
    void wheelEvent(QWheelEvent *event);

private:
    QLabel *m_imageLabel;
    double m_zoomLevel;

};

#endif
