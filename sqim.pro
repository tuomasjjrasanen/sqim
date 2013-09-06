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
    imageinfowidget.cc \
    imagewidget.cc

HEADERS  += \
    thumbnailview.hh \
    mainwindow.hh \
    imageinfowidget.hh \
    imagewidget.hh

FORMS    +=

isEmpty(PREFIX) {
    PREFIX = /usr/local
}

BINDIR=$$PREFIX/bin
LIBDIR=$$PREFIX/lib/sqim

DEFINES += SQIM_CMD_MAKE_THUMBNAIL=\\\"$$LIBDIR/make-thumbnail\\\"
DEFINES += SQIM_CMD_PARSE_DATETIME=\\\"$$LIBDIR/parse-datetime\\\"

sqim.path = $$BINDIR
sqim.files = sqim

sqimscripts.path = $$LIBDIR
sqimscripts.files = make-thumbnail parse-datetime

INSTALLS += sqim sqimscripts
