include(../../../libs.pri)
include(../../PluginTemplate.pri)

QT += widgets

greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

DEFINES += EXTENSIONSYSTEM_LIBRARY
TARGET = $$replaceLibName(extensionsystem)

LIBS += \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(tools) \
    -l$$replaceLibName(aggregation) \

HEADERS += \
    pluginerrorview.h \
    plugindetailsview.h \
    invoker.h \
    iplugin.h \
    iplugin_p.h \
    extensionsystem_global.h \
    pluginmanager.h \
    pluginmanager_p.h \
    pluginspec.h \
    pluginspec_p.h \
    pluginview.h \
    optionsparser.h \
    pluginerroroverview.h

SOURCES += \
    pluginerrorview.cpp \
    plugindetailsview.cpp \
    invoker.cpp \
    iplugin.cpp \
    pluginmanager.cpp \
    pluginspec.cpp \
    pluginview.cpp \
    optionsparser.cpp \
    pluginerroroverview.cpp

FORMS += \
    pluginerrorview.ui \
    plugindetailsview.ui \
    pluginerroroverview.ui

RESOURCES += \
    images.qrc

QMAKE_TARGET_PRODUCT = Extensionsystem
QMAKE_TARGET_DESCRIPTION = Plug-in System
