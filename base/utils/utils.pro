include(../../libs.pri)

QT += widgets sql concurrent

greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

DEFINES += UTILS_LIBRARY
TARGET = $$replaceLibName(utils)

SOURCES += \
    datbasesql.cpp \
    hostosinfo.cpp \
    languageconfig.cc \
    logasync.cpp \
    thread.cpp \
    utils.cpp

HEADERS += \
    datbasesql.h \
    hostosinfo.h \
    languageconfig.hpp \
    logasync.h \
    osspecificaspects.h \
    singleton.hpp \
    taskqueue.h \
    thread.h \
    utils_global.h \
    utils.h

QMAKE_TARGET_PRODUCT = Utils
QMAKE_TARGET_DESCRIPTION = Utils Framework
