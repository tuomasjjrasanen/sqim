#ifndef IMAGEINFO_HH
#define IMAGEINFO_HH

#include <QSharedDataPointer>
#include <QString>

class ImageInfoData : public QSharedData
{
public:

    ImageInfoData() {}

    ImageInfoData(const ImageInfoData &other)
    :QSharedData(other)
    ,m_filepath(other.m_filepath)
    ,m_timestamp(other.m_timestamp) 
    ,m_thumbnailFilepath(other.m_thumbnailFilepath)
    ,m_modificationTime(other.m_modificationTime) {}

    ~ImageInfoData() {}
    
    QString m_filepath;
    QString m_timestamp;
    QString m_thumbnailFilepath;
    QString m_modificationTime;
};

class ImageInfo
{
public:
    ImageInfo() {
        m_imageInfoData = new ImageInfoData();
    }

    ImageInfo(const ImageInfo &other)
    :m_imageInfoData(other.m_imageInfoData) {}

    void setFilepath(QString filepath) {
        m_imageInfoData->m_filepath = filepath;
    }

    void setTimestamp(QString timestamp) {
        m_imageInfoData->m_timestamp = timestamp;
    }

    void setThumbnailFilepath(QString thumbnailFilepath) {
        m_imageInfoData->m_thumbnailFilepath = thumbnailFilepath;
    }

    void setModificationTime(QString modificationTime) {
        m_imageInfoData->m_modificationTime = modificationTime;
    }

    QString filepath() const {
        return m_imageInfoData->m_filepath;
    }

    QString timestamp() const {
        return m_imageInfoData->m_timestamp;
    }

    QString thumbnailFilepath() const {
        return m_imageInfoData->m_thumbnailFilepath;
    }

    QString modificationTime() const {
        return m_imageInfoData->m_modificationTime;
    }

    bool isValid() const {
        return !m_imageInfoData->m_filepath.isEmpty()
            && !m_imageInfoData->m_timestamp.isEmpty()
            && !m_imageInfoData->m_thumbnailFilepath.isEmpty()
            && !m_imageInfoData->m_modificationTime.isEmpty();
    }

private:
    QSharedDataPointer<ImageInfoData> m_imageInfoData;
};

#endif
