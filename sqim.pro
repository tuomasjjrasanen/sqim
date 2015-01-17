QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sqim
TEMPLATE = app

VERSION = 0.1.0

QMAKE_STRIP =

SOURCES +=\
    thumbnailview.cc \
    main.cc \
    mainwindow.cc \
    metadatawidget.cc \
    imagearea.cc \
    metadata.cc \
    common.cc \
    thumbnaildelegate.cc

HEADERS  += \
    thumbnailview.hh \
    mainwindow.hh \
    metadatawidget.hh \
    imagearea.hh \
    metadata.hh \
    common.hh \
    thumbnaildelegate.hh

FORMS    +=

isEmpty(PREFIX) {
    PREFIX = /usr/local
}

QMAKE_CXXFLAGS += -DVERSION=\'\"$$VERSION\"\'

BINDIR=$$PREFIX/bin
LIBDIR=$$PREFIX/lib/sqim

sqim.path = $$BINDIR
sqim.files = sqim

INSTALLS += sqim

LIBS += -lexiv2

RESOURCES += icons.qrc application.qrc
