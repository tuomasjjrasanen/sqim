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
#include "metadatawidget.hh"

MetadataWidget::MetadataWidget(QWidget *parent)
    :QScrollArea(parent)
    ,m_filePathLabel(new QLabel(this))
    ,m_timestampLabel(new QLabel(this))
    ,m_modificationTimeLabel(new QLabel(this))
    ,m_fileSizeLabel(new QLabel(this))
    ,m_imageSizeLabel(new QLabel(this))
    ,m_tagModel(new QSqlQueryModel(this))
    ,m_tagView(new QListView(this))
{
    m_filePathLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_timestampLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_modificationTimeLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_fileSizeLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_imageSizeLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_tagView->setModel(m_tagModel);
    m_tagView->setViewMode(QListView::IconMode);
    m_tagView->setFrameShape(QFrame::NoFrame);
    m_tagView->setSelectionMode(QAbstractItemView::NoSelection);
    m_tagView->setFocusPolicy(Qt::NoFocus);
    connect(m_tagView, SIGNAL(clicked(const QModelIndex&)),
            SLOT(removeTag(const QModelIndex&)));

    QWidget* widget = new QWidget(this);
    QFormLayout *layout = new QFormLayout(widget);
    layout->addRow("File path", m_filePathLabel);
    layout->addRow("Shot timestamp", m_timestampLabel);
    layout->addRow("Modification timestamp", m_modificationTimeLabel);
    layout->addRow("File size", m_fileSizeLabel);
    layout->addRow("Pixel dimensions", m_imageSizeLabel);
    layout->addRow("Tags", m_tagView);
    widget->setLayout(layout);
    setWidget(widget);
    setWidgetResizable(true);
}

MetadataWidget::~MetadataWidget()
{
}

void MetadataWidget::updateTags()
{
    QSqlQuery query;
    query.prepare("SELECT tag FROM Tagging "
                  "WHERE Tagging.file_path = ? "
                  "ORDER BY tag");
    query.addBindValue(m_filePathLabel->text());
    query.exec();
    m_tagModel->setQuery(query);
}

void MetadataWidget::setMetadata(const QModelIndex& index)
{
    if (!index.isValid()) {
        m_filePathLabel->clear();
        m_timestampLabel->clear();
        m_modificationTimeLabel->clear();
        m_fileSizeLabel->clear();
        m_imageSizeLabel->clear();
        updateTags();
        return;
    }

    m_filePathLabel->setText(
        index.sibling(index.row(), 1).data().toString());
    m_timestampLabel->setText(
        index.sibling(index.row(), 6).data().toDateTime().toString(Qt::ISODate));
    m_modificationTimeLabel->setText(
        index.sibling(index.row(), 3).data().toDateTime().toString(Qt::ISODate));
    m_fileSizeLabel->setText(
        fileSizeToString(index.sibling(index.row(), 2).data().toULongLong()));
    int w = index.sibling(index.row(), 4).data().toInt();
    int h = index.sibling(index.row(), 5).data().toInt();
    m_imageSizeLabel->setText(
        imageSizeToString(QSize(w, h)));
    updateTags();
}

void MetadataWidget::removeTag(const QModelIndex &index)
{
    QSqlQuery query;

    query.prepare("DELETE FROM Tagging "
                  "WHERE file_path == ? AND tag == ?");
    query.addBindValue(m_filePathLabel->text());
    query.addBindValue(m_tagModel->data(index));
    query.exec();
    updateTags();
}
