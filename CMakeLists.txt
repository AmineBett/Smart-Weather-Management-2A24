cmake_minimum_required(VERSION 3.16)

project(SmartWeather LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(SmartWeather
    gbureaux.cpp
    gbureaux.h
    main.cpp
    smartweatherwindow.cpp
    smartweatherwindow.h
    stationwindow.cpp
    stationwindow.h
    technicienne.cpp
    technicienne.h
    weatherwisetest.cpp
    weatherwisetest.h
)

target_link_libraries(SmartWeather
    PRIVATE
    Qt6::Widgets
)
