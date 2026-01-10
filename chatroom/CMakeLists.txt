cmake_minimum_required(VERSION 3.16)

project(chatroom VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# AddressSanitizer
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

# UndefinedBehaviorSanitizer
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

if(ENABLE_ASAN)
  add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address)

  # 生成 LSAN 抑制文件
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/lsan_suppressions.txt
    "# Fcitx5 输入法\n"
    "leak:libFcitx5Qt6DBusAddons.so\n"
    "leak:libfcitx5platforminputcontextplugin.so\n"
    "# Qt Wayland 平台插件\n"
    "leak:QtWaylandClient\n"
    "leak:libQt6WaylandClient.so\n"
  )

  # 生成带抑制配置的启动脚本
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/run_with_asan.sh
    "#!/bin/bash\n"
    "LSAN_OPTIONS=suppressions=${CMAKE_CURRENT_BINARY_DIR}/lsan_suppressions.txt "
    "exec ${CMAKE_CURRENT_BINARY_DIR}/chatroom \"$@\"\n"
  )
  file(CHMOD ${CMAKE_CURRENT_BINARY_DIR}/run_with_asan.sh
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
  )
endif()

if(ENABLE_UBSAN)
  add_compile_options(-fsanitize=undefined)
  add_link_options(-fsanitize=undefined)
endif()

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
        chatdialog.hpp chatdialog.cc chatdialog.ui
        chatuseritem.hpp chatuseritem.cc chatuseritem.ui
        loadingitem.hpp loadingitem.cc loadingitem.ui
        chatpage.hpp chatpage.cc chatpage.ui
        searchuseritem.hpp searchuseritem.cc searchuseritem.ui
        contactitem.hpp contactitem.cc contactitem.ui
        settingdialog.hpp settingdialog.cc settingdialog.ui
        findsuccessdialog.hpp findsuccessdialog.cc findsuccessdialog.ui
        findfaileddialog.hpp findfaileddialog.cc findfaileddialog.ui
        friendapplydialog.hpp friendapplydialog.cc friendapplydialog.ui
        chatmsgitem.hpp chatmsgitem.cc
        global.hpp  global.cc
        timerbutton.hpp timerbutton.cc
        httpmanager.hpp httpmanager.cc
        serverinfo.hpp serverinfo.cc
        tcpmanager.hpp tcpmanager.cc
        userinfo.hpp userinfo.cc
        chatuserlist.hpp chatuserlist.cc
        chatmsglist.hpp chatmsglist.cc
        chattextedit.hpp chattextedit.cc
        bubblebase.hpp bubblebase.cc
        textbubble.hpp textbubble.cc
        picturebubble.hpp picturebubble.cc
        sidebarwidget.hpp sidebarwidget.cc
        sidebaritem.hpp sidebaritem.cc
        searchuserlist.hpp searchuserlist.cc
        contactlist.hpp contactlist.cc
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
            chatdialog.hpp chatdialog.cc chatdialog.ui
            chatuseritem.hpp chatuseritem.cc chatuseritem.ui
            loadingitem.hpp loadingitem.cc loadingitem.ui
            chatpage.hpp chatpage.cc chatpage.ui
            searchuseritem.hpp searchuseritem.cc searchuseritem.ui
            contactitem.hpp contactitem.cc contactitem.ui
            settingdialog.hpp settingdialog.cc settingdialog.ui
            findsuccessdialog.hpp findsuccessdialog.cc findsuccessdialog.ui
            findfaileddialog.hpp findfaileddialog.cc findfaileddialog.ui
            friendapplydialog.hpp friendapplydialog.cc friendapplydialog.ui
            chatmsgitem.hpp chatmsgitem.cc
            global.hpp  global.cc
            timerbutton.hpp timerbutton.cc
            httpmanager.hpp httpmanager.cc
            serverinfo.hpp serverinfo.cc
            tcpmanager.hpp tcpmanager.cc
            userinfo.hpp userinfo.cc
            chatuserlist.hpp chatuserlist.cc
            chatmsglist.hpp chatmsglist.cc
            chattextedit.hpp chattextedit.cc
            bubblebase.hpp bubblebase.cc
            textbubble.hpp textbubble.cc
            picturebubble.hpp picturebubble.cc
            sidebarwidget.hpp sidebarwidget.cc
            sidebaritem.hpp sidebaritem.cc
            searchuserlist.hpp searchuserlist.cc
            contactlist.hpp contactlist.cc
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
            chatdialog.hpp chatdialog.cc chatdialog.ui
            chatuseritem.hpp chatuseritem.cc chatuseritem.ui
            loadingitem.hpp loadingitem.cc loadingitem.ui
            chatpage.hpp chatpage.cc chatpage.ui
            searchuseritem.hpp searchuseritem.cc searchuseritem.ui
            contactitem.hpp contactitem.cc contactitem.ui
            settingdialog.hpp settingdialog.cc settingdialog.ui
            findsuccessdialog.hpp findsuccessdialog.cc findsuccessdialog.ui
            findfaileddialog.hpp findfaileddialog.cc findfaileddialog.ui
            friendapplydialog.hpp friendapplydialog.cc friendapplydialog.ui
            chatmsgitem.hpp chatmsgitem.cc
            global.hpp  global.cc
            timerbutton.hpp timerbutton.cc
            httpmanager.hpp httpmanager.cc
            serverinfo.hpp serverinfo.cc
            tcpmanager.hpp tcpmanager.cc
            userinfo.hpp userinfo.cc
            chatuserlist.hpp chatuserlist.cc
            chatmsglist.hpp chatmsglist.cc
            chattextedit.hpp chattextedit.cc
            bubblebase.hpp bubblebase.cc
            textbubble.hpp textbubble.cc
            picturebubble.hpp picturebubble.cc
            sidebarwidget.hpp sidebarwidget.cc
            sidebaritem.hpp sidebaritem.cc
            searchuserlist.hpp searchuserlist.cc
            contactlist.hpp contactlist.cc
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

# 复制文件到构建目录
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.ini
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/static
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(chatroom)
endif()
