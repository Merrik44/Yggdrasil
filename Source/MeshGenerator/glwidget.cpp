/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtOpenGL>


#include <math.h>
#include "vmath.h"
#include "glwidget.h"
#include "mesh.h"
#include "debug.h"
#include "string"
#include "meshgenerator.h"
#include "objexporter.h"
#include "QImage"
#include "surfacesubdivision.h"
#include "QDir"
#include "performancetimer.h"
#include "mainwindow.h"
#include "ctime"
#include "fstream"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

using namespace std;

int subdivisionLevel = 0;

//! [0]
GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    xRot = 0;
    yRot = 0;
    zRot = 0;
    yOffset =0;
    xOffset =0;
    qtGreen = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
    qtPurple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);
    zoom = 1;

    //    zoom = 0.0329676;
    //    xOffset = 0.45f;
    //yOffset = -0.790001f;
    //xRot = 656;
    //yRot = 3480;


    skeleton = NULL;
    model = NULL;
}
//! [0]

//! [1]
GLWidget::~GLWidget()
{
}
//! [1]

//! [2]
QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}
//! [2]

//! [3]
QSize GLWidget::sizeHint() const
//! [3] //! [4]
{
    return QSize(400, 400);
}
//! [4]

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

//! [5]
void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}
//! [5]

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::ResetCamera()
{
    xRot = 0;
    yRot = 0;
    zRot = 0;
    yOffset =0;
    xOffset =0;
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::loadGLTextures()
{
    QImage t;
    QImage b;

    if ( !b.load( "../images/alphaTest.png", "PNG") )
    {
        b = QImage( 16, 16, (QImage::Format_ARGB32) );
        b.fill( Qt::green );
    }

    t = QGLWidget::convertToGLFormat( b );

    glGenTextures( 1, &texture[0] );
    glTexImage2D( GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits() );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
}

//! [6]
void GLWidget::initializeGL()
{
    QDir myDir("../lst files/");
    lstFileList = myDir.entryList ();
    for( int i = 0; i < lstFileList.count(); i++ )
        cout << lstFileList.value(i).toStdString() << endl;

    LoadLST("../lst files/treefile25.lst" );
    model = generateMesh( skeleton->branches);

    // app

    loadGLTextures();
    //qglClearColor(qtPurple.dark());

    glClearColor(1,  1,  1, 1);
    glEnable(GL_DEPTH_TEST);
    //  glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);

    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // glFrontFace(GL_CW);
  //  glEnable (GL_BLEND);
   // glBlendFunc (GL_ONE, GL_ONE);


    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_POLYGON_SMOOTH );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );



    //   RunPerformanceTest();
}
//! [6]


void GLWidget::LoadModel( std::string filepath )
{
    if( model != NULL )
        delete model;
    model = new Mesh(filepath);
    model->scale = 0.01f;

}



void GLWidget::LoadLST( std::string filepath )
{
    QString path = QString(  filepath.c_str());
    QString name = path.split(QRegExp( QString("/") )).last();
    listIndex = lstFileList.indexOf(QRegExp(name ));
    if (listIndex < 0)
        listIndex = 0;

    if( skeleton != NULL )
        delete skeleton;

    skeleton = new LstSkeleton();

    skeleton->scale = 0.01f;
     skeleton->LoadLstFile( filepath);
}

void GLWidget::reloadLST( )
{
    string path = skeleton->filepath;
    if( skeleton != NULL )
        delete skeleton;
    skeleton = new LstSkeleton(path);

}

void GLWidget::ExportModelAsObj(string filepath)
{
    ExportModel( filepath, model );
}

void GLWidget::setSubdivisionLevel( int level )
{

    subdivisionLevel = level;
    if( model == NULL )
        return;


    model->RestoreMeshState();
    model->ClearNeighourAndEdgeData();
    model->ReconstructMeshDataStructure();
    model->CalculateNormals();

    ApplyLoopSubvision(model, level, NULL);
    updateGL();
}
void GLWidget::GenerateMeshFromLST()
{
    if(model != NULL)
        delete model;

    if(skeleton != NULL)
        model = generateMesh( skeleton->branches);


    setSubdivisionLevel(subdivisionLevel );
    // updateGL();
}

MainWindow* mainwindowRef;

timeval t1, t2;

void GLWidget::subdivisionPerformanceTest()
{
    const int iterations = 30;
    const int numberOfBranches = 30;
    double  averageRunTimes[numberOfBranches*3];
    int numberOfFaces[ numberOfBranches*3 ];

    for ( int i = 0; i < numberOfBranches; i++ )
    {
        averageRunTimes[i] =0;
    }
    int totalIterationsNeeded = iterations*numberOfBranches;
    int iterationsSoFar = 0;

    ofstream perfLog("../../PerformanceLog_subdiv.txt");

    for(int k = 0; k < iterations; k++)
    {
        for ( int i = 0; i < numberOfBranches; i++ )
        {
            iterationsSoFar++;
            mainwindowRef->progressBarUpdate(iterationsSoFar*100/(totalIterationsNeeded) );

            if(model!= NULL )
                delete model;

            for ( int m = 0; m < 50; m++ )
            {
                if(skeleton!= NULL )
                    delete skeleton;
                skeleton = new LstSkeleton();
                skeleton->createRandomBifurcation( i, 800, 5, 5);

                if(skeleton->branches.size() == i +1)
                {
                    break;
                }

                if(m==49)
                {
                    cout<< "could not find branch configuration" << endl;
                }


            }
            // ---- memory ----

            // --- time ----
            int index, faceCount;
            double elapsedTime;


            model = generateMesh( skeleton->branches);

            // ----- subdiv first ----
            setSubdivisionLevel( 1 );
            faceCount = model->triangles.size();

            elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
            elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms

            index = i*3 + 0;
            cout<< numberOfFaces[index] << " : " << faceCount << endl;

            numberOfFaces[index] =faceCount;
            averageRunTimes[index] += elapsedTime;

            model->StoreMeshState();




            // ----- subdiv second ----

            setSubdivisionLevel( 1 );
            faceCount = model->triangles.size();

            elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
            elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms

            index = i*3 + 1;
            cout<< numberOfFaces[index] << " : " << faceCount << endl;

            numberOfFaces[index] =faceCount;
            averageRunTimes[index] += elapsedTime;

            model->StoreMeshState();


            // ----- sub div third -----


            setSubdivisionLevel( 1 );
            faceCount = model->triangles.size();

            elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
            elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms

            index = i*3 + 2;
            cout<< numberOfFaces[index] << " : " << faceCount << endl;

            numberOfFaces[index] =faceCount;
            averageRunTimes[index] += elapsedTime;

            model->StoreMeshState();


            paintGL();
            updateGL();

        }
    }



    for ( int i = 0; i < numberOfBranches*3; i++ )
    {
        averageRunTimes[i] /= (double)iterations;

        cout << numberOfFaces[i]<< "\t" << (double)averageRunTimes[i] << endl;
        perfLog  << numberOfFaces[i]<< "\t" << (double)averageRunTimes[i] << endl;
    }

    perfLog.close();

}

void GLWidget::RunPerformanceTest()
{
    // ---- generate graphs from 2 till 50 -----
    // int64  averageRunTime = 0;
    const int iterations = 30;
    const int numberOfBranches = 50;
    double  averageRunTimes[numberOfBranches];
    for ( int i = 0; i < numberOfBranches; i++ )
    {
        averageRunTimes[i] =0;
    }
    int totalIterationsNeeded = iterations*numberOfBranches;
    int iterationsSoFar = 0;

    ofstream perfLog("../../PerformanceLog_Ics.txt");

    for(int k = 0; k < iterations; k++)
    {


        for ( int i = 0; i < numberOfBranches; i++ )
        {
            cout <<k << " "<<  i << endl;
            iterationsSoFar++;
            mainwindowRef->progressBarUpdate(iterationsSoFar*100/(totalIterationsNeeded) );

            if(model!= NULL )
                delete model;

            int branches= i*4 + 2;
            for ( int m = 0; m < 50; m++ )
            {
                if(skeleton!= NULL )
                    delete skeleton;
                skeleton = new LstSkeleton();
                skeleton->createRandomBifurcation( branches, 1300, 5, 5);

                if(skeleton->branches.size() == branches + 1)
                {
                    break;
                }

                if(i==49)
                {
                    cout<< "could not find branch configuration" << endl;
                }


            }
            cout << (branches + 1) << " | " <<skeleton->branches.size() << endl;
            // ---- memory ----

            double vm_before, rss_before;
            process_mem_usage(vm_before, rss_before);
            //  cout << "VM: " << vm_before << "; RSS: " << rss_before << endl;

            // --- time ----

            double elapsedTime;
            //gettimeofday(&t1, NULL);

            model = generateMesh( skeleton->branches);



            // compute and print the elapsed time in millisec
            elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
            elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
            //      cout << elapsedTime << " ms.\n";

            averageRunTimes[i] += elapsedTime;

            double vm_after, rss_after;
            process_mem_usage(vm_after, rss_after);
            //     cout << "VM: " << (vm_after - vm_before)<< "; RSS: " << (rss_after - rss_before) << endl;

            //    cout << runTime << endl;
            //  delete testGraph;
            //  paintGL();
            //   updateGL();
            // sleep(3.5f);
        }



        // ---- for each time joint construction with both methods ----


        // --- time subdivisions ---


        // --- delete everything ---
    }

    // return;

    for ( int i = 0; i < numberOfBranches; i++ )
    {
        int branches= i*4 + 2;
        averageRunTimes[i] /= (double)iterations;
        // averageRunTimes[i] /= CLOCKS_PER_SEC;
        cout << (double)averageRunTimes[i] << endl;
        perfLog  << branches<< "\t" << (double)averageRunTimes[i] << endl;
    }

    perfLog.close();
}

void gluPerspective(double fovy,double aspect, double zNear, double zFar)
{
    // Start in projection mode.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double xmin, xmax, ymin, ymax;
    ymax = zNear * tan(fovy * M_PI / 360.0);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;
    glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}
//! [7]
void GLWidget::paintGL()
{

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective (50.0*zoom, 1, 0.1f, 100);


    glMatrixMode(GL_MODELVIEW);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    //    GLfloat light_position0[] = {10, -10, 0.0};
    //  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glTranslatef(xOffset, yOffset, -10);
    glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);



    //SetColour(LIGHT_GREY);

    if( model != NULL)
    {
        model->StoreTextureCoordInVerticesAndMarkSeams();
        model->Draw2();
    }

    if( skeleton != NULL)
        skeleton->Draw();

    DebugDraw();


}
//! [7]

//! [8]
void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    Width = Height = side;

    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective (50.0*zoom, 1, 0.1f, 100);

    glMatrixMode(GL_MODELVIEW);
}
//! [8]

//! [9]
void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}
//! [9]

//! [10]
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        yOffset += -dy*0.005f;
        xOffset += dx*0.005f;
        updateGL();
    }
    lastPos = event->pos();


}
//! [10]

void GLWidget::wheelEvent(QWheelEvent *event)
{
    float sensitivity = 0.001f;
    int numDegrees = -event->delta();

    zoom += zoom*sensitivity*numDegrees;

    updateGL();
    event->accept();
}
