#include "texturesynthesisdialog.h"
#include "ui_texturesynthesisdialog.h"
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
#include "QTime"

using namespace std;
TextureSynthesisDialog::TextureSynthesisDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextureSynthesisDialog)
{
    ui->setupUi(this);
    
    synthesis = new TextureSynthesis();
    synthesis->dialog = this;
    synthesis->quality = ui->spinBox_4->value();
    
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

TextureSynthesisDialog::~TextureSynthesisDialog()
{
    delete ui;
}

void TextureSynthesisDialog::on_pushButton_released()
{
    //    QRgb** pixels;
    //        QRgb ** test = new QRgb*[1];
            
    //        test[0] = new QRgb[1];
    //    //    test[0][0] = 1;
            
    //    //    delete [] test[0];
    //        delete [] test;
    //        cout<<"test"<<endl;
    //    int ** test = new int*[1];
        
    //    test[0] = new int[1];
    //    test[0][0] = 1;
    //    int * test2 = new int[0];
    //    test2 = test[0];
    ////    delete [] test[0];
    //    delete [] test;
    //    cout<<"test"<<endl;
    //    cout<<test2[0]<<endl;
    //    cout<<test[0][0]<<endl;
        QString imagePath = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                      "",
                                                      tr("Files (*.*)"));
        
        QImage image;
        image.load(imagePath);
        
    //    Texture * tex = new Texture();
    //    tex->halfSize();
    //    tex->pixels[0][0] = qRgba(0,0,0,0);
    //    delete tex;
        
        synthesis->loadImage(imagePath);
        
        //this is the size of the preview image
        Vector2 size = Vector2(160,160);
        QImage temp = synthesis->originalTextures[synthesis->originalTextures.size()-1].getImage();
        //display the preview
        scaleAndDisplayImage(size,temp, ui->label);
}

void TextureSynthesisDialog::on_pushButton_4_released()
{
    QString imagePath = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                  "",
                                                  tr("PNG Format(*.png *.PNG);;JPEG Format(*.jpg *.jpeg *.jpe);;All Files (*)"));
    cout<<"ImagePath \""<<imagePath.toStdString()<<"\""<<endl;
//    imagePath.remove(0,1);
    cout<<"ImagePath \""<<imagePath.toStdString()<<"\""<<endl;
    if(imagePath!="" && synthesis!=NULL)
    {
        if (!(imagePath.contains("*.*")))
                imagePath += ".png";
        cout<<"ImagePath \""<<imagePath.toStdString()<<"\""<<endl;
        synthesis->patchedTexture->getImage().save(imagePath);
    }
}

void TextureSynthesisDialog::on_pushButton_2_released()
{
    Vector2 v = Vector2(ui->spinBox->value(),ui->spinBox->value());
    
    int iterations = ui->spinBox_3->value();
    synthesis->synthesise(v,iterations);
    
    QImage temp = synthesis->patchedTexture->getImage();
    
    Vector2 size = Vector2(400,400);
    scaleAndDisplayImage(size,temp, ui->label_2);
}

void TextureSynthesisDialog::setPreviewImage(QImage & image)
{
    Vector2 size = Vector2(400,400);
    scaleAndDisplayImage(size,image, ui->label_2);
}

void TextureSynthesisDialog ::scaleAndDisplayImage(Vector2 & sizeScaledPatched, QImage & texture, QLabel * label)
{
    worker->slotSetLabel(sizeScaledPatched, texture, label);
}

void TextureSynthesisDialog::on_spinBox_4_valueChanged(int arg1)
{
    synthesis->quality = arg1;
}
