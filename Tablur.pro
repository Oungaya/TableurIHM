#-------------------------------------------------
#
# Project created by QtCreator 2016-12-08T09:29:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tablur
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    cell.cpp \
    spreadsheet.cpp \
    gotocelldialog.cpp \
    finddialog.cpp

HEADERS  += mainwindow.h \
    cell.h \
    spreadsheet.h \
    gotocelldialog.h \
    finddialog.h

FORMS    += mainwindow.ui \
    gotocelldialog.ui \
    finddialog.ui

RESOURCES += \
    images/images.qrc

DISTFILES += \
    images/copy.png \
    images/cut.png \
    images/find.png \
    images/gotocell.png \
    images/icon.png \
    images/new.png \
    images/open.png \
    images/paste.png \
    images/save.png
