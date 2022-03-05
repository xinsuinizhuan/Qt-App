include(../../../libs.pri)
include(mimetypes/mimetypes.pri)

QT += widgets sql concurrent

greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

DEFINES += TOOLS_LIBRARY
TARGET = $$replaceLibName(tools)

LIBS += -l$$replaceLibName(utils)

SOURCES += \
    benchmarker.cpp \
    categorysortfiltermodel.cpp \
    crcalgorithm.cpp \
    itemviews.cpp \
    qtcassert.cpp \
    stringutils.cpp \
    treemodel.cpp

HEADERS += \
    algorithm.h \
    benchmarker.h \
    categorysortfiltermodel.h \
    crcalgorithm.h \
    itemviews.h \
    qtcassert.h \
    stringutils.h \
    treemodel.h \
    tools_global.h

QMAKE_TARGET_PRODUCT = Utils
QMAKE_TARGET_DESCRIPTION = Utils Framework
