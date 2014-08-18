QT       += core gui

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
    common.cc

HEADERS  += \
    thumbnailview.hh \
    mainwindow.hh \
    metadatawidget.hh \
    imagearea.hh \
    metadata.hh \
    common.hh

FORMS    +=

isEmpty(PREFIX) {
    PREFIX = /usr/local
}

BINDIR=$$PREFIX/bin
LIBDIR=$$PREFIX/lib/sqim

sqim.path = $$BINDIR
sqim.files = sqim

INSTALLS += sqim

LIBS += -lexiv2
