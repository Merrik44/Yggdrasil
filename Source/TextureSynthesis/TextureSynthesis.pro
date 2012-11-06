#-------------------------------------------------
#
# Project created by QtCreator 2012-07-01T13:00:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TextureSynthesis
TEMPLATE = app

QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS *= -fopenmp

SOURCES += main.cpp\
        mainwindow.cpp \
    texture.cpp \
    vector2.cpp \
    pixelcoherence.cpp \
    coherenttexture.cpp \
    indexedtexture.cpp \
    displaylabelthread.cpp \
    texturesynthesis.cpp \
    texturesynthesisdialog.cpp

HEADERS  += mainwindow.h \
    texture.h \
    vector2.h \
    pixelcoherence.h \
    coherenttexture.h \
    indexedtexture.h \
    displaylabelthread.h \
    texturesynthesis.h \
    texturesynthesisdialog.h
QT += opengl 
FORMS    += mainwindow.ui \
    texturesynthesisdialog.ui
