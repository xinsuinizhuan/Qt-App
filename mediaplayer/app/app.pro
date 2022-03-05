include(../../Common.pri)
include(../MediaPlayer.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = MediaPlayer

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
    -l$$replaceLibName(media) \
    -l$$replaceLibName(thirdparty)
	
RC_ICONS = app.ico
#ICON     = app.icns

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp

DESTDIR = $$APP_OUTPUT_PATH
