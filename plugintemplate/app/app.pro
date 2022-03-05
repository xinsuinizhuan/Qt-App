include(../../Common.pri)
include(../PluginTemplate.pri)

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = AppPlugin

win32 {
LIBS += -L$$APP_OUTPUT_PATH/../libs
}

unix {
LIBS += -L$$APP_OUTPUT_PATH
}

LIBS += \
    -l$$replaceLibName(utils) \
    -l$$replaceLibName(controls) \
    -l$$replaceLibName(crashhandler) \
    -l$$replaceLibName(extensionsystem) \
    -l$$replaceLibName(useraccountsystem) \
    -l$$replaceLibName(thirdparty)

RC_ICONS = app.ico
ICON     = app.icns

SOURCES += \
    main.cpp

OTHER_FILES += \
    app.ico \
    app.icns

DESTDIR = $$APP_OUTPUT_PATH

QMAKE_TARGET_PRODUCT = AppPlugin
QMAKE_TARGET_DESCRIPTION = AppPlugin
