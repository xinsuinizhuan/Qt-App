include(../../libs.pri)

QT += widgets

DEFINES += CONTROLS_LIBRARY
TARGET = $$replaceLibName(controls)

LIBS += -l$$replaceLibName(utils)

SOURCES += \
    accountcontrols.cpp \
    commonwidget.cc \
    dialog.cc \
    messbox.cpp \
    waitwidget.cpp

HEADERS += \
    accountcontrols.h \
    commonwidget.hpp \
    controls_global.h \
    dialog.hpp \
    messbox.h \
    waitwidget.h

QMAKE_TARGET_PRODUCT = Controls
QMAKE_TARGET_DESCRIPTION = Control Collection
