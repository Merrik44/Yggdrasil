#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <QProgressDialog>
#include <QKeySequence>
#include "optionsdialog.h"


const int BRUSH_SIZE_COUNT = 10;
const int BRUSH_SIZE_OPTION[] = { 1, 3, 5, 7, 9, 11, 15, 20, 25, 30 };


MainWindow::MainWindow()
    : ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //QDir::setCurrent(QApplication::applicationDirPath());
    sketchWidgetCreated = false;


    lastGeneratedFile = "";
    optionD = 2;
    optionB = 500;
    optionP = -1;
    textureIndex = 37;
    storeRoot = 1;
    subdivs = 0;

    connectActions();

    createActions();
    createToolBars();
    setupWidgets();
    this->releaseKeyboard();
    setWindowTitle(tr("TreeDraw"));
    setLineMode();

    brushSize->setCurrentIndex(2);

    acceptedImageFormats.append("*.jpg");
    acceptedImageFormats.append("*.png");
    acceptedImageFormats.append("*.pgm");
    acceptedImageFormats.append("*.raw");


    QDir directory("./Resources/Textures/");
    QStringList textureList = directory.entryList(acceptedImageFormats);

    //  cout << "texture " << textureIndex << endl;
    // displayWidget->setTexture("./Resources/Textures/blank.jpg");
    if (textureList.size() > textureIndex && textureIndex >= 0)
    {
        displayWidget->setTexture("./Resources/Textures/" + textureList[textureIndex]);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
        sketchWidget->deleteKeyPressed();
    else if (event->key() == Qt::Key_M)
        sketchWidget->toggleMode();
    else if (event->key() == Qt::Key_Control)
        sketchWidget->ctrlPressed();
    else if (event->key() == Qt::Key_Escape)
        sketchWidget->deselectAll();
    else if (event->key() == Qt::Key_A)
        sketchWidget->selectAll();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control)
        sketchWidget->ctrlReleased();
}

MainWindow::~MainWindow()
{
    //delete ui;
}

void MainWindow::openTreeFromXML()
{
    QString path = QFileDialog::getOpenFileName(
                this,
                "XML File",
                QDir::currentPath(),
                "XML file (*.xml)");

    if (!path.isNull())
    {
        //code to open tree sketch here
        sketchWidget->loadSketch(path);
    }
}

void MainWindow::saveTreeToXML()
{
    QString path = QFileDialog::getSaveFileName(
                this,
                "XML",
                QDir::currentPath(),
                "XML file (*.xml)");

    if (!path.isNull())
    {
        //you will want to modify this to take a file path
        //and pass the path to it
        if (path.length() > 4)
        {
            if (!(path[path.length() - 4] == '.' && path[path.length() - 3] == 'x' && path[path.length() - 2] == 'm' && path[path.length() - 1] == 'l'))
                path.append(".xml");
        }
        else
            path.append(".xml");
        sketchWidget->writeToXMLFile(path);
    }
}

void MainWindow::clearSketch()
{

    if (QMessageBox::question(this, "New sketch",
                              "Starting a new sketch will erase your current sketch. Are you sure you want to continue?",
                              QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton) == QMessageBox::Yes)
    {
        sketchWidget->newSketch();
    }
}

void MainWindow::undoAction()
{
    sketchWidget->undoAction();
}

void MainWindow::redoAction()
{
    sketchWidget->redoAction();
}

void MainWindow::displayXMLFile()
{
    QString path = QFileDialog::getOpenFileName(
                this,
                "XML File",
                QDir::currentPath(),
                "XML file (*.xml)");

    if (!path.isNull())
    {
        displayWidget->setSourceXMLFile(path.toStdString());
        displayWidget->repaint();
        displayWidget->updateGL();
    }
}

void MainWindow::displayLSTFile()
{
    QString path = QFileDialog::getOpenFileName(
                this,
                "LST File",
                QDir::currentPath(),
                "LST file (*.lst)");

    if (!path.isNull())
    {

        lastLSTFile = path.toStdString();
        displayWidget->setSourceFile(path.toStdString());
        displayWidget->setCurrent();
        displayWidget->repaint();
        displayWidget->updateGL();
    }
}

void MainWindow::generateFromXML()
{
    QString path = QFileDialog::getOpenFileName(
                this,
                "XML File",
                QDir::currentPath(),
                "XML file (*.xml)");

    if (!path.isNull())
    {
        //generation code here
        //displayWidget->setSourceFile("treefile.lst");
        //displayWidget->repaint();
    }
}


void MainWindow::displayAsCylinders()
{
    //   cout << "SS " << endl;
    displayWidget->displayGeneratedMesh = false;
    displayCylinderForm->setEnabled(false);

    SubdivSlider->setEnabled(false);
    displayMesh->setEnabled(true);
    displayWidget->repaint();
    displayWidget->updateGL();
}

void MainWindow::displayAsMesh()
{
    displayWidget->displayGeneratedMesh = true;
    // --------------- Generate the mesh ---------------------------------
    generateMeshFromLST(lastLSTFile);

    displayMesh->setEnabled(false);
    displayCylinderForm->setEnabled(true);
    SubdivSlider->setEnabled(true);
    displayWidget->repaint();
    displayWidget->updateGL();
}

void MainWindow::SubdivSliderChange( int value )
{
    subdivs = value;
    QProgressDialog subdivisionProgbar(this);
    subdivisionProgbar.setRange(0, 100);
    subdivisionProgbar.setModal(true);
    subdivisionProgbar.show();
    subdivisionProgbar.setCursor(QCursor(Qt::ArrowCursor));
    subdivisionProgbar.setLabelText("Applying Loop Sudivision");
    subdivisionProgbar.setValue(0);
    displayWidget->ApplySubdivisionToMesh( subdivs, &subdivisionProgbar);

}


void MainWindow::SynthesizeTexture()
{
    QString loadpath = QFileDialog::getOpenFileName( this, "",QDir::currentPath(),  "");
    QString savePath = "/Resources/Textures";
    cout << "Synthesize sample " <<  loadpath.toStdString() <<endl;
}

void MainWindow::cancelGeneration()
{
    setCursor(QCursor(Qt::ArrowCursor));
}

void MainWindow::generateFromCurrent()
{
    // This is temporary
    string basefilename = "treefile";
    string filename;
    int count = 0;

    QDir directory("./Resources/Textures/");
    QStringList textureList = directory.entryList(acceptedImageFormats);

    if (textureList.size() > textureIndex && textureIndex >= 0)
    {
        displayWidget->setTexture("./Resources/Textures/" + textureList[textureIndex]);
    }
    stringstream ss;

    displayWidget->clearDisplay();

    QProgressDialog p(this);
    //p.setCancelButton(NULL);
    p.setRange(0, 100);
    p.setModal(true);
    p.show();
    setCursor(QCursor(Qt::WaitCursor));
    p.setCursor(QCursor(Qt::ArrowCursor));

    connect(&p, SIGNAL(canceled()), this, SLOT(cancelGeneration()));

    p.setValue(0);
    p.setLabelText("Finding new temporary file location");
    QCoreApplication::processEvents();
    if (p.wasCanceled()) return;

    bool exists;
    do {
        ss.str("");
        ss << basefilename << count;
        filename = ss.str();
        ss << ".xml";
        ifstream file(ss.str().c_str());
        exists = file;
        file.close();
        ++count;
    } while (exists);

    p.setValue(5);
    p.setLabelText("Saving sketch to temporary file");
    QCoreApplication::processEvents();
    if (p.wasCanceled()) return;
    ss.str(""); ss << filename << ".xml";
    //error checking here?
    sketchWidget->writeToXMLFile(ss.str().c_str());


    p.setValue(10);
    p.setLabelText("Converting sketch to 3D");
    QCoreApplication::processEvents();
    if (p.wasCanceled()) return;
    ss.str(""); ss << "cat " << filename << ".xml | ./SketchConverter/Debug/tree3d > " << filename << ".3d.xml";
    if (system(ss.str().c_str()) != 0)
    {
        errorMessage("An error was encountered during the 2D to 3D conversion process.", "Could not generate tree");
        cancelGeneration();
        return;
    }

    p.setValue(30);
    p.setLabelText("Creating L-system definition");
    QCoreApplication::processEvents();
    if (p.wasCanceled()) return;
    ss.str(""); ss << "cat " << filename << ".3d.xml | ./LSystemParameteriser/Debug/lsparm ";
    if (optionD > 0)
        ss << "-d " << optionD << " ";
    if (optionB > 0)
        ss << "-b " << optionB << " ";
    if (optionP > 0)
        ss << "-p " << optionP << " ";
    ss << "--storeroot ";
    if (storeRoot != 0)
        ss << "1 ";
    else
        ss << "0 ";
    ss << "> " << filename << ".lpfg";
    if (system(ss.str().c_str()) != 0)
    {
        errorMessage("An error was encountered during the L-system parameterisation process.", "Could not generate tree");
        cancelGeneration();
        return;
    }

    p.setValue(50);
    p.setLabelText("Converting L-system definition to source code");
    QCoreApplication::processEvents();
    if (p.wasCanceled()) return;
    ss.str(""); ss << "cat " << filename << ".lpfg | ./LStringDeriver/Debug/lpfgtocpp > " << filename << ".cpp";
    if (system(ss.str().c_str()) != 0)
    {
        errorMessage("An error was encountered during the LPFG conversion process.", "Could not generate tree");
        cancelGeneration();
        return;
    }

    p.setValue(70);
    p.setLabelText("Compiling tree generator");
    QCoreApplication::processEvents();
    if (p.wasCanceled()) return;
    ss.str(""); ss << "cat " << filename << ".cpp | g++ -pipe -xc++ - -O3 -o " << filename;
    if (system(ss.str().c_str()) != 0)
    {
        errorMessage("An error was encountered while compiling the tree variation generator.", "Could not generate tree");
        cancelGeneration();
        return;
    }

    p.setValue(90);
    p.setLabelText("Generating tree model");
    QCoreApplication::processEvents();
    if (p.wasCanceled()) return;
    ss.str(""); ss << "./" << filename << " > " << filename << ".lst";
    if (system(ss.str().c_str()) != 0)
    {
        errorMessage("An error was encountered when running the tree generation executable.", "Could not generate tree");
        cancelGeneration();
        return;
    }

    p.setValue(95);
    p.setLabelText("Rendering tree model in display");
    QCoreApplication::processEvents();
    if (p.wasCanceled()) return;
    ss.str(""); ss << filename << ".lst";
    lastLSTFile = ss.str();
    if (!displayWidget->setSourceFile(ss.str()))
    {
        errorMessage("An error was encountered when rendering the generated tree.", "Tree could not be displayed");
        cancelGeneration();
        return;
    }


    p.setValue(100);
    p.setLabelText("Generation complete");
    QCoreApplication::processEvents();


    displayWidget->setCurrent();

    displayWidget->repaint();
    displayWidget->updateGL();

    lastGeneratedFile = filename;

    setCursor(QCursor(Qt::ArrowCursor));
    newVariation->setEnabled(true);
    displayAsCylinders();
    // displayCylinderForm->setEnabled(true);
    //   displayCylinderForm->setEnabled(true);
    // generateMesh->setEnabled(true);

}


void MainWindow::generateMeshFromLST( std::string lstfile)
{

    if(lstfile == ""  )
        return;
    cout << "ff " << lstfile << endl;

    //  stringstream ss;
    // ss << lstfile;
    // ss << ".lst";
    //lstfile += lst;

    QProgressDialog progbar(this);
    //p.setCancelButton(NULL);
    progbar.setRange(0, 100);
    progbar.setModal(true);
    progbar.show();
    setCursor(QCursor(Qt::WaitCursor));
    progbar.setCursor(QCursor(Qt::ArrowCursor));

    progbar.setValue(0);
    progbar.setLabelText("Parsing LST File");

    displayWidget->LoadLST(lstfile );

    progbar.setValue(99);
    progbar.setLabelText("Generating Mesh");


    displayWidget->GenerateMeshFromLST(&progbar);
    progbar.setValue(99);

    progbar.setVisible(false);

    QProgressDialog subdivisionProgbar(this);
    subdivisionProgbar.setRange(0, 100);
    subdivisionProgbar.setModal(true);
    subdivisionProgbar.show();
    subdivisionProgbar.setCursor(QCursor(Qt::ArrowCursor));
    subdivisionProgbar.setLabelText("Applying Loop Sudivision");
    subdivisionProgbar.setValue(0);

    displayWidget->ApplySubdivisionToMesh(subdivs, &subdivisionProgbar);
    subdivisionProgbar.setValue(99);
}

void MainWindow::generateNewVariation()
{// This is also temporary
    string basefilename = "treefile";
    string filename;
    int count = 0;


    QDir directory("./Resources/Textures/");
    QStringList textureList = directory.entryList(acceptedImageFormats);

    if (textureList.size() > textureIndex && textureIndex >= 0)
    {
        displayWidget->setTexture("./Resources/Textures/" + textureList[textureIndex]);
    }

    stringstream ss;

    if (lastGeneratedFile != "")
    {
        filename = lastGeneratedFile;
    }
    else
    {
        errorMessage("You have not generated a tree this session", "Cannot generate new variation");
        return;
    }

    displayWidget->clearDisplay();

    QProgressDialog p(this);
    //p.setCancelButton(NULL);
    p.setRange(0, 100);
    p.show();
    setCursor(QCursor(Qt::WaitCursor));
    p.setCursor(QCursor(Qt::ArrowCursor));

    connect(&p, SIGNAL(canceled()), this, SLOT(cancelGeneration()));

    p.setValue(0);
    p.setLabelText("Generating tree model");
    QCoreApplication::processEvents();
    if (p.wasCanceled()) return;
    ss.str(""); ss << "./" << filename << " > " << filename << ".lst";
    if (system(ss.str().c_str()) != 0)
    {
        errorMessage("An error was encountered when running the tree generation executable.", "Could not generate tree");
        cancelGeneration();
        return;
    }

    p.setValue(50);
    p.setLabelText("Rendering tree model in display");
    QCoreApplication::processEvents();
    if (p.wasCanceled()) return;
    ss.str(""); ss << filename << ".lst";
    if (!displayWidget->setSourceFile(ss.str()))
    {
        errorMessage("An error was encountered when rendering the generated tree.", "Tree could not be displayed");
        cancelGeneration();
        return;
    }


    p.setValue(100);
    p.setLabelText("Generation complete");
    QCoreApplication::processEvents();

    displayWidget->repaint();
    displayWidget->updateGL();
}


void MainWindow::generationOptions()
{
    OptionsDialog options(this);

    options.setValues(optionD, optionB, optionP, storeRoot, textureIndex, subdivs);

    connect(&options, SIGNAL(valuesAccepted(int,int,int,int,int,int)), this, SLOT(optionsAccepted(int,int,int,int,int,int)));

    options.exec();

}

void MainWindow::optionsAccepted(int v1, int v2, int v3, int v4, int v5, int subdValue)
{
    optionD = v1;
    optionB = v2;
    optionP = v3;
    if (v4 != 0)
        storeRoot = 1;
    else
        storeRoot = 0;
    textureIndex = v5;

    QDir directory("./Resources/Textures/");
    QStringList textureList = directory.entryList(acceptedImageFormats);

    if (textureList.size() > textureIndex && textureIndex >= 0)
        displayWidget->setTexture("./Resources/Textures/" + textureList[textureIndex]);


    subdivs =subdValue;
}


void MainWindow::exportCylindesAsOBJ()
{
    QString path = QFileDialog::getSaveFileName(
                this,
                "OBJ",
                QDir::currentPath(),
                "OBJ file (*.obj)");


    // export the mesh that is in the viewport
    if(displayMesh->isEnabled())
        displayWidget->exportToObj(path.toStdString());
    else
        displayWidget->exportMeshToObj(path.toStdString());



}

void MainWindow::exportMeshAsOBJ()
{
    QString path = QFileDialog::getSaveFileName(
                this,
                "OBJ",
                QDir::currentPath(),
                "OBJ file (*.obj)");

    cout << "!!!!" << endl;







}


void MainWindow::blackBackground()
{
    ui->actionWhite_background->setChecked(false);
    ui->actionBlack_background->setChecked(true);
    displayWidget->setBackgroundColour(0);
    displayWidget->repaint();
    displayWidget->updateGL();
}

void MainWindow::whiteBackground()
{
    ui->actionBlack_background->setChecked(false);
    ui->actionWhite_background->setChecked(true);
    displayWidget->setBackgroundColour(1);
    displayWidget->repaint();
    displayWidget->updateGL();
}


void MainWindow::setLineMode()
{
    sketchWidget->setLineMode();
    sketchWidget->setCursor(QCursor(QPixmap("./Resources/Cursors/Precision.png"),16,16));
}

void MainWindow::setSelectMode()
{
    sketchWidget->setSelectMode();
    sketchWidget->setCursor(QCursor(QPixmap("./Resources/Cursors/Precision.png"),16,16));
}

void MainWindow::setPencilMode()
{
    sketchWidget->setPencilMode();
    sketchWidget->setCursor(QCursor(QPixmap("./Resources/Cursors/Handwrite.png"),0,32));
}


void MainWindow::setBrushSize(const QString &text)
{
    /*if (sketchWidget != 0)
        sketchWidget->setInitialWidth(max(1, brushSize->currentText().toInt()));*/
    if (sketchWidgetCreated)
    {
        sketchWidget->setInitialWidth(max(1, brushSize->currentText().toInt()));
        sketchWidget->setFocus();
    }
}


void MainWindow::sketchChanged()
{
    //newVariation->setEnabled(false);
    displayWidget->setOutOfDate();
}

void MainWindow::sketchNonEmpty()
{
    generate->setEnabled(true);
}

void MainWindow::sketchEmpty()
{
    generate->setEnabled(false);
}


void MainWindow::connectActions()
{
    ui->actionSave_XML_file->setVisible(true);
    ui->actionOpen_XML_file->setVisible(true);

    connect(ui->actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(ui->actionOpen_XML_file, SIGNAL(triggered()), this, SLOT(openTreeFromXML()));
    ui->actionOpen_XML_file->setShortcuts(QKeySequence::Open);
    ui->actionOpen_XML_file->setStatusTip(tr("Open a sketch from an XML file"));

    connect(ui->actionSave_XML_file, SIGNAL(triggered()), this, SLOT(saveTreeToXML()));
    ui->actionSave_XML_file->setShortcuts(QKeySequence::Save);
    ui->actionSave_XML_file->setStatusTip(tr("Save your current sketch to an XML file"));

    /*connect(ui->actionGenerate_output_for_XML_file, SIGNAL(triggered()), this, SLOT(generateFromXML()));
    ui->actionGenerate_output_for_XML_file->setStatusTip(tr("Generate rendered output for an XML file"));*/

    connect(ui->actionDisplay_XML_file, SIGNAL(triggered()), this, SLOT(displayXMLFile()));
    ui->actionDisplay_XML_file->setStatusTip(tr("Display an XML file"));

    connect(ui->actionDisplay_LST_file, SIGNAL(triggered()), this, SLOT(displayLSTFile()));
    ui->actionDisplay_LST_file->setStatusTip(tr("Display an LST file"));

    connect(ui->actionGenerate_from_current, SIGNAL(triggered()), this, SLOT(generateFromCurrent()));
    ui->actionGenerate_from_current->setStatusTip(tr("Generate rendered output from the current sketch"));

    connect(ui->actionExport_Cylinder_Model_to_OBJ, SIGNAL(triggered()), this, SLOT(exportCylindesAsOBJ()));
    ui->actionExport_Cylinder_Model_to_OBJ->setStatusTip(tr("Export current view to OBJ file"));

    connect(ui->actionBlack_background, SIGNAL(triggered()), this, SLOT(blackBackground()));
    ui->actionBlack_background->setStatusTip(tr("Set tree display background to black"));

    connect(ui->actionWhite_background, SIGNAL(triggered()), this, SLOT(whiteBackground()));
    ui->actionWhite_background->setStatusTip(tr("Set tree display background to white"));



    connect(ui->actionUndo, SIGNAL(triggered()), this, SLOT(undoAction()));
    ui->actionUndo->setShortcuts(QKeySequence::Undo);
    ui->actionUndo->setStatusTip(tr("Undo your last action"));

    connect(ui->actionRedo, SIGNAL(triggered()), this, SLOT(redoAction()));
    ui->actionRedo->setShortcuts(QKeySequence::Redo);
    ui->actionRedo->setStatusTip(tr("Redo your last undone action"));

}

void MainWindow::createActions()
{
    //saveToXML = new QAction(QIcon(":/images/save.png"), tr("&Save..."), this);
    //saveToXML = new QAction(("&Save..."), this);
    //saveToXML->setShortcuts(QKeySequence::Save);
    //saveToXML->setStatusTip(tr("Save your current sketch to an XML file"));
    //saveToXML->setText("Save");
    //connect(saveToXML, SIGNAL(triggered()), this, SLOT(saveTreeToXML()));

    newSketch = new QAction(("&New"), this);
    newSketch->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    newSketch->setStatusTip(tr("Start a new sketch"));
    connect(newSketch, SIGNAL(triggered()), this, SLOT(clearSketch()));

    generate = new QAction(("&Generate"), this);
    generate->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
    generate->setStatusTip(tr("Generate a tree from your current sketch"));
    connect(generate, SIGNAL(triggered()), this, SLOT(generateFromCurrent()));
    generate->setEnabled(false);

    generationOption = new QAction(("&Options"), this);
    generationOption->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    generationOption->setStatusTip(tr("Choose settings for the tree generation process"));
    connect(generationOption, SIGNAL(triggered()), this, SLOT(generationOptions()));

    newVariation = new QAction(("&New Variation"), this);
    //newVariation->setShortcuts(QKeySequence(Qt::CTRL + Qt::Key_G));
    newVariation->setStatusTip(tr("Generate a new variation of the last type of tree you generated"));
    connect(newVariation, SIGNAL(triggered()), this, SLOT(generateNewVariation()));
    newVariation->setEnabled(false);


    displayMesh = new QAction(("&Display Mesh Model"), this);
    //newVariation->setShortcuts(QKeySequence(Qt::CTRL + Qt::Key_G));
    displayMesh->setStatusTip(tr("Generate a new variation of the last type of tree you generated"));
    connect(displayMesh, SIGNAL(triggered()), this, SLOT(displayAsMesh()));
    displayMesh->setEnabled(false);


    displayCylinderForm = new QAction(("&Display Cylinder Model"), this);
    //newVariation->setShortcuts(QKeySequence(Qt::CTRL + Qt::Key_G));
    displayCylinderForm->setStatusTip(tr("Generate a new variation of the last type of tree you generated"));
    connect(displayCylinderForm, SIGNAL(triggered()), this, SLOT(displayAsCylinders()));
    displayCylinderForm->setEnabled(false);

    SubdivSlider = new QSlider( Qt::Horizontal);

    SubdivSlider->setValue(subdivs);
    SubdivSlider->setMaximum(4);
    SubdivSlider->setEnabled(false);

    connect(SubdivSlider, SIGNAL( valueChanged(int)), this, SLOT(SubdivSliderChange(int)));


    undo = new QAction(QIcon("./Resources/Icons/Undo.png"),("&Undo"), this);
    undo->setStatusTip(tr("Undo your last action"));
    connect(undo, SIGNAL(triggered()), this, SLOT(undoAction()));

    redo = new QAction(QIcon("./Resources/Icons/Redo.png"),("&Redo"), this);
    redo->setStatusTip(tr("Redo your last undone action"));
    connect(redo, SIGNAL(triggered()), this, SLOT(redoAction()));

    //viewXML = new QAction(("&Display XML file..."), this);
    //connect(viewXML, SIGNAL(triggered()), this, SLOT(viewXMLAction()));

    //viewLST = new QAction(("&Display LST file..."), this);
    //connect(viewLST, SIGNAL(triggered()), this, SLOT(viewLSTAction()));

    lineMode = new QToolButton(this);
    lineMode->setText("Line Mode");
    lineMode->setIcon(QIcon("./Resources/Icons/LineMode.png"));
    lineMode->setStatusTip(tr("Change to line drawing mode in the sketch interface"));
    connect(lineMode, SIGNAL(clicked()), this, SLOT(setLineMode()));
    lineMode->setCheckable(true);
    lineMode->setChecked(true);
    lineMode->setAutoExclusive(true);

    selectMode = new QToolButton(this);
    selectMode->setText("Select Mode");
    selectMode->setIcon(QIcon("./Resources/Icons/SelectMode.png"));
    selectMode->setStatusTip(tr("Change to selection mode in the sketch interface"));
    connect(selectMode, SIGNAL(clicked()), this, SLOT(setSelectMode()));
    selectMode->setCheckable(true);
    selectMode->setAutoExclusive(true);

    pencilMode = new QToolButton(this);
    pencilMode->setText("Pencil Mode");
    pencilMode->setIcon(QIcon("./Resources/Icons/PencilMode.png"));
    pencilMode->setStatusTip(tr("Change to pencil drawing mode in the sketch interface"));
    connect(pencilMode, SIGNAL(clicked()), this, SLOT(setPencilMode()));
    pencilMode->setCheckable(true);
    pencilMode->setAutoExclusive(true);

    brushSize = new QComboBox(this);
    QIntValidator *val = new QIntValidator(brushSize);
    val->setRange(3, 30);
    brushSize->setValidator(val);
    connect(brushSize, SIGNAL(activated(const QString&)), this, SLOT(setBrushSize(const QString&)));
    for (int i=0; i<BRUSH_SIZE_COUNT; ++i)
    {
        QString s;
        s.setNum(BRUSH_SIZE_OPTION[i]);
        brushSize->insertItem(i+1, s);
    }

    brushLabel = new QLabel(this);
    brushLabel->setText(" Brush size:  ");


    texSynthOption = new QAction(("&Synthesize Texture"), this);
    //  texSynthOption->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    texSynthOption->setStatusTip(tr("Synthesize a new texture from a sample"));
    connect(texSynthOption, SIGNAL(triggered()), this,SLOT(SynthesizeTexture()));

}

/*void MainWindow::createMenus()
{

}*/

void MainWindow::createToolBars()
{

    sketchToolBar = new QToolBar("Sketch Toolbar");
    sketchToolBar->setFloatable(false);
    sketchToolBar->setMovable(false);
    sketchToolBar->addAction(newSketch);
    sketchToolBar->addAction(generate);
    sketchToolBar->addAction(generationOption);
    sketchToolBar->addSeparator();
    sketchToolBar->addAction(undo);
    sketchToolBar->addAction(redo);
    sketchToolBar->addSeparator();
    sketchToolBar->addWidget(lineMode);
    sketchToolBar->addWidget(selectMode);
    sketchToolBar->addWidget(pencilMode);
    sketchToolBar->addSeparator();
    sketchToolBar->addWidget(brushLabel);
    sketchToolBar->addWidget(brushSize);
    sketchToolBar->addSeparator();
    sketchToolBar->addAction(texSynthOption);


    displayToolBar = new QToolBar("Display Toolbar");
    displayToolBar->setFloatable(false);
    displayToolBar->setMovable(false);
    displayToolBar->addAction(newVariation);
    // displayToolBar->addWidget(generateMesh);
    displayToolBar->addAction(displayCylinderForm);
    displayToolBar->addAction(displayMesh);
    displayToolBar->addWidget(SubdivSlider);



    //sketchToolBar->addWidget(brushSize);
    //displayToolBar->setGeometry(sketchToolBar->geometry());
    displayToolBar->setMinimumHeight(34);




}

void MainWindow::setupWidgets()
{
    QFrame *frame = new QFrame;
    QGridLayout *frameLayout = new QGridLayout(frame);
    frameLayout->setSpacing(1);
    frameLayout->setMargin(1);

    sketchWidget = new SketchWidget(this);
    sketchWidgetCreated = true;
    connect(sketchWidget, SIGNAL(sketchChanged()), this, SLOT(sketchChanged()));
    connect(sketchWidget, SIGNAL(sketchEmpty()), this, SLOT(sketchEmpty()));
    connect(sketchWidget,SIGNAL(sketchNonEmpty()), this, SLOT(sketchNonEmpty()));
    sketchWidget->setMinimumSize(700, 600);
    displayWidget = new QTreeDisplayWidget(frame);
    displayWidget->setMinimumSize(550, 600);
    frameLayout->addWidget(sketchToolBar, 0, 0);
    frameLayout->addWidget(sketchWidget, 1, 0);
    frameLayout->addWidget(displayToolBar, 0, 1);
    frameLayout->addWidget(displayWidget, 1, 1);
    setCentralWidget(frame);
}

void MainWindow::errorMessage(const char* msg, const char* title)
{
    QMessageBox messageBox(this);
    messageBox.setWindowTitle(QString(title));
    messageBox.setText(QString(msg));
    messageBox.setIcon(QMessageBox::Critical);
    messageBox.exec();
}

void MainWindow::on_actionExport_Cylinder_Model_to_OBJ_triggered()
{

}

void MainWindow::on_Export_to_Mesh_Model_to_OBJ_triggered()
{

}

// OVERHERE!!!!!!
void MainWindow::on_actionCreate_Foliage_triggered()
{
    cout << "Foliage" << endl;
}

void MainWindow::on_actionCreate_Leaves_triggered()
{
    cout << "Leaves" << endl;
}
