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

#include <QVBoxLayout>

#include "thumbnaildelegate.hh"
#include "thumbnailwidget.hh"

ThumbnailWidget::ThumbnailWidget(QWidget* parent)
    :QWidget(parent)
    ,m_thumbnailView(new ThumbnailView(this))
    ,m_toolBar(new QToolBar(this))
    ,m_imageFilePaths()
{
    QLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_toolBar);
    layout->addWidget(m_thumbnailView);
    setLayout(layout);

    foreach (QAction* action, m_thumbnailView->actions()) {
        m_toolBar->addAction(action);
    }

    m_thumbnailView->setItemDelegate(
        new ThumbnailDelegate(m_thumbnailView, this));

    connect(m_thumbnailView, SIGNAL(currentThumbnailChanged(Metadata)),
            SIGNAL(currentThumbnailChanged(Metadata)));
    connect(m_thumbnailView, SIGNAL(currentThumbnailActivated(Metadata)),
            SIGNAL(currentThumbnailActivated(Metadata)));
}

ThumbnailWidget::~ThumbnailWidget()
{
}

bool ThumbnailWidget::addThumbnail(const Metadata metadata)
{
    QString filePath = metadata.value("filePath").toString();

    if (m_imageFilePaths.contains(filePath))
        return false;

    if (m_thumbnailView->addThumbnail(metadata)) {
        m_imageFilePaths << filePath;
        return true;
    }

    return false;
}

void ThumbnailWidget::clear()
{
    qobject_cast<QStandardItemModel*>(m_thumbnailView->model())->clear();
    m_imageFilePaths.clear();
}
