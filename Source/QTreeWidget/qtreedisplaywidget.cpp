#include "qtreedisplaywidget.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <locale>
#include <stack>

#include <rapidxml-1.13/rapidxml.hpp>

using namespace rapidxml;
using namespace std;

const float DefaultCameraDistance = 50.f;
const float DefaultCameraRotationX = 0;
const float DefaultCameraRotationY = 0;

const float FieldOfView = 45.f;
const float NearPlane = 1.f;
const float FarPlane = 100000.f;

GLuint texture;

GLuint CreateGLTexture(const char * filename, int width, int height)
{
    GLuint texture;
    char * image;
    std::ifstream fin(filename,std::ios::in | std::ios::binary);

    if (!fin.is_open()) return 0;

    qDebug("texture open");
    image = new char[height*width*3];
    fin.read(image,height*width*3);
    fin.close();

    glGenTextures(0, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height,
                      GL_RGB, GL_UNSIGNED_BYTE, image);



    delete[] image;
    return texture;
}

QTreeDisplayWidget::QTreeDisplayWidget(QWidget *parent) :
    QGLWidget(parent),
    cameraRotationX(DefaultCameraRotationX),
    cameraRotationY(DefaultCameraRotationY),
    mouseDown(false),
    cameraDistance(DefaultCameraDistance)
{
    makeCurrent();

    glEnable(GL_TEXTURE_2D);

    texture = CreateGLTexture("woodtexture2.raw",512,512);

    glEnable(GL_LIGHTING);

    GLfloat al[] = {0.0, 0.0, 0.0, 1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, al);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    GLfloat light_ambient[] = {1, 1, 1, 1.0};
    GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light_position[] = {0.0, 100.0, 100.0, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    GLfloat light_ambient1[] = {1, 1, 1, 1.0};
    GLfloat light_diffuse1[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light_specular1[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light_position1[] = {0.0, 100.0, -100.0, 1.0};
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
}

void QTreeDisplayWidget::initializeGL()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);

}

void QTreeDisplayWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    if (h == 0)
        h = 1;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FieldOfView, w/(float)h, NearPlane, FarPlane);
    glMatrixMode(GL_MODELVIEW);
}

void QTreeDisplayWidget::paintGL()
{
    makeCurrent();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glLoadIdentity();

    glTranslatef(0, 0, -cameraDistance);
    glRotatef(cameraRotationY,1.0,0.0,0.0);
    glRotatef(cameraRotationX,0.0,1.0,0.0);

    glColor3f(1.0f,0.0f,0.0f);

    glBindTexture(GL_TEXTURE_2D, texture);

    std::list<Operation*>::iterator e = renderList.end();
    for(std::list<Operation*>::iterator i = renderList.begin();i!=e;i++)
    {
        (*i)->Render();
    }


}


void QTreeDisplayWidget::mouseMoveEvent(QMouseEvent  *event)
{
    if (mouseDown)
    {
        int dx = movementX - event->x();
        int dy = movementY - event->y();

        cameraRotationX -= dx * 0.3;
        cameraRotationY -= dy * 0.3;
        glDraw();

        movementX = event->x();
        movementY = event->y();
    }
}

void QTreeDisplayWidget::mousePressEvent(QMouseEvent  *event)
{
    movementX = event->x();
    movementY = event->y();
    mouseDown = true;
}

void QTreeDisplayWidget::mouseReleaseEvent(QMouseEvent  *event)
{
    mouseDown = false;
}

void QTreeDisplayWidget::wheelEvent(QWheelEvent *event)
{
    cameraDistance -= event->delta()/32;
    glDraw();
}



void QTreeDisplayWidget::setSourceFile(const char *fileName)
{
    string s,operation;
    fstream source;
    list<Operation*> newRenderList;
    size_t pos;
    char line[2048];

    source.open(fileName,std::fstream::in);

    if (source)
    {
        getline(source, s);

        if (s == FileHeader)
        {
            float x,y,z,angle;
            float startRadius, endRadius, height;

            stack<GeneralisedCylinderOperation*> branchStack;
            GeneralisedCylinderOperation* currentBranch = new GeneralisedCylinderOperation();
            newRenderList.push_back(currentBranch);

            while (source.getline(line,2048) != 0)
            {
                s = std::string(line);
                //trim left space
                s.erase(s.begin(),std::find_if(s.begin(),s.end(),std::not1(std::ptr_fun<int, int>(std::isspace))));

                pos = s.find("(");

                if (pos == std::string::npos)
                    continue;

                operation = s.substr(0,pos);


                if (operation == "SB")
                {
                    branchStack.push(currentBranch);
                    currentBranch = new GeneralisedCylinderOperation();
                    newRenderList.push_back(new PushOperation());
                    newRenderList.push_back(currentBranch);
                }

                else if (operation == "EB")
                {
                    currentBranch->constructCompleteBranch();

                    currentBranch = branchStack.top();
                    branchStack.pop();
                    newRenderList.push_back(new PopOperation());
                }

                else if (operation == "Cut")
                    newRenderList.push_back(new CutOperation());

                else if (operation == "MovRel3f")
                {
                    sscanf(s.substr(pos).c_str(),"(%f, %f, %f)", &x, &y, &z);
                    newRenderList.push_back(new TranslateOperation(x, y, z));
                    currentBranch->applyTranslation(x, y, z);
                }

                else if (operation == "RotRel3f")
                {
                    sscanf(s.substr(pos).c_str(),"(%f, %f, %f, %f)", &angle, &x, &y, &z);
                    newRenderList.push_back(new RotateOperation(angle, x, y, z));
                    currentBranch->applyRotation(angle, x, y, z);
                }

                else if (operation == "Cylinder")
                {
                    sscanf(s.substr(pos).c_str(),"(%f, %f, %f)", &startRadius, &endRadius, &height);
                    //newRenderList.push_back(new CylinderOperation(startRadius, endRadius, height));
                    currentBranch->addBranch(startRadius, endRadius, height);
                }

            }

            currentBranch->constructCompleteBranch();

            if (source.eof())
            {
                renderList = newRenderList;
                sourceFile = std::string(fileName);
                paintGL();
            }
        }



        source.close();

    }

}

pair<float,float> extractParameterRange(const xml_node<> *node)
{
    float first,second;
    sscanf(node->first_node("min",0,false)->value(),"%f",&first);
    sscanf(node->first_node("max",0,false)->value(),"%f",&second);
        return make_pair(first,second);
}

void readBranch(list<Operation*> *opList, const xml_node<> *node)
{
    pair<float,float> lengthP;
    lengthP = extractParameterRange(node->first_node("length", 0, false));
    float length = lengthP.first/10;

    opList->push_back(new CylinderOperation(0.2f,0.2f,length));

    xml_node<> *children = node->first_node("children", 0, false);
    for (xml_node<> *child = children->first_node("branch"); child; child = child->next_sibling("branch"))
    {
        float relativePosition;
        sscanf(child->first_node("positionOnParent", 0, false)->value(), "%f", &relativePosition);

        pair<float,float> angleP;
        angleP = extractParameterRange(child->first_node("angleToParent", 0, false));
        float angle = (angleP.first + angleP.second)/2;

        float aroundAngle;
        sscanf(child->first_node("rotationalAngleToParent", 0, false)->value(), "%f", &aroundAngle);

        opList->push_back(new PushOperation());

        opList->push_back(new TranslateOperation(0.0f,length*relativePosition,0.0f));

        opList->push_back(new RotateOperation(aroundAngle,0.0f,1.0f,0.0f));

        opList->push_back(new RotateOperation(angle,0.0f,0.0f,1.0f));

        readBranch(opList,child);

        opList->push_back(new PopOperation());
    }

}


void QTreeDisplayWidget::setSourceXMLFile(const char *fileName)
{

    ifstream fin(fileName,ios::binary);

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
    sourceFile = std::string(fileName);
    paintGL();

}
