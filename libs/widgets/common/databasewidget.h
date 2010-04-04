/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-11-14
 * Description : database migration dialog
 *
 * Copyright (C) 2009-2010 by Holger Foerster <Hamsi2k at freenet dot de>
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

#ifndef DATABASEWIDGET_H
#define DATABASEWIDGET_H

// Qt includes

#include <QString>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>

// KDE includes

#include <kdialog.h>
#include <kurlrequester.h>
#include <kurl.h>

// Local includes

#include "albumsettings.h"
#include "databaseparameters.h"

namespace Digikam
{


class DatabaseWidget : public QWidget
{
    Q_OBJECT

public:

    DatabaseWidget(QWidget* parent);
    ~DatabaseWidget();

    KUrlRequester* databasePathEdit;
    QString        originalDbPath;
    QString        originalDbType;
    QLabel*        databasePathLabel;
    QComboBox*     databaseType;
    QCheckBox*     internalServer;
    QSpinBox*      hostPort;

    QLineEdit*     databaseName;
    QLineEdit*     databaseNameThumbnails;
    QLineEdit*     hostName;
    QLineEdit*     connectionOptions;
    QLineEdit*     userName;
    QLineEdit*     password;

    QGroupBox*     expertSettings;

public:

    void setParametersFromSettings(const AlbumSettings* settings);
    DatabaseParameters getDatabaseParameters();

public Q_SLOTS:

    void slotChangeDatabasePath(const KUrl&);
    void slotDatabasePathEdited(const QString&);
    void setDatabaseInputFields(const QString&);
    void slotHandleInternalServerCheckbox(int enableFields);
    void checkDatabaseConnection();

private:

    void checkDBPath();
    void setupMainArea();
};

}  // namespace Digikam

#endif  // DATABASEWIDGET_H
