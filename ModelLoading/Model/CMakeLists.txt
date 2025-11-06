cmake_minimum_required(VERSION 3.16)

project(Model VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_definitions(-DASSIMP_DLL)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)

INCLUDE_DIRECTORIES(. ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

# link_directories要放在add_executable之前，不然无效。
link_directories(E:/lzh/study/OpenglQt6/github/ModelLoading/Model/libs/)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Model
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        lzhopenglwidget.h lzhopenglwidget.cpp
        res.qrc
        model.h
        mesh.h
        shader.h
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Model APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Model SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Model
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Model PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Model PRIVATE Qt6::OpenGLWidgets)

#target_link_libraries(Model PRIVATE assimp freetype glfw3 irrKlang)
target_link_libraries(Model PRIVATE assimp-vc142-mt)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Model)
endif()
set_target_properties(Model PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Model
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Model)
endif()
