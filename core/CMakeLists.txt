# Core (GUI-independent) part of DoubleContact
#
# To use code under GPLv2, switch on USE_GPL2 define.
# NBU support will be disabled

cmake_minimum_required(VERSION 3.5)

add_library(S_CORE STATIC)
set_target_properties(S_CORE PROPERTIES AUTOMOC ON)
target_link_libraries(S_CORE PUBLIC Qt::Core Qt::Xml quazip_static)
target_include_directories(S_CORE PUBLIC .)

target_sources(S_CORE
    PRIVATE
    # HEADERS
    ################
        bstring.h
        contactlist.h
        corehelpers.h
        extra.h
        globals.h
        languagemanager.h
        formats/iformat.h
        formats/formatfactory.h
        formats/common/quotedprintable.h
        formats/common/textreport.h
        formats/common/vcarddata.h
        formats/common/vdata.h
        formats/files/csvfile.h
        formats/files/fileformat.h
        formats/files/htmlfile.h
        formats/files/mpbfile.h
        formats/files/nbffile.h
        formats/files/udxfile.h
        formats/files/vcfdirectory.h
        formats/files/vcffile.h
        formats/files/xmlcontactfile.h
        formats/network/asyncformat.h
        formats/profiles/csvprofilebase.h
        formats/profiles/explaybm50profile.h
        formats/profiles/explaytv240profile.h
        formats/profiles/genericcsvprofile.h
        formats/profiles/osmoprofile.h
        formats/profiles/sylpheedprofile.h
    # SOURCES
    ################
        bstring.cpp
        contactlist.cpp
        corehelpers.cpp
        extra.cpp
        globals.cpp
        languagemanager.cpp
        formats/formatfactory.cpp
        formats/common/quotedprintable.cpp
        formats/common/textreport.cpp
        formats/common/vcarddata.cpp
        formats/common/vdata.cpp
        formats/files/csvfile.cpp
        formats/files/fileformat.cpp
        formats/files/htmlfile.cpp
        formats/files/mpbfile.cpp
        formats/files/nbffile.cpp
        formats/files/udxfile.cpp
        formats/files/vcfdirectory.cpp
        formats/files/vcffile.cpp
        formats/files/xmlcontactfile.cpp
        formats/network/asyncformat.cpp
        formats/profiles/csvprofilebase.cpp
        formats/profiles/explaybm50profile.cpp
        formats/profiles/explaytv240profile.cpp
        formats/profiles/genericcsvprofile.cpp
        formats/profiles/osmoprofile.cpp
        formats/profiles/sylpheedprofile.cpp
)

if (WITH_CALLS)
    target_compile_definitions(S_CORE PRIVATE WITH_CALLS)
endif ()

if (WITH_MESSAGES)
    target_compile_definitions(S_CORE PRIVATE WITH_MESSAGES)
    target_sources(S_CORE
        PRIVATE
        # HEADERS
        ################
            decodedmessagelist.h
            formats/common/nokiadata.h
            formats/common/pdu.h
            formats/common/vmessagedata.h
        # SOURCES
        ################
            decodedmessagelist.cpp
            formats/common/nokiadata.cpp
            formats/common/pdu.cpp
            formats/common/vmessagedata.cpp
    )
endif ()

if (USE_GPL2)
    target_compile_definitions(S_CORE PRIVATE USE_GPL2)
else ()
    target_sources(S_CORE
        PRIVATE
        # HEADERS
        ################
            formats/files/nbufile.h
        # SOURCES
        ################
            formats/files/nbufile.cpp
    )
endif ()
