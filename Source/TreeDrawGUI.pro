# -------------------------------------------------
# Project created by QtCreator 2011-06-13T13:24:08
# -------------------------------------------------
QT += core \
    gui
QT += opengl
TARGET = TreeDrawGUI/TreeDrawGUI
TEMPLATE = app
SOURCES += TreeDrawGUI/main.cpp \
    TreeDrawGUI/mainwindow.cpp \
    TreeDrawGUI/sketchwidget.cpp \
    TreeDrawGUI/branch.cpp \
    TreeDrawGUI/newbranch.cpp \
    TreeDrawGUI/treefilexml.cpp \
    TreeDrawGUI/thicknessvariation.cpp \
    TreeDrawGUI/change.cpp \
    TreeDrawGUI/changeaddbranch.cpp \
    TreeDrawGUI/changeremovebranch.cpp \
    TreeDrawGUI/changewidthvariation.cpp \
    TreeDrawGUI/anglevariation.cpp \
    TreeDrawGUI/changeanglevariation.cpp \
    TreeDrawGUI/changelengthvariation.cpp \
    TreeDrawGUI/lengthvariation.cpp \
    TreeDrawGUI/Display/operation.cpp \
    TreeDrawGUI/Display/qtreedisplaywidget.cpp \
    TreeDrawGUI/Display/treemodel.cpp \
    TreeDrawGUI/Display/vector.cpp \
    TreeDrawGUI/Display/point.cpp \
    TreeDrawGUI/Display/matrix.cpp \
    TreeDrawGUI/Display/displaybranch.cpp \
    TreeDrawGUI/changeaddsegmentedbranch.cpp \
    TreeDrawGUI/segmentedbranch.cpp \
    TreeDrawGUI/widthchanges.cpp \
    TreeDrawGUI/selectbox.cpp \
    TreeDrawGUI/branchplacement.cpp \
    TreeDrawGUI/optionsdialog.cpp \
    TreeDrawGUI/loadsketch.cpp \
    MeshGenerator/vmath.cpp \
    MeshGenerator/surfacesubdivision.cpp \
    MeshGenerator/objexporter.cpp \
    MeshGenerator/mesh.cpp \
    MeshGenerator/meshgenerator.cpp \
    MeshGenerator/lstskeleton.cpp \
    MeshGenerator/jointgenerator.cpp \
    MeshGenerator/debug.cpp \
    TextureSynthesis/vector2.cpp \
    TextureSynthesis/texturesynthesis.cpp \
    TextureSynthesis/texture.cpp \
    TextureSynthesis/pixelcoherence.cpp \
    TextureSynthesis/indexedtexture.cpp \
    TextureSynthesis/coherenttexture.cpp \
    TextureSynthesis/texturesynthesisdialog.cpp
HEADERS += TreeDrawGUI/mainwindow.h \
    TreeDrawGUI/sketchwidget.h \
    TreeDrawGUI/branch.h \
    TreeDrawGUI/newbranch.h \
    TreeDrawGUI/treefilexml.h \
    TreeDrawGUI/thicknessvariation.h \
    TreeDrawGUI/change.h \
    TreeDrawGUI/changeaddbranch.h \
    TreeDrawGUI/changeremovebranch.h \
    TreeDrawGUI/changewidthvariation.h \
    TreeDrawGUI/anglevariation.h \
    TreeDrawGUI/changeanglevariation.h \
    TreeDrawGUI/changelengthvariation.h \
    TreeDrawGUI/lengthvariation.h \
    TreeDrawGUI/Display/qtreedisplaywidget.h \
    TreeDrawGUI/Display/operation.h \
    TreeDrawGUI/Display/treemodel.h \
    TreeDrawGUI/Display/vector.h \
    TreeDrawGUI/Display/point.h \
    TreeDrawGUI/Display/matrix.h \
    TreeDrawGUI/Display/displaybranch.h \
    TreeDrawGUI/changeaddsegmentedbranch.h \
    TreeDrawGUI/segmentedbranch.h \
    TreeDrawGUI/widthchanges.h \
    TreeDrawGUI/selectbox.h \
    TreeDrawGUI/branchplacement.h \
    TreeDrawGUI/optionsdialog.h \
    TreeDrawGUI/ancestorwidthvariation.h \
    TreeDrawGUI/loadsketch.h \
    TreeDrawGUI/ui_mainwindow.h \
    MeshGenerator/vmath.h \
    MeshGenerator/surfacesubdivision.h \
    MeshGenerator/objexporter.h \
    MeshGenerator/mesh.h \
    MeshGenerator/meshgenerator.h \
    MeshGenerator/lstskeleton.h \
    MeshGenerator/jointgenerator.h \
    MeshGenerator/debug.h \
    MeshGenerator/branchnode.h \
    TextureSynthesis/vector2.h \
    TextureSynthesis/texturesynthesis.h \
    TextureSynthesis/texture.h \
    TextureSynthesis/pixelcoherence.h \
    TextureSynthesis/indexedtexture.h \
    TextureSynthesis/coherenttexture.h \
    TextureSynthesis/texturesynthesisdialog.h \\\

INCLUDEPATH += /QTreeWidget/
INCLUDEPATH += /TreeDrawGUI/
FORMS += TreeDrawGUI/mainwindow.ui \
    TextureSynthesis/texturesynthesisdialog.ui
OTHER_FILES += ../Desktop/save.png \
    images/save.png
CONFIG += CONSOLE
