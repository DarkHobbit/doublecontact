
cmake_minimum_required(VERSION 3.5)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    install(FILES uchmviewer.png DESTINATION ${APP_ICONS_INSTALL_DIR})
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    install(
    FILES
        32x32/doublecontact_32x32.xpm
    DESTINATION ${APP_IMG_PATH})
elseif (&{CMAKE_SYSTEM_NAME} MATCHES "Windows")
    install(
    FILES
        32x32/doublecontact_32x32.png
    DESTINATION ${APP_IMG_PATH})
endif ()
