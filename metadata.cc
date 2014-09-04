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

static const int METADATA_VERSION = 1;

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

        Exiv2::ExifData &exifData = image->exifData();
        if (exifData.empty()) {
            qWarning() << filePath << " does not have EXIF data";
            return true;
        }

        QString dateTimeString = QString::fromStdString(
            exifData["Exif.Image.DateTime"]
            .toString());
        dateTimeString.truncate(19);
        qWarning() << dateTimeString;
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

static bool writeMetadata(const QString& metadataFilePath,
                          const Metadata& metadata)
{
    QFile metadataFile(metadataFilePath);
    if (!metadataFile.open(QIODevice::WriteOnly)) {
        qWarning() << "failed to open metadata file for writing";
        return false;
    }

    QDataStream out(&metadataFile);
    out << metadata;
    if (out.status() != QDataStream::Ok) {
        qWarning() << "failed to write to the metadata file";
        return false;
    }

    return true;
}

static bool readMetadata(const QString& metadataFilePath, Metadata& metadata)
{
    QFile metadataFile(metadataFilePath);
    if (!metadataFile.open(QIODevice::ReadOnly)) {
        qWarning() << "failed to open metadata file for reading";
        return false;
    }

    QDataStream in(&metadataFile);
    in >> metadata;
    if (in.status() != QDataStream::Ok) {
        qWarning() << "failed to read metadata file";
        return false;
    }

    return true;
}

Metadata getMetadata(const QString& imageFilePath)
{
    Metadata metadata;

    QFileInfo imageFileInfo(imageFilePath);
    QFileInfo metadataFileInfo(cacheDir(imageFilePath), "meta.dat");

    if (metadataFileInfo.exists()
        && metadataFileInfo.lastModified() >= imageFileInfo.lastModified()) {
        if (readMetadata(metadataFileInfo.filePath(), metadata)
            && metadata.contains("_version")
            && metadata.value("_version").toInt() == METADATA_VERSION) {
            return metadata;
        }
        qWarning() << "failed to read cached metadata";
        metadata.clear();
    }

    metadata.insert("_version", QVariant(METADATA_VERSION));
    metadata.insert("filePath", QVariant(imageFilePath));
    metadata.insert("modificationTime", QVariant(imageFileInfo.lastModified()));
    metadata.insert("fileSize", QVariant(imageFileInfo.size()));

    if (!parseExif(imageFilePath, metadata)) {
        qCritical() << "failed to parse Exif metadata";
        metadata.clear();
        return metadata;
    }

    if (!writeMetadata(metadataFileInfo.filePath(), metadata)) {
        qCritical() << "failed to write cached metadata";
        metadata.clear();
        return metadata;
    }

    return metadata;
}

QTransform exifOrientation(const Metadata& metadata)
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
