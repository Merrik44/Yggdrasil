#-------------------------------------------------
#
# Project created by QtCreator 2011-06-13T13:24:08
#
#-------------------------------------------------

QT       += core gui
QT += opengl

TARGET = TreeDrawGUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sketchwidget.cpp \
    branch.cpp \
    newbranch.cpp \
    treefilexml.cpp \
    thicknessvariation.cpp \
    change.cpp \
    changeaddbranch.cpp \
    changeremovebranch.cpp \
    changewidthvariation.cpp \
    anglevariation.cpp \
    changeanglevariation.cpp \
    changelengthvariation.cpp \
    lengthvariation.cpp \
    Display/operation.cpp \
    Display/qtreedisplaywidget.cpp \
    Display/treemodel.cpp \
    Display/vector.cpp \
    Display/point.cpp \
    Display/matrix.cpp \
    Display/displaybranch.cpp \
    changeaddsegmentedbranch.cpp \
    segmentedbranch.cpp \
    widthchanges.cpp \
    selectbox.cpp \
    branchplacement.cpp \
    optionsdialog.cpp \
    loadsketch.cpp

HEADERS  += mainwindow.h \
    sketchwidget.h \
    branch.h \
    newbranch.h \
    treefilexml.h \
    thicknessvariation.h \
    change.h \
    changeaddbranch.h \
    changeremovebranch.h \
    changewidthvariation.h \
    anglevariation.h \
    changeanglevariation.h \
    changelengthvariation.h \
    lengthvariation.h \
    Display/qtreedisplaywidget.h \
    Display/operation.h \
    Display/treemodel.h \
    Display/vector.h \
    Display/point.h \
    Display/matrix.h \
    Display/displaybranch.h \
    changeaddsegmentedbranch.h \
    segmentedbranch.h \
    widthchanges.h \
    selectbox.h \
    branchplacement.h \
    optionsdialog.h \
    ancestorwidthvariation.h \
    loadsketch.h

INCLUDEPATH += /QTreeWidget/

FORMS    += mainwindow.ui

OTHER_FILES += \
    ../Desktop/save.png \
    images/save.png

CONFIG += CONSOLE
