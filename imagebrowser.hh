#ifndef IMAGEBROWSER_H
#define IMAGEBROWSER_H

#include <QListView>
#include <QHash>
#include <QStandardItemModel>

enum {
    COL_THUMB_FILEPATH,
    COL_IMAGE_FILEPATH,
    COL_IMAGE_DATETIME,
    COLS
};

class ImageBrowser : public QWidget
{
    Q_OBJECT
    
public:
    explicit ImageBrowser(QWidget *parent = 0);
    ~ImageBrowser();

    void addImage(const QStringList columns);

public slots:
    void sortOlderFirst();
    void sortNewerFirst();

private:
    QListView *m_iconView;
    QStandardItemModel *m_iconModel;
    QHash<QString, QStandardItem*> m_itemMap;
};

#endif // IMAGEBROWSER_H
