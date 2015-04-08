#-------------------------------------------------
#
# Project created by QtCreator 2014-01-31T17:35:18
#
#-------------------------------------------------

QT       += core gui sql
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = InvManager
TEMPLATE = app

QMAKE_CXXFLAGS+="-Wno-ignored-qualifiers -Wno-unused-parameter -Wno-unused-variable"

RC_FILE = appIco.rc

CONFIG(debug,debug|release) {
    DEFINES += DEBUG_MODE
}

INCLUDEPATH += "$$PWD\include"
DEPENDPATH += "$$PWD\include"

LIBS += "-L$$PWD\bin"
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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Lepton/build-Lepton-Desktop_Qt_5_4_0_MinGW_32bit-Release/release/ -lLepton
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Lepton/build-Lepton-Desktop_Qt_5_4_0_MinGW_32bit-Release/debug/ -lLepton

INCLUDEPATH += $$PWD/../../Lepton/Lepton/include
DEPENDPATH += $$PWD/../../Lepton/Lepton/include
