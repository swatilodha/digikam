/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-02-20
 * Description : a widget to display non standard Exif metadata
 *               used by camera makers
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QMap>
#include <QFile>

// KDE includes.

#include <klocale.h>

// Local includes.

#include "ddebug.h"
#include "dmetadata.h"
#include "makernotewidget.h"
#include "makernotewidget.moc"

namespace Digikam
{

// This list mix differents tags name used by camera makers.
static const char* MakerNoteHumanList[] =
{
     "AFFocusPos",
     "AFMode",
     "AFPoint",
     "AutofocusMode",
     "ColorMode",
     "ColorTemperature",
     "Contrast",
     "DigitalZoom",
     "ExposureMode",
     "ExposureProgram",
     "ExposureCompensation",
     "ExposureManualBias",
     "Flash",
     "FlashBias",
     "FlashMode",
     "FlashType",
     "FlashDevice",
     "FNumber", 
     "Focus"
     "FocusDistance",
     "FocusMode",
     "FocusSetting",
     "FocusType",
     "Hue",
     "HueAdjustment",
     "ImageStabilizer",
     "ImageStabilization",
     "InternalFlash", 
     "ISOSelection",
     "ISOSpeed",
     "Lens",
     "LensType",
     "LensRange",
     "Macro",
     "MacroFocus",
     "MeteringMode",
     "NoiseReduction",
     "OwnerName",
     "Quality",
     "Tone",
     "ToneComp",
     "Saturation",
     "Sharpness",
     "ShootingMode",
     "ShutterSpeedValue",
     "SpotMode",
     "SubjectDistance",
     "WhiteBalance",
     "WhiteBalanceBias",
     "-1"
};

static const char* ExifEntryListToIgnore[] =
{
     "GPSInfo",
     "Iop",
     "Thumbnail",
     "Image",
     "Photo",
     "-1"
};

MakerNoteWidget::MakerNoteWidget(QWidget* parent, const char* name)
               : MetadataWidget(parent, name)
{
    for (int i=0 ; QString(ExifEntryListToIgnore[i]) != QString("-1") ; i++)
        m_keysFilter << ExifEntryListToIgnore[i];

    for (int i=0 ; QString(MakerNoteHumanList[i]) != QString("-1") ; i++)
        m_tagsfilter << MakerNoteHumanList[i];
}

MakerNoteWidget::~MakerNoteWidget()
{
}

QString MakerNoteWidget::getMetadataTitle(void)
{
    return i18n("MakerNote EXIF Tags");
}

bool MakerNoteWidget::loadFromURL(const KUrl& url)
{
    setFileName(url.path());

    if (url.isEmpty())
    {
        setMetadata();
        return false;
    }
    else
    {    
        DMetadata metadata(url.path());
        QByteArray exifData = metadata.getExif();

        if (exifData.isEmpty())
        {
            setMetadata();
            return false;
        }
        else
            setMetadata(exifData);
    }

    return true;
}

bool MakerNoteWidget::decodeMetadata()
{
    DMetadata metaData;
    if (!metaData.setExif(getMetadata()))
        return false;

    // Update all metadata contents.
    setMetadataMap(metaData.getExifTagsDataList(m_keysFilter, true));

    return true;
}

void MakerNoteWidget::buildView(void)
{
    if (getMode() == SIMPLE)
    {
        setIfdList(getMetadataMap(), m_tagsfilter);
    }
    else
    {
        setIfdList(getMetadataMap());
    }
}

QString MakerNoteWidget::getTagTitle(const QString& key)
{
    QString title = DMetadata::getExifTagTitle(key.toAscii());

    if (title.isEmpty())
        return i18n("Unknown");

    return title;
}

QString MakerNoteWidget::getTagDescription(const QString& key)
{
    QString desc = DMetadata::getExifTagDescription(key.toAscii());

    if (desc.isEmpty())
        return i18n("No description available");

    return desc;
}

void MakerNoteWidget::slotSaveMetadataToFile(void)
{
    KUrl url = saveMetadataToFile(i18n("EXIF File to Save"),
                                  QString("*.dat|"+i18n("EXIF binary Files (*.dat)")));
    storeMetadataToFile(url);
}

}  // namespace Digikam

