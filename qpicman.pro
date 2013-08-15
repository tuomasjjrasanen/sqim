QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qpicman
TEMPLATE = app

QMAKE_STRIP =

SOURCES +=\
    imageview.cc \
    main.cc \
    mainwindow.cc

HEADERS  += \
    imageview.hh \
    mainwindow.hh

FORMS    +=

isEmpty(PREFIX) {
    PREFIX = /usr/local
}

BINDIR=$$PREFIX/bin
LIBDIR=$$PREFIX/lib/qpicman

DEFINES += QPICMAN_CACHE_SCRIPT=\\\"$$LIBDIR/qpicman-cache-image-info\\\"

qpicman.path = $$BINDIR
qpicman.files = qpicman

qpicmanscripts.path = $$LIBDIR
qpicmanscripts.files = qpicman-cache-image-info

INSTALLS += qpicman qpicmanscripts
