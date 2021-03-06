#
# Copyright (c) 2010-2016 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif (POLICY CMP0063)

set(libcommonwidgets_SRCS
    common/abstractitemdragdrophandler.cpp
    common/buttonicondisabler.cpp
    common/colorgradientwidget.cpp
    common/comboboxutilities.cpp
    common/dsliderspinbox.cpp
    common/dnuminput.cpp
    common/dcombobox.cpp
    common/dexpanderbox.cpp
    common/dactivelabel.cpp
    common/dfontselect.cpp
    common/dgradientslider.cpp
    common/dragdropimplementations.cpp
    common/filesaveoptionsbox.cpp
    common/fullscreensettings.cpp
    common/modelcompleter.cpp
    common/searchtextbar.cpp
    common/splashscreen.cpp
    common/statesavingobject.cpp
    common/visibilitycontroller.cpp
    common/workingwidget.cpp
    common/drawdecoderwidget.cpp
    common/dwidgetutils.cpp
    common/squeezedcombobox.cpp
)

set(libimagehistory_SRCS
    imagehistory/filtershistorywidget.cpp
    imagehistory/imagefiltershistoryitemdelegate.cpp
    imagehistory/imagefiltershistorymodel.cpp
    imagehistory/imagefiltershistorytreeitem.cpp
    imagehistory/versionswidget.cpp
)

set(libcolorchooser_SRCS
    colorchooser/dcolorvalueselector.cpp
    colorchooser/dhuesaturationselect.cpp
    colorchooser/dcolorchoosermode.cpp
)

set(libmainview_SRCS
    mainview/dxmlguiwindow.cpp
    mainview/daboutdata.cpp
    mainview/dzoombar.cpp
    mainview/dlogoaction.cpp
    mainview/ditemtooltip.cpp
    mainview/dhistoryview.cpp
    mainview/dcursortracker.cpp
    mainview/thumbbardock.cpp
    mainview/thememanager.cpp
    mainview/sidebar.cpp
    mainview/dmessagebox.cpp
    ../../app/utils/digikam_globals.cpp
   )

set(libitemview_SRCS
    itemview/dcategorizedview.cpp
    itemview/dcategorizedsortfilterproxymodel.cpp
    itemview/ditemdelegate.cpp
    itemview/dcategorydrawer.cpp
    itemview/imagedelegateoverlay.cpp
    itemview/itemviewhoverbutton.cpp
    itemview/itemviewimagedelegate.cpp
    itemview/itemviewtooltip.cpp
    itemview/itemviewcategorized.cpp
)

set(libgraphicsviewwidgets_SRCS
    graphicsview/clickdragreleaseitem.cpp
    graphicsview/dimgchilditem.cpp
    graphicsview/dimgpreviewitem.cpp
    graphicsview/regionframeitem.cpp
    graphicsview/graphicsdimgitem.cpp
    graphicsview/graphicsdimgview.cpp
    graphicsview/imagezoomsettings.cpp
    graphicsview/previewlayout.cpp
    graphicsview/paniconwidget.cpp
    graphicsview/itemvisibilitycontroller.cpp
)

set(libmetadatawidgets_SRCS
    metadata/mdkeylistviewitem.cpp
    metadata/metadatalistview.cpp
    metadata/metadatalistviewitem.cpp
    metadata/metadatawidget.cpp
    metadata/metadataselector.cpp
    metadata/metadatapanel.cpp
    metadata/iptcwidget.cpp
    metadata/exifwidget.cpp
    metadata/makernotewidget.cpp
    metadata/xmpwidget.cpp
    metadata/ratingwidget.cpp
    metadata/colorlabelwidget.cpp
    metadata/picklabelwidget.cpp
    metadata/altlangstredit.cpp
    metadata/colorlabelwidget.cpp
    metadata/countryselector.cpp
    metadata/subjectwidget.cpp
)

set(libiccprofileswidgets_SRCS
    iccprofiles/iccpreviewwidget.cpp
    iccprofiles/iccprofilewidget.cpp
    iccprofiles/cietonguewidget.cpp
    iccprofiles/iccprofilescombobox.cpp
)

include_directories(
    $<TARGET_PROPERTY:KF5::Kipi,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::NotifyConfig,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:Qt5::PrintSupport,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
)

if(GPHOTO2_FOUND)
    include_directories(${GPHOTO2_INCLUDE_DIRS})
endif()

add_library(digikamwidgetscore_src OBJECT
    ${libmainview_SRCS}
    ${libcommonwidgets_SRCS}
    ${libgraphicsviewwidgets_SRCS}
    ${libiccprofileswidgets_SRCS}
    ${libitemview_SRCS}
    ${libmetadatawidgets_SRCS}
    ${libcolorchooser_SRCS}
)

add_library(imagehistorywidgets_src OBJECT ${libimagehistory_SRCS})
