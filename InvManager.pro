#-------------------------------------------------
#
# Project created by QtCreator 2014-01-31T17:35:18
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = InvManager
TEMPLATE = app

CONFIG += c++11

RC_FILE = appIco.rc

CONFIG(debug,debug|release) {
    DEFINES += DEBUG_MODE
}

INCLUDEPATH += "C:\Documents and Settings\shipping.FMMI\Desktop\InvManager\include"
DEPENDPATH += "C:\Documents and Settings\shipping.FMMI\Desktop\InvManager\include"

LIBS += "-LC:\Documents and Settings\shipping.FMMI\Desktop\InvManager\bin"
LIBS += "-lLepton"

SOURCES += main.cpp\
    mainwindow.cpp \
    adddialog.cpp \
    updatedialog.cpp \
    utility.cpp \
    movedialog.cpp \
    logindialog.cpp \
    itemselectdialog.cpp \
    database.cpp \
    admindialog.cpp \
    newlogindialog.cpp \
    addwarehousedialog.cpp

HEADERS  += mainwindow.h \
    adddialog.h \
    updatedialog.h \
    utility.h \
    movedialog.h \
    logindialog.h \
    itemselectdialog.h \
    structures.h \
    database.h \
    admindialog.h \
    newlogindialog.h \
    addwarehousedialog.h

FORMS    += mainwindow.ui \
    adddialog.ui \
    updatedialog.ui \
    movedialog.ui \
    logindialog.ui \
    itemselectdialog.ui \
    admindialog.ui \
    newlogindialog.ui \
    addwarehousedialog.ui

RESOURCES += \
    icons.qrc

DISTFILES += \
    appIco.rc
