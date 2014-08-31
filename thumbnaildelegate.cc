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

#include <QPainter>

#include "thumbnaildelegate.hh"

ThumbnailDelegate::ThumbnailDelegate(QAbstractItemView *view, QObject *parent)
    : QStyledItemDelegate(parent)
    , m_view(view)
{
}

void ThumbnailDelegate::paint(QPainter *painter,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    // Draw rects to create more distinctive visualization for item
    // selection and current item.
    painter->save();
    if (option.state.testFlag(QStyle::State_Selected)) {
        QPen pen(painter->pen());
        pen.setColor(Qt::white);
        if (m_view && m_view->currentIndex() == index)
            pen.setColor(Qt::yellow);
        painter->setPen(pen);
        QRect rect(option.rect);
        rect.setWidth(rect.width() - 3);
        rect.setHeight(rect.height() - 3);
        painter->drawRect(rect);
    }
    painter->restore();
}
