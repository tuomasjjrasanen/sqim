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

#include "common.hh"

QDir cacheDir(const QString& filePath)
{
    return QDir(QDir::homePath()
                + "/.cache/sqim"
                + filePath);
}

bool makeCacheDir(const QString& filePath)
{
    QDir d = cacheDir(filePath);

    if (!d.exists()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        // Ensure the cache directory exists.
        if (!d.mkpath(".")) {
            qWarning() << "failed to create the cache directory";
            return false;
        }
        locker.unlock();
    }
    return true;
}

QString fileSizeToString(const qint64 bytes)
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

QString imageSizeToString(const QSize& size)
{
    const int w = size.width();
    const int h = size.height();

    return QString("%1 x %2 (%3 megapixels)")
        .arg(w).arg(h).arg(w * h / 1000000.0, 0, 'f', 1);
}
