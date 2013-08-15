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

qpicman.path = /usr/local/bin
qpicman.files = qpicman qpicman-cache-image-info

INSTALLS += qpicman
