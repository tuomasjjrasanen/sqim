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

#ifndef METADATA_HH
#define METADATA_HH

#include <QHash>
#include <QTransform>

typedef QHash<QString, QVariant> Metadata;

bool parseMetadata(const QString& filePath, Metadata& metadata);
QTransform exifOrientation(const Metadata& metadata);

#endif // METADATA_HH
