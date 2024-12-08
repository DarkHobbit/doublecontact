
cmake_minimum_required(VERSION 3.5)

add_subdirectory(quazip/quazip)

if (WITH_NETWORK)
    add_subdirectory(qwebdavlib/qwebdavlib)
endif ()
