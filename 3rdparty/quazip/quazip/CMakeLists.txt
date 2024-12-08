
cmake_minimum_required(VERSION 3.5)

project(quazip VERSION 1.0.0 LANGUAGES C CXX)

add_library(quazip_static STATIC)
set_target_properties(quazip_static PROPERTIES AUTOMOC ON)
target_link_libraries(quazip_static PUBLIC Qt::Core libzip::zip ZLIB::ZLIB)
target_include_directories(quazip_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(quazip_static PRIVATE QUAZIP_BUILD)

target_sources(quazip_static
    PRIVATE
    # HEADERS
    ################
         crypt.h
         ioapi.h
         JlCompress.h
         quaadler32.h
         quachecksum32.h
         quacrc32.h
         quagzipfile.h
         quaziodevice.h
         quazipdir.h
         quazipfile.h
         quazipfileinfo.h
         quazip_global.h
         quazip.h
         quazipnewinfo.h
         unzip.h
         zip.h
    # SOURCES
    ################
        qioapi.cpp
        JlCompress.cpp
        quaadler32.cpp
        quacrc32.cpp
        quagzipfile.cpp
        quaziodevice.cpp
        quazip.cpp
        quazipdir.cpp
        quazipfile.cpp
        quazipfileinfo.cpp
        quazipnewinfo.cpp
        unzip.c
        zip.c
 )

if (TARGET ${QT}::Core5Compat)
    target_link_libraries(quazip_static PUBLIC ${QT}::Core5Compat)
endif ()
