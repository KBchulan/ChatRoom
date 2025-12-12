cmake_minimum_required(VERSION 3.16)

project(chatroom VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
        main.cc
        mainwindow.cc
        mainwindow.hpp
        mainwindow.ui
        resources/resources.qrc
)

# Windows 图标资源
if(WIN32)
    list(APPEND PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources/app.rc)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(chatroom
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        logindialog.hpp logindialog.cc logindialog.ui
        registerdialog.hpp registerdialog.cc registerdialog.ui
        resetpassworddialog.hpp resetpassworddialog.cc resetpassworddialog.ui
        global.hpp  global.cc
        timerbutton.hpp timerbutton.cc
        httpmanager.hpp httpmanager.cc
        serverinfo.hpp serverinfo.cc
        Defer.hpp config.ini
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET chatroom APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(chatroom SHARED
            ${PROJECT_SOURCES}
            logindialog.hpp logindialog.cc logindialog.ui
            registerdialog.hpp registerdialog.cc registerdialog.ui
            resetpassworddialog.hpp resetpassworddialog.cc resetpassworddialog.ui
            global.hpp  global.cc
            timerbutton.hpp timerbutton.cc
            httpmanager.hpp httpmanager.cc
            serverinfo.hpp serverinfo.cc
            Defer.hpp config.ini
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(chatroom
            ${PROJECT_SOURCES}
            logindialog.hpp logindialog.cc logindialog.ui
            registerdialog.hpp registerdialog.cc registerdialog.ui
            resetpassworddialog.hpp resetpassworddialog.cc resetpassworddialog.ui
            global.hpp  global.cc
            timerbutton.hpp timerbutton.cc
            httpmanager.hpp httpmanager.cc
            serverinfo.hpp serverinfo.cc
            Defer.hpp config.ini
        )
    endif()
endif()

target_include_directories(chatroom PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(chatroom PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.chatroom)
endif()
set_target_properties(chatroom PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS chatroom
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 复制配置文件到构建目录
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.ini
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(chatroom)
endif()
