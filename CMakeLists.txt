cmake_minimum_required(VERSION 3.5)

project(MPR-Viewer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(VTK REQUIRED)
find_package(ITK REQUIRED)

set(SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/MPRViewerWidget.cpp
        src/filePage.cpp
        src/mainwindow.ui
        src/customemprviewerinteractor.cpp
)

set(HEADERS
        include/mainwindow.h
        include/MPRViewerWidget.h
        include/filePage.h
        include/customemprviewerinteractor.h
        # without ITKVTKGlue, need these header file
        include/itkImageToVTKImageFilter.h
        include/itkVTKImageToImageFilter.h
        include/itkImageToVTKImageFilter.hxx
        include/itkVTKImageToImageFilter.hxx
)

set(RESOURCE
        style/res.qrc
#        style/styles.qss
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MPR-Viewer
        MANUAL_FINALIZATION
        ${HEADERS} ${SOURCES} ${RESOURCE}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET MPR-Viewer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MPR-Viewer SHARED
            ${HEADERS} ${SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MPR-Viewer
            ${HEADERS} ${SOURCES} ${RESOURCE}
        )
    endif()
endif()

target_link_libraries(MPR-Viewer PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${VTK_LIBRARIES}
    ${ITK_LIBRARIES})

set_target_properties(MPR-Viewer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS MPR-Viewer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MPR-Viewer)
endif()
