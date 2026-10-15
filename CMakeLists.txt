cmake_minimum_required(VERSION 3.16)

project(SmartWeatherManagement LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport)

qt_add_executable(SmartWeatherManagement
    main.cpp

    gbureaux.cpp
    gbureaux.h
    gbureaux.ui

    stationwindow.cpp
    stationwuindow.h

    technicienne.cpp
    technicienne.h

    weatherwisetest.cpp
    weatherwisetest.h
    weatherwisetest.ui

    gutilisateur.cpp
    gutilisateur.h
    gutilisateur.ui
)

target_link_libraries(SmartWeatherManagement
    PRIVATE
    Qt6::Widgets
    Qt6::PrintSupport
)