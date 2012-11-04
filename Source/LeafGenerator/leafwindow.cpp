#include <QtGui>

#include "leafwindow.h"
#include "sketcharea.h"
#include "sidebar.h"
#include "foliageparameters.h"

LeafWindow::LeafWindow()
{
    sketchArea = new SketchArea;
    sketchArea->setMinimumSize(512, 512);
    sideBar = new SideBar(sketchArea);
         //setLeftWidget(sketchArea);
    QFrame *frame = new QFrame(this);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(sketchArea);
    layout->addWidget(sideBar);
    frame->setLayout(layout);
    setCentralWidget(frame);
    createActions();
    createMenus();

    setWindowTitle(tr("Leaf Generator"));
    resize(500, 500);

    qsrand(QTime::currentTime().msec());
    statusBar()->showMessage("Text",1);
}

void LeafWindow::closeEvent(QCloseEvent *event)
{
    if (saveOnExit()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void LeafWindow::foliage()
{
    FoliageParameters* f = new FoliageParameters();
    //f->setValues();
    f->createMesh(QString("GeneratedLeaves/treefile112.lst"));

}

void LeafWindow::open()
{
    if (saveOnExit()) {
        QString fileName = QFileDialog::getOpenFileName(this,
                                   tr("Open File"), QDir::currentPath());
        if (!fileName.isEmpty())
            sketchArea->openImage(fileName);
    }
}

void LeafWindow::setStatusText(QString text)
{
    statusBar()->showMessage(text, 5000);
}

void LeafWindow::save()
{
    QAction *action = qobject_cast<QAction *>(sender());
    //Takes format from menu name
    QByteArray fileFormat = action->data().toByteArray();
    saveFile(fileFormat);
}

void LeafWindow::penColor()
{
    QColor newColor = QColorDialog::getColor(sketchArea->getPenColor());
    if (newColor.isValid())
        sketchArea->setPenColor(newColor);
}

void LeafWindow::about()
{
    QMessageBox::about(this, tr("About Leaf Generator"),
            tr("<p>Created by: Donovan Foster</p>"));
}

void LeafWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    foreach (QByteArray format, QImageWriter::supportedImageFormats()) {
        QString text = tr("%1...").arg(QString(format).toUpper());

        QAction *action = new QAction(text, this);
        action->setData(format);
        connect(action, SIGNAL(triggered()), this, SLOT(save()));
        saveAsActs.append(action);
    }

    printAct = new QAction(tr("&Print..."), this);
    connect(printAct, SIGNAL(triggered()), sketchArea, SLOT(print()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    penColorAct = new QAction(tr("&Pen Color..."), this);
    connect(penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));

    newAct = new QAction(tr("&New Sketch"), this);
    newAct->setShortcut(tr("Ctrl+N"));
    connect(newAct, SIGNAL(triggered()),
            sketchArea, SLOT(clearImage()));

    clearScreenAct = new QAction(tr("&Clear Sketch"), this);
    clearScreenAct->setShortcut(tr("Ctrl+L"));
    connect(clearScreenAct, SIGNAL(triggered()),
            sketchArea, SLOT(clearImage()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    foliageAct = new QAction(tr("&Foliage"), this);
    connect(foliageAct, SIGNAL(triggered()), this, SLOT(foliage()));

    connect(sketchArea, SIGNAL(setStatusText(QString)), this, SLOT(setStatusText(QString)));
}

void LeafWindow::createMenus()
{
    saveAsMenu = new QMenu(tr("&Save As"), this);
    foreach (QAction *action, saveAsActs)
        saveAsMenu->addAction(action);

    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addMenu(saveAsMenu);
    //fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(foliageAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    optionMenu = new QMenu(tr("&Options"), this);
    //optionMenu->addAction(penColorAct);
    //optionMenu->addSeparator();
    optionMenu->addAction(clearScreenAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(optionMenu);
    menuBar()->addMenu(helpMenu);
}

bool LeafWindow::saveOnExit()
 {
     if (sketchArea->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Leaf"),
                           tr("The image has been modified.\n"
                              "Do you want to save your changes?"),
                           QMessageBox::Save | QMessageBox::Discard
                           | QMessageBox::Cancel);
         if (ret == QMessageBox::Save) {
             return saveFile("png");
         } else if (ret == QMessageBox::Cancel) {
             return false;
         }
     }
     return true;
 }

bool LeafWindow::saveFile(const QByteArray &fileFormat)
{
    QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                               initialPath,
                               tr("%1 Files (*.%2);;All Files (*)")
                               .arg(QString(fileFormat.toUpper()))
                               .arg(QString(fileFormat)));
    if (fileName.isEmpty()) {
        return false;
    } else {
        return sketchArea->saveImage(fileName, fileFormat);
    }
}

LeafWindow::~LeafWindow()
{
}
