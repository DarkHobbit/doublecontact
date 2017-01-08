project(doublecontact CXX)

cmake_minimum_required ( VERSION 2.8.8 )
add_definitions(-Wall -g)
find_package(Qt4 COMPONENTS QtCore QtGui QtXml REQUIRED)

set(SOURCES main.cpp mainwindow.cpp contactdialog.cpp phonetypedialog.cpp logwindow.cpp settingsdialog.cpp datedetailsdialog.cpp helpers.cpp comparecontainers.cpp comparedialog.cpp multicontactdialog.cpp aboutdialog.cpp languageselectdialog.cpp)
set(UIS mainwindow.ui contactdialog.ui phonetypedialog.ui logwindow.ui settingsdialog.ui datedetailsdialog.ui comparedialog.ui multicontactdialog.ui aboutdialog.ui languageselectdialog.ui)
set(MOC_HEADERS mainwindow.h contactdialog.h phonetypedialog.h logwindow.h settingsdialog.h datedetailsdialog.h comparedialog.h multicontactdialog.h aboutdialog.h languageselectdialog.h
 helpers.h comparecontainers.h ../model/contactmodel.h)

include_directories(
 ${CMAKE_BINARY_DIR} ${QT_INCLUDE_DIR} ${QT_INCLUDE_DIR}/QtCore ${QT_INCLUDE_DIR}/QtGui ${QT_INCLUDE_DIR}/QtXml
 ../core ../model)
add_subdirectory(../core "${CMAKE_CURRENT_BINARY_DIR}/core")
add_subdirectory(../model "${CMAKE_CURRENT_BINARY_DIR}/model")

include(${QT_USE_FILE})
qt4_wrap_ui(UI_HEADERS ${UIS})
qt4_wrap_cpp(MOC_SRCS ${MOC_HEADERS})

set(TR_DIR ../translations)
set(TS_FILES ${TR_DIR}/doublecontact_ru_RU.ts ${TR_DIR}/doublecontact_en_GB.ts)
qt4_add_translation(qm_files ${TS_FILES})

add_executable(doublecontact ${SOURCES} $<TARGET_OBJECTS:S_CORE> $<TARGET_OBJECTS:S_MODEL> ${UI_HEADERS} ${MOC_SRCS} ${qm_files})
target_link_libraries(doublecontact ${QT_LIBRARIES})
