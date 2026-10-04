cmake_minimum_required(VERSION 3.16)

project(SmartWeather LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(SmartWeather
    main.cpp
    stationwindow.cpp
    stationwindow.h
)

target_link_libraries(SmartWeather
    PRIVATE
    Qt6::Widgets
)