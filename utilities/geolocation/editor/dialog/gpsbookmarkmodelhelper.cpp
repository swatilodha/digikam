/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2009-11-21
 * @brief  Central object for managing bookmarks
 *
 * @author Copyright (C) 2009,2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "gpsbookmarkmodelhelper.h"

// Qt includes

#include <QStandardItemModel>
#include <QStandardPaths>

// KDE includes

#include <kactioncollection.h>
#include <kbookmarkmenu.h>
#include <kbookmarkmanager.h>
#include <klocalizedstring.h>

// local includes

#include "gpsbookmarkowner.h"
#include "gpsundocommand.h"
#include "gpsimagemodel.h"

namespace Digikam
{

class GPSBookmarkModelHelper::Private
{
public:

    Private()
      : model(0),
        bookmarkManager(0),
        kipiImageModel(0),
        visible(false)
    {
    }

    void addBookmarkGroupToModel(const KBookmarkGroup& group);

public:

    QStandardItemModel* model;
    KBookmarkManager*   bookmarkManager;
    GPSImageModel*     kipiImageModel;
    QPixmap             pixmap;
    QUrl                bookmarkIconUrl;
    bool                visible;
};

void GPSBookmarkModelHelper::Private::addBookmarkGroupToModel(const KBookmarkGroup& group)
{
    KBookmark currentBookmark = group.first();

    while (!currentBookmark.isNull())
    {
        if (currentBookmark.isGroup())
        {
            addBookmarkGroupToModel(currentBookmark.toGroup());
        }
        else
        {
            bool okay                                 = false;
            const KGeoMap::GeoCoordinates coordinates = KGeoMap::GeoCoordinates::fromGeoUrl(currentBookmark.url().url(), &okay);

            if (okay)
            {
                QStandardItem* const item = new QStandardItem();
                item->setData(currentBookmark.text(), Qt::DisplayRole);
                item->setData(QVariant::fromValue(coordinates), GPSBookmarkModelHelper::CoordinatesRole);
                model->appendRow(item);
            }
        }

        currentBookmark = group.next(currentBookmark);
    }
}

GPSBookmarkModelHelper::GPSBookmarkModelHelper(KBookmarkManager* const bookmarkManager, GPSImageModel* const kipiImageModel, QObject* const parent)
    : ModelHelper(parent),
      d(new Private())
{
    d->model           = new QStandardItemModel(this);
    d->bookmarkManager = bookmarkManager;
    d->kipiImageModel  = kipiImageModel;
    d->bookmarkIconUrl = QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                                    QStringLiteral("gpssync/bookmarks-marker.png")));
    d->pixmap          = QPixmap(d->bookmarkIconUrl.toLocalFile());

    connect(d->bookmarkManager, SIGNAL(bookmarksChanged(QString)),
            this, SLOT(slotUpdateBookmarksModel()));

    connect(d->bookmarkManager, SIGNAL(changed(QString,QString)),
            this, SLOT(slotUpdateBookmarksModel()));

    slotUpdateBookmarksModel();
}

GPSBookmarkModelHelper::~GPSBookmarkModelHelper()
{
    delete d;
}

QAbstractItemModel* GPSBookmarkModelHelper::model() const
{
    return d->model;
}

QItemSelectionModel* GPSBookmarkModelHelper::selectionModel() const
{
    return 0;
}

bool GPSBookmarkModelHelper::itemCoordinates(const QModelIndex& index, KGeoMap::GeoCoordinates* const coordinates) const
{
    const KGeoMap::GeoCoordinates itemCoordinates = index.data(CoordinatesRole).value<KGeoMap::GeoCoordinates>();

    if (coordinates)
    {
        *coordinates = itemCoordinates;
    }

    return itemCoordinates.hasCoordinates();
}

bool GPSBookmarkModelHelper::itemIcon(const QModelIndex& index, QPoint* const offset, QSize* const size, QPixmap* const pixmap, QUrl* const url) const
{
    Q_UNUSED(index)

    if (offset)
    {
        *offset = QPoint(d->pixmap.width()/2, d->pixmap.height()-1);
    }

    if (url)
    {
        *url = d->bookmarkIconUrl;

        if (size)
        {
            *size = d->pixmap.size();
        }
    }
    else
    {
        *pixmap = d->pixmap;
    }

    return true;
}

void GPSBookmarkModelHelper::slotUpdateBookmarksModel()
{
    d->model->clear();

    // iterate trough all bookmarks
    d->addBookmarkGroupToModel(d->bookmarkManager->root());
}

void GPSBookmarkModelHelper::setVisible(const bool state)
{
    d->visible = state;
    emit(signalVisibilityChanged());
}

KGeoMap::ModelHelper::Flags GPSBookmarkModelHelper::modelFlags() const
{
    return FlagSnaps|(d->visible?FlagVisible:FlagNull);
}

KGeoMap::ModelHelper::Flags GPSBookmarkModelHelper::itemFlags(const QModelIndex& /*index*/) const
{
    return FlagVisible|FlagSnaps;
}

void GPSBookmarkModelHelper::snapItemsTo(const QModelIndex& targetIndex, const QList<QModelIndex>& snappedIndices)
{
    GPSUndoCommand* const undoCommand = new GPSUndoCommand();
    KGeoMap::GeoCoordinates targetCoordinates;

    if (!itemCoordinates(targetIndex, &targetCoordinates))
        return;

    for (int i = 0; i < snappedIndices.count(); ++i)
    {
        const QPersistentModelIndex itemIndex = snappedIndices.at(i);
        GPSImageItem* const item             = d->kipiImageModel->itemFromIndex(itemIndex);

        GPSDataContainer newData;
        newData.setCoordinates(targetCoordinates);

        GPSUndoCommand::UndoInfo undoInfo(itemIndex);
        undoInfo.readOldDataFromItem(item);

        item->setGPSData(newData);
        undoInfo.readNewDataFromItem(item);

        //undoCommand->addUndoInfo(GPSUndoCommand::UndoInfo(itemIndex, oldData, newData, oldTagList, newTagList));
        undoCommand->addUndoInfo(undoInfo);
    }

    qCDebug(DIGIKAM_GENERAL_LOG)<<targetIndex.data(Qt::DisplayRole).toString();
    undoCommand->setText(i18np("1 image snapped to '%2'",
                               "%1 images snapped to '%2'", snappedIndices.count(), targetIndex.data(Qt::DisplayRole).toString()));

    emit(signalUndoCommand(undoCommand));
}

}  // namespace Digikam
