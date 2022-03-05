include(../../libs.pri)

QT += widgets multimediawidgets multimedia

DEFINES += MEDIA_LIBRARY
TARGET = $$replaceLibName(media)

LIBS += -l$$replaceLibName(utils)

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    mainwindow.cpp \
    mediawidget.cpp \
    playcontrolwidget.cpp \
    playerwidget.cpp \
    playlistmodel.cpp \
    playlistview.cpp \
    qmediaplaylist.cpp \
    qplaylistfileparser.cpp

HEADERS += \
    mainwindow.h \
    media_global.h \
    mediawidget.h \
    playcontrolwidget.h \
    playerwidget.h \
    playlistmodel.h \
    playlistview.h \
    qmediaplaylist.h \
    qmediaplaylist_p.h \
    qplaylistfileparser_p.h
