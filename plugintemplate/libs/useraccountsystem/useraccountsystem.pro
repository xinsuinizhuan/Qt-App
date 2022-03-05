include(../../../libs.pri)
include(../../PluginTemplate.pri)

QT += widgets sql

DEFINES += USERACCOUNTSYSTEM_LIBRARY
TARGET = $$replaceLibName(useraccountsystem)

LIBS += \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(controls) \
    -l$$replaceLibName(tools) \
    -l$$replaceLibName(extensionsystem)

SOURCES += \
    accountquery.cpp \
    changepasswdwidget.cpp \
    currentloginwidget.cpp \
    loginwidget.cpp \
    registerwidget.cpp \
    useraccountsystem.cpp

HEADERS += \
    useraccountsystem_global.h \
    accountquery.h \
    changepasswdwidget.h \
    currentloginwidget.h \
    loginwidget.h \
    registerwidget.h \
    useraccountsystem.h

QMAKE_TARGET_PRODUCT = User Account System
QMAKE_TARGET_DESCRIPTION = User Account Control System
