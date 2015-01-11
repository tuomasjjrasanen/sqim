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

#include <QDateTime>
#include <QDebug>
#include <QMutexLocker>
#include <QVariant>
#include <QSize>

#include <exiv2/exiv2.hpp>

#include "common.hh"
#include "metadata.hh"

static bool isImageFile(const QString& filePath)
{
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(
        filePath.toStdString());

    return image.get() != 0;
}

static bool parseExif(const QString& filePath, Metadata& metadata)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    try {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(
            filePath.toStdString());
        if (image.get() == 0) {
            qWarning() << filePath << " is not recognized as a valid image file";
            return false;
        }
        image->readMetadata();

        const int w = image->pixelWidth();
        const int h = image->pixelHeight();
        metadata.insert("imageSize", QVariant(QSize(w, h)));

        metadata.insert("timestamp", QDateTime());
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

Metadata getMetadata(const QString& imageFilePath)
{
    Metadata metadata;

    QFileInfo imageFileInfo(imageFilePath);

    metadata.insert("filePath", QVariant(imageFilePath));
    metadata.insert("modificationTime", QVariant(imageFileInfo.lastModified()));
    metadata.insert("fileSize", QVariant(imageFileInfo.size()));

    if (!parseExif(imageFilePath, metadata)) {
        qWarning() << "failed to parse Exif metadata";
    }

    return metadata;
}

QTransform exifTransform(const Metadata& metadata)
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

    if (metadata.contains("orientation"))
        return transforms[metadata.value("orientation").toInt() - 1];

    return QTransform();
}
