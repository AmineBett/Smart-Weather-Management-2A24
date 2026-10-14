QT += core gui widgets printsupport

CONFIG += c++17

TARGET = SmartWeatherManagement
TEMPLATE = app

SOURCES += \
    main.cpp \
    gutilisateur.cpp \
    gbureaux.cpp \
    technicienne.cpp \
    stationwindow.cpp

HEADERS += \
    gutilisateur.h \
    gbureaux.h \
    technicienne.h \
    stationwindow.h

# Un seul fichier .ui pour tout le projet
FORMS += \
    gutilisateur.ui
