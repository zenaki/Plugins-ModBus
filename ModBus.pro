QT += core network
QT -= gui

CONFIG += c++11

TARGET = ModBus
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
        3rdparty/qextserialport/qextserialport.cpp \
        3rdparty/libmodbus/src/modbus.c \
        3rdparty/libmodbus/src/modbus-data.c \
        3rdparty/libmodbus/src/modbus-tcp.c \
    controller/worker.cpp

#include(qtservice/src/qtservice.pri)

HEADERS += imodbus.h \
        3rdparty/qextserialport/qextserialport.h \
        3rdparty/qextserialport/qextserialenumerator.h \
        3rdparty/libmodbus/src/modbus.h \
    controller/worker.h

INCLUDEPATH += 3rdparty/libmodbus \
               3rdparty/libmodbus/src \
               3rdparty/qextserialport \
               src
unix {
    SOURCES += 3rdparty/qextserialport/posix_qextserialport.cpp	\
           3rdparty/qextserialport/qextserialenumerator_unix.cpp
    DEFINES += _TTY_POSIX_
}

win32 {
    SOURCES += 3rdparty/qextserialport/win_qextserialport.cpp \
                3rdparty/qextserialport/qextserialenumerator_win.cpp
    DEFINES += _TTY_WIN_  WINVER=0x0501
    LIBS += -lsetupapi -lws2_32
}
