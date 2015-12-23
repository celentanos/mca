TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH +=
    ../libraries/LiquidCrystal_I2C

DEPENDPATH +=
    ../libraries/LiquidCrystal_I2C

HEADERS += \
    src/main/defines.h \
    src/main/functions.h \
    src/main/main.ino
