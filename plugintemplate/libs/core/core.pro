include(../../../libs.pri)

QT += widgets

DEFINES += CORE_LIBRARY
TARGET = $$replaceLibName(core)

SOURCES += \
    mpages.cpp

HEADERS += \
    core_global.h \
    mpages.h

QMAKE_TARGET_PRODUCT = Core
QMAKE_TARGET_DESCRIPTION = Plug-in core
