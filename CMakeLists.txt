################################################################################
#
# Options:
#   REQUEST_QT:STRING   Explicitly specify the required generation of Qt. This
#                       can be useful if different major versions of Qt are
#                       installed on the system. Valid values are '4', '5' or
#                       '6'. If not specified, the build script uses the first
#                       available major version of Qt, starting the search at
#                       '4' and ending at '6'.
#   WITH_CALLS:BOOL
#   WITH_MESSAGES:BOOL
#   WITH_NETWORK:BOOL
#   USE_GPL2:BOOL
#
################################################################################


cmake_minimum_required(VERSION 3.11)

project(DoubleContact VERSION 0.2.5 LANGUAGES CXX)

option(WITH_CALLS    "" ON)
option(WITH_MESSAGES "" ON)
option(USE_GPL2      "" OFF)
option(WITH_NETWORK  "" OFF)

set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

# Necessary packages
find_package(libzip REQUIRED)
find_package(ZLIB REQUIRED)


# What version of Qt are we working with?
################################################################################

if (NOT ${REQUEST_QT} OR ${REQUEST_QT} EQUAL 4)
    if (WITH_NETWORK)
        set(EXTRA_PACK QtNetwork)
    else ()
        unset(EXTRA_PACK)
    endif ()

    find_package(Qt4 COMPONENTS QtCore QtGui QtXml ${EXTRA_PACK})

    if (Qt4_FOUND)
        set(REQUEST_QT 4)
    elseif (${REQUEST_QT})
        message(FATAL_ERROR "The required Qt4 was not found.")
    endif ()
endif ()

if (NOT ${REQUEST_QT} OR ${REQUEST_QT} EQUAL 5)
    if (WITH_NETWORK)
        set(EXTRA_PACK Network)
    else ()
        unset(EXTRA_PACK)
    endif ()

    find_package(Qt5 COMPONENTS Core PrintSupport Widgets Xml ${EXTRA_PACK})

    if (Qt5_FOUND)
        set(REQUEST_QT 5)
    elseif (${REQUEST_QT})
        message(FATAL_ERROR "The required Qt5 was not found.")
    endif ()
endif ()

if (NOT ${REQUEST_QT} OR ${REQUEST_QT} EQUAL 6)
    if (WITH_NETWORK)
        set(EXTRA_PACK Network)
    else ()
        unset(EXTRA_PACK)
    endif ()

    find_package(Qt6 COMPONENTS Core PrintSupport Widgets Xml Core5Compat ${EXTRA_PACK})

    if (Qt6_FOUND)
        set(REQUEST_QT 6)
    elseif (${REQUEST_QT})
        message(FATAL_ERROR "The required Qt6 was not found.")
    endif ()
endif ()

if (NOT ${REQUEST_QT})
    message(FATAL_ERROR "No suitable version of Qt was found.")
endif ()

set(QT Qt${REQUEST_QT})
message("-- Used Qt version: ${REQUEST_QT}")


# Aliases for Qt libraries for easy use regardless of version.
################################################################################

macro(qt_aliase)
    if(NOT TARGET Qt::${ARGV0})
        if (${REQUEST_QT} EQUAL 4)
            set(_lib ${ARGV1})
        else ()
            set(_lib ${ARGV0})
        endif ()

        if (TARGET ${QT}::${_lib})
            add_library(Qt::${ARGV0} ALIAS ${QT}::${_lib})
        else ()
            message(WARNING "the ${QT}::${_lib} lib not found")
        endif ()
    endif()
endmacro()

qt_aliase(Core QtCore)
qt_aliase(PrintSupport QtGui)
qt_aliase(Widgets QtGui)
qt_aliase(Xml QtXml)

if (WITH_NETWORK)
    qt_aliase(Network QtNetwork)
endif ()


# The location of the installation directories. Only relative paths may be used.
#
#       Common:
#
# APP_BIN_PATH    directory to install executable.
# APP_I18N_PATH   directory to install translations.
# APP_DOC_PATH    directory to install documentation.
# APP_IMG_PATH    directory to install application icons.
#
#       System-specific:
#
# APP_DSK_PATH    installation directory for a desktop entry in GNU/Linux.
#
# From https://cmake.org/cmake/help/latest/command/install.html
#
# As absolute paths are not supported by cpack installer generators, it is
# preferable to use relative paths throughout. In particular, there is no need
# to make paths absolute by prepending CMAKE_INSTALL_PREFIX; this prefix is
# used by default if the DESTINATION is a relative path.
################################################################################

if (${CMAKE_SYSTEM_NAME}  MATCHES "Windows")
    set(APP_ROOT_PATH doublecontact)
    set(APP_BIN_PATH  ${APP_ROOT_PATH})
    set(APP_I18N_PATH ${APP_ROOT_PATH})
    set(APP_IMG_PATH  ${APP_ROOT_PATH})
    set(APP_DOC_PATH  ${APP_ROOT_PATH}/doc)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
else ()
    set(APP_BIN_PATH  bin)
    set(APP_I18N_PATH share/doublecontact/translations)
    set(APP_DSK_PATH  share/applications)
    set(APP_IMG_PATH  share/pixmaps)
    set(APP_DOC_PATH  share/doc/doublecontact)
endif ()


# Run in those subdirectories
################################################################################

add_subdirectory(3rdparty)
add_subdirectory(app)
add_subdirectory(contconv)
add_subdirectory(core)
add_subdirectory(img)
add_subdirectory(model)
add_subdirectory(translations)


# Install documentation.
################################################################################

file(GLOB APP_DOC_FILES LIST_DIRECTORIES FALSE ./COPYING ./README.md doc/*.*)
install(FILES ${APP_DOC_FILES} DESTINATION ${APP_DOC_PATH})


# Settings for packers.
# Currently only DEB is supported
################################################################################

# https://cmake.org/cmake/help/latest/cpack_gen/deb.html
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Mikhail Y. Zvyozdochkin")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Offline DE-independent contact manager")
set(CPACK_DEBIAN_PACKAGE_SECTION "misc")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include (CPack)
