TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += \
    ../../../dev/arduino/hardware/arduino/avr/cores/arduino \
    ../../../dev/arduino/hardware/arduino/avr/libraries/Wire \
    ../../libraries/LiquidCrystal_I2C \
    ../../libraries/Rtc_by_Makuna/src \
    ../../libraries/DallasTemperature \
    ../../libraries/OneWire

DEPENDPATH += \
    ../../../dev/arduino/hardware/arduino/avr/cores/arduino \
    ../../../dev/arduino/hardware/arduino/avr/libraries/Wire \
    ../../libraries/LiquidCrystal_I2C \
    ../../libraries/Rtc_by_Makuna/src \
    ../../libraries/DallasTemperature \
    ../../libraries/OneWire

HEADERS += \
    main/defines.h \
    main/functions.h \
    main/main.ino \
    main/temp.h
