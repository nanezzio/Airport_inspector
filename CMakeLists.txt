cmake_minimum_required(VERSION 3.16)

project(Airport_inspector VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Charts)

qt_standard_project_setup()

qt_add_executable(Airport_inspector
    main.cpp
    database.h
    database.cpp
    mainwindow.ui
    mainwindow.h
    mainwindow.cpp
    statsdialog.ui
    statsdialog.h
    statsdialog.cpp
)

target_link_libraries(Airport_inspector PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::Charts
)

set_target_properties(Airport_inspector PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)