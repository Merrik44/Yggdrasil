#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QtCore/qmath.h>
#include <iostream>
#include <QProgressBar>
#include "displaylabelthread.h"

#include "coherenttexture.h"
#include "indexedtexture.h"
#include "texture.h"
#include "texturesynthesis.h"
#include "vector2.h"
#include <vector>
using namespace std;
bool t = true;



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    
    synthesis = new TextureSynthesis();
    synthesis->quality = ui->horizontalSlider->value();
    
    QThread* thread = new QThread;
    worker = new DisplayLabelThread();
//    worker->moveToThread(thread);
//    connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(thread, SIGNAL(started()), worker, SLOT(slotSetLabel(Vector2&, Texture &, QLabel *)));
//    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
//    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    
    worker->moveToThread(thread);
    thread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_released()
{
    QString imagePath = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                  "",
                                                  tr("Files (*.*)"));
    synthesis->loadImage(imagePath);
    //this is the size of the preview image
    Vector2 size = Vector2(128,128);
    QImage temp = synthesis->originalTextures[synthesis->originalTextures.size()-1].getImage();
    //display the preview
    scaleAndDisplayImage(size,temp, ui->label);
       
}

void MainWindow::on_pushButton_2_clicked()
{

}

void MainWindow::on_pushButton_4_released()
{
    QString imagePath = QFileDialog::getSaveFileName(this, tr("Open File"),
                                                  "",
                                                  tr("Files (*.*)"));
    cout<<"ImagePath \""<<"\""<<endl;
    if(imagePath!="" && synthesis!=NULL)
    {
        synthesis->patchedTexture->getImage().save(imagePath);
    }
}

void MainWindow::on_pushButton_2_released()
{    
    Vector2 v = Vector2(ui->spinBox->value(),ui->spinBox->value());
    
    int iterations = ui->spinBox_3->value();
    synthesis->synthesise(v,iterations);
    
    QImage temp = synthesis->patchedTexture->getImage();
    
    Vector2 size = Vector2(128,128);
    scaleAndDisplayImage(size,temp, ui->label_2);
}

void MainWindow ::scaleAndDisplayImage(Vector2 & sizeScaledPatched, QImage & texture, QLabel * label)
{
    worker->slotSetLabel(sizeScaledPatched, texture, label);
    
}
void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    synthesis->quality = value;
}
