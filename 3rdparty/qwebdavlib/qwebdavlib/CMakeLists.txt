
cmake_minimum_required(VERSION 3.5)

add_library(qwebdav STATIC)
set_target_properties(qwebdav PROPERTIES AUTOMOC ON)
target_link_libraries(qwebdav PUBLIC Qt::Core Qt::Network Qt::Xml)
target_include_directories(qwebdav PUBLIC .)
target_compile_definitions(qwebdav PRIVATE QWEBDAV_LIBRARY)

target_sources(qwebdav
    PRIVATE
    # HEADERS
    ################
        qwebdav.h
        qwebdavitem.h
        qwebdavdirparser.h
        qnaturalsort.h
        qwebdav_global.h
    # SOURCES
    ################
        qwebdav.cpp
        qwebdavitem.cpp
        qwebdavdirparser.cpp
        qnaturalsort.cpp
)
