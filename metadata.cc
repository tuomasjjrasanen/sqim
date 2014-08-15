// sqim - simply qute image manager
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

#include <QDataStream>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>

#include <exiv2/exiv2.hpp>

#include "metadata.hh"
#include "common.hh"

static QString fileSizeToString(const qint64 bytes)
{
    static qreal KiB = 1024;
    static qreal MiB = KiB * KiB;
    static qreal GiB = MiB * KiB;
    if (bytes > GiB) {
        return QString::number(bytes / GiB, 'f', 1)
            + " GiB (" + QString::number(bytes) + " B)";
    }
    if (bytes > MiB) {
        return QString::number(bytes / MiB, 'f', 1)
            + " MiB (" + QString::number(bytes) + " B)";
    }
    if (bytes > KiB) {
        return QString::number(bytes / KiB, 'f', 1)
            + " KiB (" + QString::number(bytes) + " B)";
    }
    return QString::number(bytes) + " B";
}

static bool parseExivMetadata(const QString& filePath,
                              Metadata& metadata)
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
        metadata.insert("imageSize",
                        QString("%1 x %2 (%3 megapixels)")
                        .arg(w).arg(h).arg(w * h / 1000000.0, 0, 'f', 1));

        Exiv2::ExifData &exifData = image->exifData();
        if (exifData.empty()) {
            qWarning() << filePath << " does not have EXIF data";
            return false;
        }

        metadata.insert("timestamp",
                        QString::fromStdString(
                            exifData["Exif.Photo.DateTimeOriginal"]
                            .toString()));
    } catch (Exiv2::AnyError& e) {
        qWarning() << "failed to retrieve metadata from " 
                   << filePath << ": " << e.what();
        return false;
    }
    return true;
}

bool parseMetadata(const QString& filePath)
{
    QFileInfo imageFileInfo(filePath);
    QFileInfo metadataFileInfo(cacheDir(filePath), "meta.dat");
    Metadata metadata;

    bool result = parseExivMetadata(filePath, metadata);

    metadata.insert("filepath", filePath);
    metadata.insert("modificationTime",
                    imageFileInfo.lastModified()
                    .toString("yyyy-MM-ddThh:mm:ss"));
    metadata.insert("fileSize", fileSizeToString(imageFileInfo.size()));

    QFile metadataFile(metadataFileInfo.filePath());
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

    return result;
}

bool readMetadata(const QString& filePath, Metadata& metadata)
{
    QFileInfo imageFileInfo(filePath);
    QDir cacheDir(QDir::homePath()
                  + "/.cache/sqim"
                  + imageFileInfo.canonicalFilePath());
    QFileInfo metadataFileInfo(cacheDir, "meta.dat");

    if (metadataFileInfo.exists()
        && metadataFileInfo.lastModified() >= imageFileInfo.lastModified()) {
        QFile metadataFile(metadataFileInfo.filePath());
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
    return false;
}
