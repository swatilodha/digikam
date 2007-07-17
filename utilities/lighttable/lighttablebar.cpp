/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-04-11
 * Description : light table thumbs bar
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes.

#include <Q3ValueList>
#include <QPixmap>
#include <QPainter>
#include <QImage>
#include <QCursor>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QPaintEvent>

// KDE includes.

#include <klocale.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kstandarddirs.h>

// Local includes.

#include "ddebug.h"
#include "album.h"
#include "albumdb.h"
#include "albummanager.h"
#include "albumsettings.h"
#include "dragobjects.h"
#include "imageattributeswatch.h"
#include "metadatahub.h"
#include "ratingpopupmenu.h"
#include "dpopupmenu.h"
#include "themeengine.h"
#include "lighttablebar.h"
#include "lighttablebar.moc"

namespace Digikam
{

class LightTableBarPriv
{

public:

    LightTableBarPriv()
    {
        navigateByPair = false;
        toolTip        = 0;
    }

    bool                  navigateByPair;

    QPixmap               ratingPixmap;

    LightTableBarToolTip *toolTip;
};

class LightTableBarItemPriv
{

public:

    LightTableBarItemPriv()
    {
        onLeftPanel  = false;
        onRightPanel = false;
    }

    bool       onLeftPanel;
    bool       onRightPanel;

    ImageInfo  info;
};

LightTableBar::LightTableBar(QWidget* parent, int orientation, bool exifRotate)
             : ThumbBarView(parent, orientation, exifRotate)
{
    d = new LightTableBarPriv;
    setMouseTracking(true);
    readToolTipSettings();
    d->toolTip = new LightTableBarToolTip(this);

    connect(ThemeEngine::componentData(), SIGNAL(signalThemeChanged()),
            this, SLOT(slotUpdate()));

    connect(this, SIGNAL(signalItemSelected(ThumbBarItem*)),
            this, SLOT(slotItemSelected(ThumbBarItem*)));

    // -- Load rating Pixmap ------------------------------------------

    d->ratingPixmap = QPixmap(KStandardDirs::locate("data", "digikam/data/rating.png"));

    QPainter painter(&d->ratingPixmap);
    painter.fillRect(0, 0, d->ratingPixmap.width(), d->ratingPixmap.height(),
                     ThemeEngine::componentData()->textSpecialRegColor());
    painter.end();

    if (orientation == Qt::Vertical)
        setMinimumWidth(d->ratingPixmap.width()*5 + 6 + 2*getMargin());
    else
        setMinimumHeight(d->ratingPixmap.width()*5 + 6 + 2*getMargin());

    // ----------------------------------------------------------------

    ImageAttributesWatch *watch = ImageAttributesWatch::componentData();

    connect(watch, SIGNAL(signalImageRatingChanged(qlonglong)),
            this, SLOT(slotImageRatingChanged(qlonglong)));
}

LightTableBar::~LightTableBar()
{
    delete d->toolTip;
    delete d;
}

void LightTableBar::setNavigateByPair(bool b)
{
    d->navigateByPair = b;
}

void LightTableBar::slotImageRatingChanged(qlonglong imageId)
{
    for (ThumbBarItem *item = firstItem(); item; item = item->next())
    {
        LightTableBarItem *ltItem = dynamic_cast<LightTableBarItem*>(item);
        if (ltItem->info().id() == imageId)
        {
            triggerUpdate();
            return;
        }
    }
}

void LightTableBar::contentsMouseReleaseEvent(QMouseEvent *e)
{
    if (!e) return;

    ThumbBarView::contentsMouseReleaseEvent(e);

    QPoint pos = QCursor::pos();
    LightTableBarItem *item = findItemByPos(e->pos());
    if (!item) return;

    RatingPopupMenu *ratingMenu = 0;

    if (e->button() == Qt::RightButton)
    {
        DPopupMenu popmenu(this);
        popmenu.insertItem(SmallIcon("previous"), i18n("Show on left panel"), 10);
        popmenu.insertItem(SmallIcon("next"), i18n("Show on right panel"), 11);
        popmenu.insertItem(SmallIcon("editimage"), i18n("Edit"), 12);

        if (d->navigateByPair)
        {
            popmenu.setItemEnabled(10, false);
            popmenu.setItemEnabled(11, false);
        }

        popmenu.insertSeparator();
        popmenu.insertItem(SmallIcon("fileclose"), i18n("Remove"), 13);
        popmenu.insertItem(SmallIcon("editshred"), i18n("Clear all"), 14);
        popmenu.insertSeparator();

        // Assign Star Rating -------------------------------------------

        ratingMenu = new RatingPopupMenu();

        connect(ratingMenu, SIGNAL(activated(int)),
                this, SLOT(slotAssignRating(int)));

        popmenu.insertItem(i18n("Assign Rating"), ratingMenu);

        switch(popmenu.exec(pos))
        {
            case 10:    // Left panel
            {
                emit signalSetItemOnLeftPanel(item->info());
                break;
            }
            case 11:    // Right panel
            {
                emit signalSetItemOnRightPanel(item->info());
                break;
            }
            case 12:    // Edit
            {
                emit signalEditItem(item->info());
                break;
            }
            case 13:    // Remove
            {
                emit signalRemoveItem(item->info());
                break;
            }
            case 14:    // Clear All
            {
                emit signalClearAll();
                break;
            }
            default:
                break;
        }
    }

    delete ratingMenu;
}

void LightTableBar::slotAssignRating(int rating)
{
    rating = qMin(5, qMax(0, rating));
    ImageInfo info = currentItemImageInfo();
    if (!info.isNull())
    {
        MetadataHub hub;
        hub.load(info);
        hub.setRating(rating);
        hub.write(info, MetadataHub::PartialWrite);
        hub.write(info.filePath(), MetadataHub::FullWriteIfChanged);
    }
}

void LightTableBar::slotAssignRatingNoStar()
{
    slotAssignRating(0);
}

void LightTableBar::slotAssignRatingOneStar()
{
    slotAssignRating(1);
}

void LightTableBar::slotAssignRatingTwoStar()
{
    slotAssignRating(2);
}

void LightTableBar::slotAssignRatingThreeStar()
{
    slotAssignRating(3);
}

void LightTableBar::slotAssignRatingFourStar()
{
    slotAssignRating(4);
}

void LightTableBar::slotAssignRatingFiveStar()
{
    slotAssignRating(5);
}

void LightTableBar::setOnLeftPanel(const ImageInfo &info)
{
    for (ThumbBarItem *item = firstItem(); item; item = item->next())
    {
        LightTableBarItem *ltItem = dynamic_cast<LightTableBarItem*>(item);
        if (ltItem)
        {
            if (!info.isNull())
            {
                if (ltItem->info() == info)
                {
                    ltItem->setOnLeftPanel(true);
                    repaintItem(item);
                }
                else if (ltItem->isOnLeftPanel() == true)
                {
                    ltItem->setOnLeftPanel(false);
                    repaintItem(item);
                }
            }
            else if (ltItem->isOnLeftPanel() == true)
            {
                ltItem->setOnLeftPanel(false);
                repaintItem(item);
            }
        }
    }
}

void LightTableBar::setOnRightPanel(const ImageInfo &info)
{
    for (ThumbBarItem *item = firstItem(); item; item = item->next())
    {
        LightTableBarItem *ltItem = dynamic_cast<LightTableBarItem*>(item);
        if (ltItem)
        {
            if (!info.isNull())
            {
                if (ltItem->info() == info)
                {
                    ltItem->setOnRightPanel(true);
                    repaintItem(item);
                }
                else if (ltItem->isOnRightPanel() == true)
                {
                    ltItem->setOnRightPanel(false);
                    repaintItem(item);
                }
            }
            else if (ltItem->isOnRightPanel() == true)
            {
                ltItem->setOnRightPanel(false);
                repaintItem(item);
            }
        }
    }
}

void LightTableBar::slotItemSelected(ThumbBarItem* item)
{
    if (item)
    {
        LightTableBarItem *ltItem = dynamic_cast<LightTableBarItem*>(item);
        if (ltItem)
        {
            emit signalLightTableBarItemSelected(ltItem->info());
            return;
        }
    }

    emit signalLightTableBarItemSelected(ImageInfo());
}

ImageInfo LightTableBar::currentItemImageInfo() const
{
    if (currentItem())
    {
        LightTableBarItem *item = dynamic_cast<LightTableBarItem*>(currentItem());
        return item->info();
    }

    return ImageInfo();
}

ImageInfoList LightTableBar::itemsImageInfoList()
{
    ImageInfoList list;

    for (ThumbBarItem *item = firstItem(); item; item = item->next())
    {
        LightTableBarItem *ltItem = dynamic_cast<LightTableBarItem*>(item);
        if (ltItem) 
        {
            list << ltItem->info();
        }
    }

    return list;
}

void LightTableBar::setSelectedItem(LightTableBarItem* ltItem)
{
    ThumbBarItem *item = static_cast<ThumbBarItem*>(ltItem);
    if (item) ThumbBarView::setSelected(item);
}

void LightTableBar::removeItem(const ImageInfo &info)
{
    if (info.isNull()) return;

    LightTableBarItem* ltItem = findItemByInfo(info);
    ThumbBarItem *item        = static_cast<ThumbBarItem*>(ltItem);  
    if (item) ThumbBarView::removeItem(item);
}

LightTableBarItem* LightTableBar::findItemByInfo(const ImageInfo &info) const
{
    if (!info.isNull())
    {
        for (ThumbBarItem *item = firstItem(); item; item = item->next())
        {
            LightTableBarItem *ltItem = dynamic_cast<LightTableBarItem*>(item);
            if (ltItem)
            {
                if (ltItem->info() == info)
                    return ltItem;
            }
        }
    }
    return 0;
}

LightTableBarItem* LightTableBar::findItemByPos(const QPoint& pos) const
{
    ThumbBarItem *item = findItem(pos);
    if (item)
    {
        LightTableBarItem *ltItem = dynamic_cast<LightTableBarItem*>(item);
        return ltItem;
    }

    return 0;
}

void LightTableBar::readToolTipSettings()
{
    AlbumSettings* albumSettings = AlbumSettings::componentData();
    if (!albumSettings) return;

    Digikam::ThumbBarToolTipSettings settings;
    settings.showToolTips   = albumSettings->getShowToolTips();
    settings.showFileName   = albumSettings->getToolTipsShowFileName();
    settings.showFileDate   = albumSettings->getToolTipsShowFileDate();
    settings.showFileSize   = albumSettings->getToolTipsShowFileSize();
    settings.showImageType  = albumSettings->getToolTipsShowImageType();
    settings.showImageDim   = albumSettings->getToolTipsShowImageDim();
    settings.showPhotoMake  = albumSettings->getToolTipsShowPhotoMake();
    settings.showPhotoDate  = albumSettings->getToolTipsShowPhotoDate();
    settings.showPhotoFocal = albumSettings->getToolTipsShowPhotoFocal();
    settings.showPhotoExpo  = albumSettings->getToolTipsShowPhotoExpo();
    settings.showPhotoMode  = albumSettings->getToolTipsShowPhotoMode();
    settings.showPhotoFlash = albumSettings->getToolTipsShowPhotoFlash();
    settings.showPhotoWB    = albumSettings->getToolTipsShowPhotoWB();
    setToolTipSettings(settings);
}

void LightTableBar::viewportPaintEvent(QPaintEvent* e)
{
    ThemeEngine* te = ThemeEngine::componentData();
    QRect    er(e->rect());
    QPixmap  bgPix;
    QPainter p2(&bgPix);

    if (countItems() > 0)
    {
        int cy=0, cx=0, ts=0, y1=0, y2=0, x1=0, x2=0;
        QPixmap  tile;
        QPainter p(&tile);

        if (getOrientation() == Qt::Vertical)
        {
            cy = viewportToContents(er.topLeft()).y();

            bgPix = QPixmap(contentsRect().width(), er.height());

            ts = getTileSize() + 2*getMargin();
            tile = QPixmap(visibleWidth(), ts);

            y1 = (cy/ts)*ts;
            y2 = ((y1 + er.height())/ts +1)*ts;
        }
        else
        {
            cx = viewportToContents(er.topLeft()).x();

            bgPix = QPixmap(er.width(), contentsRect().height());

            ts   = getTileSize() + 2*getMargin();
            tile = QPixmap(ts, visibleHeight());

            x1 = (cx/ts)*ts;
            x2 = ((x1 + er.width())/ts +1)*ts;
        }

        bgPix.fill(te->baseColor());

        for (ThumbBarItem *item = firstItem(); item; item = item->next())
        {
            if (getOrientation() == Qt::Vertical)
            {
                if (y1 <= item->position() && item->position() <= y2)
                {
                    if (item == currentItem())
                        tile = te->thumbSelPixmap(tile.width(), tile.height());
                    else
                        tile = te->thumbRegPixmap(tile.width(), tile.height());

                    if (item == currentItem())
                    {
                        p.setPen(QPen(te->textSelColor(), 3));
                        p.drawRect(2, 2, tile.width()-2, tile.height()-2);
                    }
                    else
                    {
                        p.setPen(QPen(te->textRegColor(), 1));
                        p.drawRect(0, 0, tile.width(), tile.height());
                    }

                    if (!item->pixmap().isNull())
                    {
                        QPixmap pix; 
                        pix.fromImage(QImage(item->pixmap().toImage()).
                                             scaled(getTileSize(), getTileSize(), Qt::KeepAspectRatio));
                        int x = (tile.width()  - pix.width())/2;
                        int y = (tile.height() - pix.height())/2;

                        p.drawPixmap(x, y, pix);

                        LightTableBarItem *ltItem = dynamic_cast<LightTableBarItem*>(item);

                        if (ltItem->isOnLeftPanel())
                        {
                            QPixmap lPix = SmallIcon("previous"); 
                            p.drawPixmap(getMargin(), getMargin(), lPix);
                        }
                        if (ltItem->isOnRightPanel())
                        {
                            QPixmap rPix = SmallIcon("next"); 
                            p.drawPixmap(tile.width() - getMargin() - rPix.width(), getMargin(), rPix);
                        }

                        QRect r(0, tile.height()-getMargin()-d->ratingPixmap.height(), 
                                tile.width(), d->ratingPixmap.height());
                        int rating = ltItem->info().rating();
                        int xr     = (r.width() - rating * d->ratingPixmap.width())/2;
                        int wr     = rating * d->ratingPixmap.width();
                        p.drawTiledPixmap(xr, r.y(), wr, r.height(), d->ratingPixmap);
                    }

                    p2.drawPixmap(0, item->position() - cy, tile);
                }
            }
            else
            {
                if (x1 <= item->position() && item->position() <= x2)
                {
                    if (item == currentItem())
                        tile = te->thumbSelPixmap(tile.width(), tile.height());
                    else
                        tile = te->thumbRegPixmap(tile.width(), tile.height());

                    if (item == currentItem())
                    {
                        p.setPen(QPen(te->textSelColor(), 2));
                        p.drawRect(1, 1, tile.width()-1, tile.height()-1);
                    }
                    else
                    {
                        p.setPen(QPen(te->textRegColor(), 1));
                        p.drawRect(0, 0, tile.width(), tile.height());
                    }

                    if (!item->pixmap().isNull())
                    {
                        QPixmap pix; 
                        pix.fromImage(QImage(item->pixmap().toImage()).
                                      scaled(getTileSize(), getTileSize(),
                                      Qt::KeepAspectRatio));
                        int x = (tile.width() - pix.width())/2;
                        int y = (tile.height()- pix.height())/2;
                        p.drawPixmap(x, y, pix);

                        LightTableBarItem *ltItem = dynamic_cast<LightTableBarItem*>(item);

                        if (ltItem->isOnLeftPanel())
                        {
                            QPixmap lPix = SmallIcon("previous"); 
                            p.drawPixmap(getMargin(), getMargin(), lPix);
                        }
                        if (ltItem->isOnRightPanel())
                        {
                            QPixmap rPix = SmallIcon("next"); 
                            p.drawPixmap(tile.width() - getMargin() - rPix.width(), getMargin(), rPix);
                        }

                        QRect r(0, tile.height()-getMargin()-d->ratingPixmap.height(), 
                                tile.width(), d->ratingPixmap.height());
                        int rating = ltItem->info().rating();
                        int xr     = (r.width() - rating * d->ratingPixmap.width())/2;
                        int wr     = rating * d->ratingPixmap.width();
                        p.drawTiledPixmap(xr, r.y(), wr, r.height(), d->ratingPixmap);
                    }

                    p2.drawPixmap(item->position() - cx, 0, tile);
                }
            }
        }

        p.end();

        QPainter p3(viewport());

        if (getOrientation() == Qt::Vertical)
            p3.drawPixmap(0, er.y(), bgPix);
        else
            p3.drawPixmap(er.x(), 0, bgPix);

        p3.end();
    }
    else
    {
        bgPix = QPixmap(contentsRect().width(), contentsRect().height());
        bgPix.fill(te->baseColor());
        p2.setPen(QPen(te->textRegColor()));
        p2.drawText(0, 0, bgPix.width(), bgPix.height(),
                    Qt::AlignCenter|Qt::TextWordWrap, 
                    i18n("Drag and drop images here"));
        QPainter p3(viewport());
        p3.drawPixmap(0, 0, bgPix);
        p3.end();
    }

    p2.end();
}

void LightTableBar::startDrag()
{
    if (!currentItem()) return;

    KUrl::List       urls;
    KUrl::List       kioURLs;
    Q3ValueList<int> albumIDs;
    Q3ValueList<int> imageIDs;

    LightTableBarItem *item = dynamic_cast<LightTableBarItem*>(currentItem());

    urls.append(item->info().fileUrl());
    kioURLs.append(item->info().databaseUrl());
    imageIDs.append(item->info().id());
    albumIDs.append(item->info().albumId());

    QPixmap icon(DesktopIcon("image", 48));
    int w = icon.width();
    int h = icon.height();

    QPixmap pix(w+4,h+4);
    QPainter p(&pix);
    p.fillRect(0, 0, w+4, h+4, QColor(Qt::white));
    p.setPen(QPen(Qt::black, 1));
    p.drawRect(0, 0, w+4, h+4);
    p.drawPixmap(2, 2, icon);
    p.end();

    Q3DragObject* drag = 0;

    drag = new ItemDrag(urls, kioURLs, albumIDs, imageIDs, this);
    if (drag)
    {
        drag->setPixmap(pix);
        drag->drag();
    }
}

void LightTableBar::contentsDragMoveEvent(QDragMoveEvent *e)
{
    int              albumID;
    Q3ValueList<int> albumIDs;
    Q3ValueList<int> imageIDs;
    KUrl::List       urls;
    KUrl::List       kioURLs;

    if (ItemDrag::decode(e, urls, kioURLs, albumIDs, imageIDs) ||
        AlbumDrag::decode(e, urls, albumID) ||
        TagDrag::canDecode(e))
    {
        e->accept();
        return;
    }

    e->ignore();
}

void LightTableBar::contentsDropEvent(QDropEvent *e)
{
    int              albumID;
    Q3ValueList<int> albumIDs;
    Q3ValueList<int> imageIDs;
    KUrl::List       urls;
    KUrl::List       kioURLs;

    if (ItemDrag::decode(e, urls, kioURLs, albumIDs, imageIDs))
    {
        ImageInfoList imageInfoList;

        for (Q3ValueList<int>::const_iterator it = imageIDs.begin();
             it != imageIDs.end(); ++it)
        {
            ImageInfo info(*it);
            if (!findItemByInfo(info))
            {
                imageInfoList.append(info);
            }
        }

        emit signalDroppedItems(imageInfoList);
        e->accept();
    }
    else if (AlbumDrag::decode(e, urls, albumID))
    {
        Q3ValueList<qlonglong> itemIDs = DatabaseAccess().db()->getItemIDsInAlbum(albumID);
        ImageInfoList imageInfoList;

        for (Q3ValueList<qlonglong>::const_iterator it = itemIDs.begin();
             it != itemIDs.end(); ++it)
        {
            ImageInfo info(*it);
            if (!findItemByInfo(info))
            {
                imageInfoList.append(info);
            }
        }

        emit signalDroppedItems(imageInfoList);
        e->accept();
    }
    else if(TagDrag::canDecode(e))
    {
        QByteArray  ba = e->encodedData("digikam/tag-id");
        QDataStream ds(ba);
        int tagID;
        ds >> tagID;

        Q3ValueList<qlonglong> itemIDs = DatabaseAccess().db()->getItemIDsInTag(tagID, true);
        ImageInfoList imageInfoList;

        for (Q3ValueList<qlonglong>::const_iterator it = itemIDs.begin();
             it != itemIDs.end(); ++it)
        {
            ImageInfo info(*it);
            if (!findItemByInfo(info))
            {
                imageInfoList.append(info);
            }
        }

        emit signalDroppedItems(imageInfoList);
        e->accept();
    }
    else 
    {
        e->ignore();
    }
}

// -------------------------------------------------------------------------

LightTableBarItem::LightTableBarItem(LightTableBar *view, const ImageInfo &info)
                 : ThumbBarItem(view, info.fileUrl())
{
    d = new LightTableBarItemPriv;
    d->info = info;
}

LightTableBarItem::~LightTableBarItem()
{
    delete d;
}

ImageInfo LightTableBarItem::info()
{
    return d->info;
}

void LightTableBarItem::setOnLeftPanel(bool on)
{
    d->onLeftPanel = on;
}

void LightTableBarItem::setOnRightPanel(bool on)
{
    d->onRightPanel = on;
}

bool LightTableBarItem::isOnLeftPanel() const
{
    return d->onLeftPanel;
}

bool LightTableBarItem::isOnRightPanel() const
{
    return d->onRightPanel;
}

// -------------------------------------------------------------------------

LightTableBarToolTip::LightTableBarToolTip(ThumbBarView* parent)
                    : ThumbBarToolTip(parent)
{
}

QString LightTableBarToolTip::tipContentExtraData(ThumbBarItem* item)
{
    QString tip, str;
    AlbumSettings* settings = AlbumSettings::componentData();
    ImageInfo info          = static_cast<LightTableBarItem *>(item)->info();

    if (settings)
    {
        if (settings->getToolTipsShowAlbumName() ||
            settings->getToolTipsShowComments()  ||
            settings->getToolTipsShowTags()      ||
            settings->getToolTipsShowRating())
        {
            tip += m_headBeg + i18n("digiKam Properties") + m_headEnd;

            if (settings->getToolTipsShowAlbumName())
            {
                PAlbum* album = AlbumManager::componentData()->findPAlbum(info.albumId());
                if (album)
                    tip += m_cellSpecBeg + i18n("Album:") + m_cellSpecMid + 
                           album->albumPath().remove(0, 1) + m_cellSpecEnd;
            }

            if (settings->getToolTipsShowComments())
            {
                str = info.comment();
                if (str.isEmpty()) str = QString("---");
                tip += m_cellSpecBeg + i18n("Comments:") + m_cellSpecMid + breakString(str) + m_cellSpecEnd;
            }

            if (settings->getToolTipsShowTags())
            {
                QStringList tagPaths = AlbumManager::componentData()->tagPaths(info.tagIds(), false);

                str = tagPaths.join(", ");
                if (str.isEmpty()) str = QString("---");
                if (str.length() > m_maxStringLen) str = str.left(m_maxStringLen-3) + "...";
                tip += m_cellSpecBeg + i18n("Tags:") + m_cellSpecMid + str + m_cellSpecEnd;
            }

            if (settings->getToolTipsShowRating())
            {
                str.fill( '*', info.rating() );
                if (str.isEmpty()) str = QString("---");
                tip += m_cellSpecBeg + i18n("Rating:") + m_cellSpecMid + str + m_cellSpecEnd;
            }
        }
    }
 
    return tip;
}

}  // NameSpace Digikam
