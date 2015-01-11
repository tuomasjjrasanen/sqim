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

#include <QIcon>
#include <QPainter>
#include <QPixmap>

#include "thumbnaildelegate.hh"

ThumbnailDelegate::ThumbnailDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

void ThumbnailDelegate::paint(QPainter *painter,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    QRect rect(option.rect);
    rect.setWidth(rect.width() - 3);
    rect.setHeight(rect.height() - 3);

    QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();

    painter->drawPixmap(rect, icon.pixmap(rect.size()));

    // Draw rects to create more distinctive visualization for item
    // selection and current item.
    painter->save();
    if (option.state.testFlag(QStyle::State_Selected)) {
        QPen pen(painter->pen());
        pen.setColor(Qt::white);
        if (option.state.testFlag(QStyle::State_HasFocus))
            pen.setColor(Qt::yellow);
        painter->setPen(pen);
        painter->drawRect(rect);
    }
    painter->restore();
}
