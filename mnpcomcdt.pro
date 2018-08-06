
QT += serialport
QT += widgets
QT += core
#CONFIG += console
#CONFIG -= app_bundle
TEMPLATE = app
TARGET = mnpComCdt
DEFINES += PLUGIN_DLL
CONFIG += qt warn_on thread exceptions
DESTDIR = ../lib

INCLUDEPATH += \
             ..\include

HEADERS += \
    #hserialport.h \
    hpointseldlg.h \
    huserhandle.h \
    hpointinputdlg.h \
    hprotocol.h \
    #huserworker.h \
    huserminitor.h

SOURCES += \
    main.cpp \
    #hserialport.cpp \
    hpointseldlg.cpp \
    huserhandle.cpp \
    hpointinputdlg.cpp \
    hprotocol.cpp \
    #huserworker.cpp \
    huserminitor.cpp

FORMS += \
    UI/pointseldlg.ui \
    UI/pointinputdlg.ui \
    UI/userminitor.ui

