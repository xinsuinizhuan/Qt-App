include(../plugins.pri)

QT += widgets network

greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

DEFINES += TCPTOOLPLUGIN_LIBRARY
TARGET = $$replaceLibName(tcptoolplugin)

LIBS += \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(extensionsystem) \
    -l$$replaceLibName(controls) \
    -l$$replaceLibName(core) \

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    tcpclient.cpp \
    tcpclientthread.cpp \
    tcppage.cpp \
    tcpserver.cpp \
    tcpserverthread.cpp \
    tcptoolplugin.cpp \
    tcpwidget.cpp

HEADERS += \
    tcpclient.h \
    tcpclientthread.h \
    tcppage.h \
    tcpserver.h \
    tcpserverthread.h \
    tcptoolplugin.h \
    tcptoolplugin_global.h \
    tcpwidget.h

OTHER_FILES += tcptoolplugin.json

QMAKE_TARGET_PRODUCT = Tcp Tool Plugin
QMAKE_TARGET_DESCRIPTION = Tcp Tool

