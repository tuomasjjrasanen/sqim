QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sqim
TEMPLATE = app

QMAKE_STRIP =

SOURCES +=\
    thumbnailwidget.cc \
    main.cc \
    mainwindow.cc

HEADERS  += \
    thumbnailwidget.hh \
    mainwindow.hh

FORMS    +=

isEmpty(PREFIX) {
    PREFIX = /usr/local
}

BINDIR=$$PREFIX/bin
LIBDIR=$$PREFIX/lib/sqim

DEFINES += SQIM_CACHE_SCRIPT=\\\"$$LIBDIR/sqim-cache-image-info\\\"

sqim.path = $$BINDIR
sqim.files = sqim

sqimscripts.path = $$LIBDIR
sqimscripts.files = sqim-cache-image-info

INSTALLS += sqim sqimscripts
