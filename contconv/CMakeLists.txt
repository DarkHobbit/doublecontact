
cmake_minimum_required (VERSION 3.5)

project(ContConv LANGUAGES CXX)

add_executable(contconv)
set_target_properties(contconv PROPERTIES AUTOMOC ON)
target_link_libraries(contconv Qt::Core S_CORE)

install(TARGETS contconv DESTINATION ${APP_BIN_PATH})

target_sources(contconv
    PRIVATE
    # HEADERS
    ################
        convertor.h
        consoleasyncui.h
    # SOURCES
    ################
        main.cpp
        convertor.cpp
        consoleasyncui.cpp
)
