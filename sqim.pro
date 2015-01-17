QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sqim
TEMPLATE = app

VERSION = 0.1.0

QMAKE_STRIP =

SOURCES +=\
    imagelistview.cc \
    main.cc \
    mainwindow.cc \
    metadatawidget.cc \
    imageview.cc \
    metadata.cc \
    common.cc \
    imageitemdelegate.cc

HEADERS  += \
    imagelistview.hh \
    mainwindow.hh \
    metadatawidget.hh \
    imageview.hh \
    metadata.hh \
    common.hh \
    imageitemdelegate.hh

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
