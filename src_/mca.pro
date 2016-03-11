TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += \
    ../../libraries/LiquidCrystal_I2C \
    ../../libraries/Rtc_by_Makuna/src

DEPENDPATH += \
    ../../libraries/LiquidCrystal_I2C \
    ../../libraries/Rtc_by_Makuna/src

HEADERS += \
    main/defines.h \
    main/functions.h \
    main/main.ino \
    main/temp.h
