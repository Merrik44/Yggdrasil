#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sketchwidget.h"
#include "TreeDrawGUI/Display/qtreedisplaywidget.h"
#include <string>

QT_BEGIN_NAMESPACE
class QAction;
//class QListWidget;
//class QMenu;
//class QTextEdit;
QT_END_NAMESPACE

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //explicit MainWindow(QWidget *parent = 0);
    MainWindow();
    ~MainWindow();

public slots:
    void sketchChanged();
    void sketchNonEmpty();
    void sketchEmpty();

private slots:
    void openTreeFromXML();
    void saveTreeToXML();
    void clearSketch();
    void undoAction();
    void redoAction();
    void displayXMLFile();
    void displayLSTFile();

    void generateFromXML();
    void generateFromCurrent();
    void generateNewVariation();
    void generationOptions();
    void optionsAccepted(int v1, int v2, int v3, int v4, int v5, int subdValue);
    void generateMeshFromLST( std::string lstfile);
    void exportToOBJ();
    void displayAsCylinders();
    void displayAsMesh();
    void SynthesizeTexture();

    void blackBackground();
    void whiteBackground();

    void setLineMode();
    void setSelectMode();
    void setPencilMode();

    void setBrushSize(const QString &text);

    void cancelGeneration();

private:
    Ui::MainWindow *ui;
    void connectActions();
    void createActions();
    //void createMenus();
    void createToolBars();
    //void createStatusBar();
    void setupWidgets();

    SketchWidget *sketchWidget;
    QTreeDisplayWidget *displayWidget;
    QToolBar *sketchToolBar;
    QToolBar *displayToolBar;
    QAction *generate;
    QAction *generationOption;
    QAction *undo;
    QAction *redo;
    QAction *newVariation;
    QCheckBox *generateMesh;
    QAction *newSketch;
    QAction *displayMesh;
    QAction *texSynthOption;
    QAction *displayCylinderForm;

    //QAction *newSketch;

   // QCheckBox *pencilMode;
    QToolButton *lineMode;
    QToolButton *selectMode;
    QToolButton *pencilMode;

    QLabel *brushLabel;
    QComboBox *brushSize;

    std::string lastGeneratedFile;
    std::string lastLSTFile;

    int optionD, optionB, optionP, textureIndex, storeRoot, subdivs;
    void errorMessage(const char* msg, const char* title);
    QStringList acceptedImageFormats;

    bool sketchWidgetCreated;

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // MAINWINDOW_H
