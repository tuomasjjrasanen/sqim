#ifndef IMAGE_HH
#define IMAGE_HH

#include <QSharedDataPointer>
#include <QString>

class ImageData : public QSharedData
{
public:

    ImageData() {}

    ImageData(const ImageData &other)
    :QSharedData(other)
    ,m_filepath(other.m_filepath)
    ,m_timestamp(other.m_timestamp) 
    ,m_thumbnail(other.m_thumbnail)
    ,m_modificationTime(other.m_modificationTime) {}

    ~ImageData() {}
    
    QString m_filepath;
    QString m_timestamp;
    QImage m_thumbnail;
    QString m_modificationTime;
};

class Image
{
public:
    Image() {
        m_imageData = new ImageData();
    }

    Image(const Image &other)
    :m_imageData(other.m_imageData) {}

    void setFilepath(QString filepath) {
        m_imageData->m_filepath = filepath;
    }

    void setTimestamp(QString timestamp) {
        m_imageData->m_timestamp = timestamp;
    }

    void setThumbnail(QImage thumbnail) {
        m_imageData->m_thumbnail = thumbnail;
    }

    void setModificationTime(QString modificationTime) {
        m_imageData->m_modificationTime = modificationTime;
    }

    QString filepath() const {
        return m_imageData->m_filepath;
    }

    QString timestamp() const {
        return m_imageData->m_timestamp;
    }

    QImage thumbnail() const {
        return m_imageData->m_thumbnail;
    }

    QString modificationTime() const {
        return m_imageData->m_modificationTime;
    }

    bool isValid() const {
        return !m_imageData->m_filepath.isEmpty()
            && !m_imageData->m_timestamp.isEmpty()
            && !m_imageData->m_thumbnail.isNull()
            && !m_imageData->m_modificationTime.isEmpty();
    }

private:
    QSharedDataPointer<ImageData> m_imageData;
};

#endif
