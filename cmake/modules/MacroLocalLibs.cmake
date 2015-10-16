# Some useful macros to detect local or system based libraries
#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

macro(DETECT_LIBKIPI MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBKIPI)

        message(STATUS "libkipi : search system based library")
        find_package(KF5Kipi ${MIN_VERSION} CONFIG REQUIRED)

        if(KF5Kipi_FOUND)
            set(KF5Kipi_LIBRARIES KF5::Kipi)
            get_target_property(KF5Kipi_INCLUDE_DIRS KF5::Kipi INTERFACE_INCLUDE_DIRECTORIES)
            set(KF5Kipi_FOUND TRUE)
        else()
            set(KF5Kipi_FOUND FALSE)
        endif()

    else()

        message(STATUS "libkipi : use local library from ${CMAKE_SOURCE_DIR}/extra/libkipi/")
        find_file(KF5Kipi_FOUND CMakeLists.txt PATHS ${CMAKE_SOURCE_DIR}/extra/libkipi/)

        if(NOT KF5Kipi_FOUND)
            message(ERROR "libkipi : local library not found")
            set(KF5Kipi_FOUND FALSE)
        else()
            set(KF5Kipi_FOUND TRUE)            
        endif()

        set(KF5Kipi_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/extra/libkipi/src ${CMAKE_BINARY_DIR}/extra/libkipi)
        set(KF5Kipi_LIBRARIES KF5Kipi)

    endif()

    message(STATUS "libkipi found         : ${KF5Kipi_FOUND}")
    message(STATUS "libkipi library       : ${KF5Kipi_LIBRARIES}")
    message(STATUS "libkipi includes      : ${KF5Kipi_INCLUDE_DIRS}")

    # Detect libkipi so version used to compile kipi tool to identify if plugin can be loaded in memory by libkipi.
    # This will be used to populate plugin desktop files.

    find_file(KF5KipiConfig_FOUND libkipi_config.h PATHS ${KF5Kipi_INCLUDE_DIRS})
    file(READ "${KF5KipiConfig_FOUND}" KIPI_CONFIG_H_CONTENT)

    string(REGEX REPLACE
           ".*static +const +int +kipi_binary_version += ([^ ;]+).*"
           "\\1"
           KIPI_LIB_SO_CUR_VERSION_FOUND
           "${KIPI_CONFIG_H_CONTENT}"
          )

    set(KIPI_LIB_SO_CUR_VERSION ${KIPI_LIB_SO_CUR_VERSION_FOUND} CACHE STRING "libkipi so version")
    message(STATUS "libkipi SO version  : ${KIPI_LIB_SO_CUR_VERSION}")

endmacro()

###########################################################################################################################################"

macro(DETECT_LIBKEXIV2 MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBKEXIV2)

        message(STATUS "libkexiv2 : search system based library")
        find_package(KF5KExiv2 ${MIN_VERSION})

        if(KF5KExiv2_FOUND)
            set(KF5KExiv2_LIBRARIES KF5::KExiv2)
            get_target_property(KF5KExiv2_INCLUDE_DIRS KF5::KExiv2 INTERFACE_INCLUDE_DIRECTORIES)
            set(KF5KExiv2_FOUND TRUE)
        else()
            set(KF5KExiv2_FOUND FALSE)
        endif()

    else()

        message(STATUS "libkexiv2 : use local library from ${CMAKE_SOURCE_DIR}/extra/libkexiv2/")
        find_file(KF5KExiv2_FOUND CMakeLists.txt PATHS ${CMAKE_SOURCE_DIR}/extra/libkexiv2/)

        if(NOT KF5KExiv2_FOUND)
            message(ERROR "libkexiv2 : local library not found")
            set(KF5KExiv2_FOUND FALSE)
        else()
            set(KF5KExiv2_FOUND TRUE) 
        endif()

        set(KF5KExiv2_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/extra/libkexiv2/src ${CMAKE_BINARY_DIR}/extra/libkexiv2)
        set(KF5KExiv2_LIBRARIES KF5KExiv2)

    endif()

    message(STATUS "libkexiv2 found       : ${KF5KExiv2_FOUND}")
    message(STATUS "libkexiv2 library     : ${KF5KExiv2_LIBRARIES}")
    message(STATUS "libkexiv2 includes    : ${KF5KExiv2_INCLUDE_DIRS}")

endmacro()

###########################################################################################################################################"

macro(DETECT_LIBKFACE MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBKFACE)

        message(STATUS "libkface : search system based library")
        find_package(KF5KFace ${MIN_VERSION})

        if(KF5KFace_FOUND)
            set(KF5KFace_LIBRARIES KF5::KFace)
            get_target_property(KF5KFace_INCLUDE_DIRS KF5::KFace INTERFACE_INCLUDE_DIRECTORIES)
            set(KF5KFace_FOUND TRUE)
        else()
            set(KF5KFace_FOUND FALSE)
        endif()

    else()

        message(STATUS "libkface : use local library from ${CMAKE_SOURCE_DIR}/extra/libkface/")
        find_file(KF5KFace_FOUND CMakeLists.txt PATHS ${CMAKE_SOURCE_DIR}/extra/libkface/)

        if(NOT KF5KFace_FOUND)
            message(ERROR "libkface : local library not found")
            set(KF5KFace_FOUND FALSE)
        else()
            set(KF5KFace_FOUND TRUE) 
        endif()

        set(KF5KFace_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/extra/libkface/src ${CMAKE_BINARY_DIR}/extra/libkface)
        set(KF5KFace_LIBRARIES KF5KFace)

    endif()

    message(STATUS "libkface found        : ${KF5KFace_FOUND}")
    message(STATUS "libkface library      : ${KF5KFace_LIBRARIES}")
    message(STATUS "libkface includes     : ${KF5KFace_INCLUDE_DIRS}")

endmacro()

###########################################################################################################################################"

macro(DETECT_LIBKSANE MIN_VERSION)

    if (NOT DIGIKAMSC_COMPILE_LIBKSANE)

        message(STATUS "libksane : search system based library")
        find_package(KF5Sane ${MIN_VERSION})

        if(KF5Sane_FOUND)
            set(LIBKSANE_LIBRARIES KF5::Sane)
            get_target_property(LIBKSANE_INCLUDES KF5::Sane INTERFACE_INCLUDE_DIRECTORIES)
            set(KF5Sane_FOUND TRUE)
        else()
            set(KF5Sane_FOUND FALSE)
        endif()

    else()

        message(STATUS "libksane : use local library from ${CMAKE_SOURCE_DIR}/extra/libksane/")
        find_file(KF5Sane_FOUND CMakeLists.txt PATHS ${CMAKE_SOURCE_DIR}/extra/libksane/)

        if(NOT KF5Sane_FOUND)
            message(ERROR "libksane : local library not found")
            set(KF5Sane_FOUND FALSE)
        else()
            set(KF5Sane_FOUND TRUE)
        endif()

        set(LIBKSANE_INCLUDES ${CMAKE_SOURCE_DIR}/extra/libksane/src ${CMAKE_BINARY_DIR}/extra/libksane)
        set(LIBKSANE_LIBRARIES KF5Sane)

    endif()

    message(STATUS "libksane found      : ${KF5Sane_FOUND}")
    message(STATUS "libksane library    : ${LIBKSANE_LIBRARIES}")
    message(STATUS "libksane includes   : ${LIBKSANE_INCLUDES}")

endmacro()
