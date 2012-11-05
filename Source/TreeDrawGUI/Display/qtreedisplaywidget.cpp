#include "qtreedisplaywidget.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stack>
#include <functional>
#include <locale>
#include <QDebug>
#include "matrix.h"
#include "point.h"
#include <math.h>
#include "MeshGenerator/surfacesubdivision.h"
#include "MeshGenerator/meshgenerator.h"
#include "MeshGenerator/objexporter.h"



using namespace std;

const float DefaultCameraDistance = 120.f;
const float DefaultCameraRotationX = 0.0f;
const float DefaultCameraRotationY = 0;

const float FieldOfView = 45.f;
const float NearPlane = 1.f;
const float FarPlane = 100000.f;

const int MOVEMENT_MODE_INACTIVE = 0;
const int MOVEMENT_MODE_ROTATE = 1;
const int MOVEMENT_MODE_PAN = 2;

const int BACKGROUND_COLOUR_BLACK = 0;
const int BACKGROUND_COLOUR_WHITE = 1;

const int TEXTURE_SIZE = 512;


GLuint texture;



GLuint CreateGLTexture(const char * filename, int width, int height)
{
    GLuint texture;
    QImage t;
    QImage b;

    t = QGLWidget::convertToGLFormat( b );
    if ( !b.load( filename ) )
    {
        cout << "Texture could not be loaded: " << filename << endl;
        b = QImage( 32, 32, QImage::Format_RGB32);
        b.fill( Qt::white );
    }

    t = QGLWidget::convertToGLFormat( b );
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexImage2D( GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits() );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture);

    return texture;
}

QTreeDisplayWidget::QTreeDisplayWidget(QWidget *parent) :
    QGLWidget(parent),
    cameraRotationX(DefaultCameraRotationX),
    cameraRotationY(DefaultCameraRotationY),
    movementMode(MOVEMENT_MODE_INACTIVE),
    cameraDistance(DefaultCameraDistance)
{
    tree = 0;
    centerX = 0.0f;
    centerY = 0.0f;
    centerZ = 0.0f;
    panX = 0.0f;
    panY = 0.0f;
    zoomFactor = 0.0001f;
    panFactor = 1.0f;
    isCurrent = false;
    displayGeneratedMesh = false;
    //barkTexture = 0;
   // barkTexture = 0;

    backgroundColour = BACKGROUND_COLOUR_BLACK;

    setCursor(QCursor(Qt::OpenHandCursor));


    makeCurrent();

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING);

    glDisable(GL_CULL_FACE);

    lstGraph = NULL;
    subdivisionSurface = NULL;
    foliageMesh = NULL;
    renderFoliage = true;
    renderSubdivisionSurface = true;
    renderTexture = true;


    cout << "eeee" << endl;
    //setTexture("./Resources/Textures/bark.jpg");
    //    LoadLST("./lst files/treefile4.lst" );
    //    model = generateMesh( lstGraph->branches);
    //    model->scale = 60;

    //  ApplyLoopSubvision(model, 4);
}

QTreeDisplayWidget::~QTreeDisplayWidget()
{
    if (tree != 0)
        delete tree;

    if( lstGraph != NULL )
        delete lstGraph;
    lstGraph = NULL;

    if( subdivisionSurface != NULL )
        delete subdivisionSurface;
    subdivisionSurface = NULL;
}

void QTreeDisplayWidget::setBarkTexture(const QString &path)
{
    glDeleteTextures( 1, &barkTexture );
    barkTexture = CreateGLTexture(path.toStdString().c_str(), TEXTURE_SIZE, TEXTURE_SIZE);
}

void QTreeDisplayWidget::setLeafTexture(const QString &path)
{
    glDeleteTextures( 1, &leafTexture );
    leafTexture = CreateGLTexture(path.toStdString().c_str(), TEXTURE_SIZE, TEXTURE_SIZE);
}


void QTreeDisplayWidget::initializeGL()
{
    if (backgroundColour == BACKGROUND_COLOUR_BLACK)
    {
        glClearColor(0.0, 0.0, 0.0, 0.0);
    }
    else if (backgroundColour == BACKGROUND_COLOUR_WHITE)
    {
        glClearColor(1.0, 1.0, 1.0, 1.0);
    }
    else
    {
        glClearColor(0.0, 0.0, 0.0, 0.0);
    }
    glEnable(GL_DEPTH_TEST);

}

void QTreeDisplayWidget::LoadLST( std::string filepath )
{


    if( lstGraph != NULL )
        delete lstGraph;
    lstGraph = new LstSkeleton(filepath);
}

void QTreeDisplayWidget::GenerateMeshFromLST( QProgressDialog* progressBar)
{

    if( subdivisionSurface != NULL )
        delete subdivisionSurface;
    subdivisionSurface = generateMesh( lstGraph->branches, progressBar);
}

void QTreeDisplayWidget::ApplySubdivisionToMesh( int numberOfSubdvisions,  QProgressDialog* progressBar )
{

    if( subdivisionSurface == NULL )
        return;


    subdivisionSurface->RestoreMeshState();
    subdivisionSurface->ClearNeighourAndEdgeData();
    subdivisionSurface->ReconstructMeshDataStructure();
    subdivisionSurface->CalculateNormals();


    ApplyLoopSubvision(subdivisionSurface, numberOfSubdvisions, progressBar);

}

void QTreeDisplayWidget::LoadFoliage(std::string filepath)
{
    cout << "loading foliage " << endl;
    if( foliageMesh != NULL)
        delete foliageMesh;

    foliageMesh = new Mesh(filepath);
}

void qtGluPerspective(double fovy,double aspect, double zNear, double zFar)
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


void QTreeDisplayWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    if (h == 0)
        h = 1;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    qtGluPerspective(FieldOfView, w/(float)h, NearPlane, FarPlane);
    glMatrixMode(GL_MODELVIEW);
}

void QTreeDisplayWidget::paintGL()
{
    makeCurrent();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (backgroundColour == BACKGROUND_COLOUR_BLACK)
    {
        glClearColor(0.0, 0.0, 0.0, 0.0);
    }
    else if (backgroundColour == BACKGROUND_COLOUR_WHITE)
    {
        glClearColor(1.0, 1.0, 1.0, 1.0);
    }
    else
    {
        glClearColor(0.0, 0.0, 0.0, 0.0);
    }

    glLoadIdentity();


        if(renderTexture)
           glEnable(GL_TEXTURE_2D);
        else
             glDisable(GL_TEXTURE_2D);



        glTranslatef(-panX, -panY, -0.0f);
        GLfloat light_position0[] = {0,0, -1, 0.0};
        glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
        glTranslatef(0, 0, -cameraDistance);
        glRotatef(cameraRotationY,1.0,0.0,0.0);
        glRotatef(cameraRotationX,0.0,1.0,0.0);


        glTranslatef(-centerX, -centerY, -centerZ);

        glColor3f(1.0f,1.0f,1.0f);


        glBindTexture(GL_TEXTURE_2D, barkTexture);
        if( renderSubdivisionSurface )
        {
            if(subdivisionSurface!= NULL)
                subdivisionSurface->Draw();
        }
        else
        {
            if(tree != NULL)
                tree->Render();
        }

         glBindTexture(GL_TEXTURE_2D, leafTexture);
        if(renderFoliage)
        {
            if( foliageMesh != NULL )
                foliageMesh->Draw();
        }




}


void QTreeDisplayWidget::mouseMoveEvent(QMouseEvent  *event)
{
    if (movementMode == MOVEMENT_MODE_ROTATE || movementMode == MOVEMENT_MODE_PAN)
    {
        int dx = movementX - event->x();
        int dy = movementY - event->y();

        if (movementMode == MOVEMENT_MODE_ROTATE)
        {
            cameraRotationX -= dx * 0.3;
            cameraRotationY -= dy * 0.3;
        }
        else if (movementMode == MOVEMENT_MODE_PAN)
        {
            float x = ((float)dx)/this->width();
            float y = ((float)dy)/this->height();

            panX += x * panFactor;
            panY -= y * panFactor;
        }
        glDraw();

        movementX = event->x();
        movementY = event->y();
    }
}

void QTreeDisplayWidget::mousePressEvent(QMouseEvent  *event)
{
    if (movementMode == MOVEMENT_MODE_INACTIVE)
    {
        movementX = event->x();
        movementY = event->y();
        int button = event->buttons();
        if (button == Qt::LeftButton)
        {
            movementMode = MOVEMENT_MODE_ROTATE;
        }
        else if (button == Qt::RightButton)
        {
            movementMode = MOVEMENT_MODE_PAN;
        }

        setCursor(QCursor(Qt::ClosedHandCursor));
    }
}

void QTreeDisplayWidget::mouseReleaseEvent(QMouseEvent  *event)
{
    movementMode = MOVEMENT_MODE_INACTIVE;
    setCursor(QCursor(Qt::OpenHandCursor));
}

void QTreeDisplayWidget::wheelEvent(QWheelEvent *event)
{
    cameraDistance -= event->delta() * zoomFactor;
    panFactor = fabs(cameraDistance) * 0.85f;
    glDraw();
}


void QTreeDisplayWidget::clearDisplay()
{
    if (tree != 0)
    {
        delete tree;
    }
    tree = 0;


    if( lstGraph != NULL )
        delete lstGraph;
    lstGraph = NULL;

    if( subdivisionSurface != NULL )
        delete subdivisionSurface;
    subdivisionSurface = NULL;

    if( foliageMesh != NULL )
        delete foliageMesh;
    foliageMesh = NULL;

}



bool QTreeDisplayWidget::setSourceFile(const std::string &fileName)
{
    TreeModel *newTree;
    try {
        newTree = TreeModel::createFromLST(fileName);
    }
    catch (...) {
        newTree = 0;
    }

    if (tree != 0)
        delete tree;
    tree = 0;
    if (newTree != 0)
    {
        tree = newTree;
        sourceFile = string(fileName);

        cameraRotationX = DefaultCameraRotationX;
        cameraRotationY = DefaultCameraRotationY;

        //float boundX, boundY, boundZ;

        tree->calculateCameraSettings(centerX, centerY, centerZ, boundX, boundY, boundZ);
        centerX = 0;centerY = 0; centerZ = 0;


        cameraDistance = fmax(boundX * 3, fmax(boundY * 3, boundZ * 1.2));
        zoomFactor = cameraDistance * (0.03f/90.0f);
        panFactor = cameraDistance * 0.85f;
        panX = 0.0f;
        panY = 0.0f;
        //cameraDistance = 2*tree->getMaxHeight();

        GLfloat al[] = {0.2, 0.2, 0.2, 1.0};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, al);
        glEnable(GL_LIGHT0);
        glEnable(GL_TEXTURE_2D);

        /*GLfloat light_ambient0[] = {1, 1, 1, 1.0};
        GLfloat light_diffuse0[] = {1.0, 1.0, 1.0, 1.0};
        GLfloat light_specular0[] = {1.0, 1.0, 1.0, 1.0};
        GLfloat light_ambient0[] = {0.5, 0.5, 0.5, 1.0};
        GLfloat light_diffuse0[] = {0.5, 0.5, 0.5, 1.0};
        GLfloat light_specular0[] = {0.5, 0.5, 0.5, 1.0};
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);*/


        glDraw();
    }

    return tree != 0;
}


void QTreeDisplayWidget::setBackgroundColour(int colour)
{
    backgroundColour = colour;
}


void QTreeDisplayWidget::setSourceXMLFile(const std::string &fileName)
{
    TreeModel *newTree = TreeModel::createFromXML(fileName);

    if (tree != 0)
        delete tree;
    tree = 0;

    if (newTree != 0)
    {
        tree = newTree;
        sourceFile = string(fileName);

        cameraRotationX = DefaultCameraRotationX;
        cameraRotationY = DefaultCameraRotationY;

        centerX = 0.0f;
        centerY = 0.0f;
        centerZ = 0.0f;

        cameraDistance = 100.0f;
        zoomFactor = 0.1f;
        panFactor = 0.85f * cameraDistance;
        panX = 0.0f;
        panY = 0.0f;


        boundX = 100.0f; boundY = 100.0f; boundZ = 100.0f;

        GLfloat al[] = {0.2, 0.2, 0.2, 1.0};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, al);
        glEnable(GL_LIGHT0);
        //glDisable(GL_LIGHT0);
        glEnable(GL_TEXTURE_2D);

        /*GLfloat light_ambient0[] = {1, 1, 1, 1.0};
        GLfloat light_diffuse0[] = {1.0, 1.0, 1.0, 1.0};
        GLfloat light_specular0[] = {1.0, 1.0, 1.0, 1.0};
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);*/

        glDraw();
    }

    /*ifstream fin(fileName,ios::binary);

    string line;
    string *data = new string();
    while (getline(fin, line))
        *data += line + '\n';


    char* xmlData = new char[data->size()+1];
    memcpy(xmlData,data->c_str(),data->size()+1);

    delete data;

    fin.close();

    xml_document<> *document = new xml_document<>();
    document->parse<0>(xmlData);

    list<Operation*> newRenderList;

    readBranch(&newRenderList, document->first_node("branch",0,false));

    renderList = newRenderList;
    sourceFile = string(fileName);
    paintGL();*/

}

void QTreeDisplayWidget::exportCylinderModelToObj(const std::string &fileName)
{
    if (tree != 0)
        tree->writeToObj(fileName);
}

void QTreeDisplayWidget::exportMeshToObj(const std::string &fileName)
{
    if (subdivisionSurface != 0)
        ExportModel( fileName, subdivisionSurface );
}

void QTreeDisplayWidget::setOutOfDate()
{
    if (isCurrent)
    {
        isCurrent = false;

        /*GLfloat light_ambient0[] = {1, 1, 1, 1.0};
        GLfloat light_diffuse0[] = {1.0, 1.0, 1.0, 1.0};
        GLfloat light_specular0[] = {1.0, 1.0, 1.0, 1.0};*/
        GLfloat light_ambient0[] = {0.5, 0.5, 0.5, 1.0};
        GLfloat light_diffuse0[] = {0.5, 0.5, 0.5, 1.0};
        GLfloat light_specular0[] = {0.5, 0.5, 0.5, 1.0};
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
        glDraw();
    }
}


void QTreeDisplayWidget::setCurrent()
{
    if (!isCurrent)
    {
        isCurrent = true;

        GLfloat light_ambient0[] = {1, 1, 1, 1.0};
        GLfloat light_diffuse0[] = {1.0, 1.0, 1.0, 1.0};
        GLfloat light_specular0[] = {1.0, 1.0, 1.0, 1.0};
        /*GLfloat light_ambient0[] = {0.5, 0.5, 0.5, 1.0};
        GLfloat light_diffuse0[] = {0.5, 0.5, 0.5, 1.0};
        GLfloat light_specular0[] = {0.5, 0.5, 0.5, 1.0};*/
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
        glDraw();
    }
}

