// SQIM - Simple Qt Image Manager
// Copyright (C) 2014 Tuomas Räsänen <tuomasjjrasanen@tjjr.fi>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.

// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <exiv2/exiv2.hpp>

#include "common.hh"
#include "metadata.hh"

static bool fillWithFileInfo(const QString& filePath, Metadata& metadata)
{
    QFileInfo fileInfo(filePath);

    metadata.insert("filePath", QVariant(filePath));
    metadata.insert("modificationTime", QVariant(fileInfo.lastModified().toUTC()));
    metadata.insert("fileSize", QVariant(fileInfo.size()));

    return true;
}

static bool fillWithImageInfo(const QString& filePath, Metadata& metadata)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    try {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(
            filePath.toStdString());
        if (image.get() == 0) {
            qCritical() << "failed to recognize " << filePath << " as an image";
            return false;
        }
        image->readMetadata();

        const int w = image->pixelWidth();
        const int h = image->pixelHeight();
        metadata.insert("imageSize", QVariant(QSize(w, h)));

        metadata.insert("timestamp", QDateTime::fromTime_t(0).toUTC());
        metadata.insert("orientation", 1);

        Exiv2::ExifData &exifData = image->exifData();
        if (exifData.empty()) {
            qWarning() << filePath << " does not have EXIF data";
            return true;
        }

        QString dateTimeString = QString::fromStdString(
            exifData["Exif.Image.DateTime"]
            .toString());
        dateTimeString.truncate(19);
        QDateTime dateTime = QDateTime::fromString(dateTimeString,
                                                   "yyyy:MM:dd HH:mm:ss");
        dateTime.setTimeSpec(Qt::UTC);
        if (dateTime.isValid())
            metadata.insert("timestamp", QVariant(dateTime));
        qlonglong orientation = qlonglong(
            exifData["Exif.Image.Orientation"].toLong());
        metadata.insert("orientation", QVariant(orientation));
    } catch (Exiv2::AnyError& e) {
        qWarning() << "failed to retrieve metadata from "
                   << filePath << ": " << e.what();
        return false;
    }
    return true;
}

Metadata getMetadata(const QString& filePath)
{
    Metadata metadata;

    if (!fillWithFileInfo(filePath, metadata)) {
        qCritical() << "failed to get file info from " << filePath;
        metadata.clear();
        return metadata;
    }

    if (!fillWithImageInfo(filePath, metadata)) {
        qCritical() << "failed to get image info from " << filePath;
        metadata.clear();
        return metadata;
    }

    return metadata;
}

QTransform exifTransform(const Metadata& metadata)
{
    if (metadata.contains("orientation"))
        return exifTransform(metadata.value("orientation").toInt());

    return QTransform();
}

QTransform exifTransform(int orientation)
{
    static const QTransform transforms[] = {
        QTransform(),
        QTransform(),
        QTransform().rotate(180),
        QTransform(),
        QTransform(),
        QTransform().rotate(90),
        QTransform(),
        QTransform().rotate(-90)
    };

    return transforms[orientation - 1];
}
