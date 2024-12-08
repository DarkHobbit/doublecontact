# Model-related part of DoubleContact

cmake_minimum_required(VERSION 3.5)

add_library(S_MODEL STATIC)
set_target_properties(S_MODEL PROPERTIES AUTOMOC ON)
target_link_libraries(S_MODEL PUBLIC Qt::Core Qt::Widgets S_CORE)
target_include_directories(S_MODEL PUBLIC .)

target_sources(S_MODEL
    PRIVATE
    # HEADERS
    ################
        contactmodel.h
        innerfilemodel.h
        modelhelpers.h
        recentlist.h
        configmanager.h
    # SOURCES
    ################
        contactmodel.cpp
        innerfilemodel.cpp
        modelhelpers.cpp
        recentlist.cpp
        configmanager.cpp
)

if (WITH_CALLS)
    target_compile_definitions(S_MODEL PRIVATE WITH_CALLS)
    target_sources(S_MODEL
        PRIVATE
            callmodel.h
            callmodel.cpp
    )
endif ()

if (WITH_MESSAGES)
    target_compile_definitions(S_MODEL PRIVATE WITH_MESSAGES)
    target_sources(S_MODEL
        PRIVATE
            messagemodel.h
            messagemodel.cpp
    )
endif ()
