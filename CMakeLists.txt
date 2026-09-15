cmake_minimum_required(VERSION 3.16)

project(BookShelf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(BookShelf
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/bookmodel.h
    src/bookmodel.cpp
)

target_link_libraries(BookShelf PRIVATE Qt6::Widgets)

set_target_properties(BookShelf PROPERTIES WIN32_EXECUTABLE ON)
