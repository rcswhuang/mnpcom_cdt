
QT += serialport
QT += widgets
QT += core
#CONFIG += console
#CONFIG -= app_bundle
TEMPLATE = app
TARGET = mnpComCdt
DEFINES += PLUGIN_DLL
CONFIG += qt warn_on thread exceptions

INCLUDEPATH += \
             ../include

HEADERS += \
    hpointseldlg.h \
    huserhandle.h \
    hpointinputdlg.h \
    hprotocol.h \
    #huserworker.h \
    huserminitor.h \
    hmsgglobal.h \
    hserialport.h

SOURCES += \
    main.cpp \
    hpointseldlg.cpp \
    huserhandle.cpp \
    hpointinputdlg.cpp \
    hprotocol.cpp \
    #huserworker.cpp \
    huserminitor.cpp \
    hserialport.cpp

FORMS += \
    UI/pointseldlg.ui \
    UI/pointinputdlg.ui \
    UI/userminitor.ui

