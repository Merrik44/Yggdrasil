#-------------------------------------------------
#
# Project created by QtCreator 2011-07-19T09:57:41
#
#-------------------------------------------------

QT       += core gui
QT += opengl

TARGET = TreeDisplay
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    operation.cpp \
    operation.cpp \
    qtreedisplaywidget.cpp

HEADERS  += mainwindow.h \
    operation.h \
    qtreedisplaywidget.h

INCLUDEPATH += \include\rapidxml-1.13

FORMS    += mainwindow.ui
