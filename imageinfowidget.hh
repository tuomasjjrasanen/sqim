#ifndef IMAGEINFOWIDGET_HH
#define IMAGEINFOWIDGET_HH

#include <QLabel>
#include <QMap>

class ImageInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageInfoWidget(QWidget *parent = 0);
    ~ImageInfoWidget();

public slots:
    void setImageInfo(QMap<QString, QString> imageInfo);

private:
    QLabel *m_filepathLabel;
    QLabel *m_timestampLabel;
    QLabel *m_modificationTimeLabel;

};

#endif
