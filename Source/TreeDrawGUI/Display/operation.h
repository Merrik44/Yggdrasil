#ifndef OPERATION_H
#define OPERATION_H

#include <string>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <vector>

#include "point.h"
#include "vector.h"
#include "matrix.h"

const int CylinderSlices = 16;

using namespace std;


class Operation
{

public:
    Operation(){}
    virtual int Render(Matrix &m){}

};


//EB()
class PopOperation: public Operation
{

public:
    PopOperation()
    {

    }

    int Render(Matrix &m)
    {
        glPopMatrix();
        return 2;
    }


};

//SB()
class PushOperation: public Operation
{

public:
    PushOperation()
    {

    }

    int Render(Matrix &m)
    {
        glPushMatrix();
        return 1;
    }


};

//Cut()
class CutOperation: public Operation
{

public:
    CutOperation()
    {

    }

    int Render(Matrix &m)
    {
        //???
        return 0;
    }


};

//MovRel3f()
class TranslateOperation: public Operation
{

private:
    float x,y,z;

public:
    TranslateOperation(float ax, float ay, float az) :
        x(ax), y(ay), z(az)
    {

    }

    int Render(Matrix &m)
    {
        Matrix trans = Matrix::translationMatrix(x, y, z);
        m = m * trans;
        glTranslatef(x,y,z);
        return 0;
    }


};

//RotRel3f()
class RotateOperation: public Operation
{

private:
    float angle,x,y,z;

public:
    RotateOperation(float aangle, float ax, float ay, float az) :
        angle(aangle), x(ax), y(ay), z(az)
    {

    }

    int Render(Matrix &m)
    {
        Matrix rot = Matrix::rotationMatrix(angle*180/M_PI, x, y, z);
        m = m * rot;
        glRotatef(angle*180/M_PI,x,y,z);
        return 0;
    }

};




//Cylinder()
class CylinderOperation: public Operation
{

private:
    float baseRadius,topRadius,height;

public:
    CylinderOperation(float startRadius, float endRadius, float cylHeight) :
        baseRadius(startRadius), topRadius(endRadius), height(cylHeight)
    {

    }

    void drawCylinder(int slices, float height, float startRadius, float endRadius, Matrix &m)
    {
        double step = 2.0 * M_PI / slices;

        glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= slices; ++i)
        {
            double a = i * step;
            GLfloat sx = startRadius * cos(a);
            GLfloat sy = startRadius * sin(a);
            GLfloat ex = endRadius * cos(a);
            GLfloat ey = endRadius * sin(a);
            Point c = m * Point(sx, 0, sy);
            Point b = m * Point(0,0,0);
            Vector normal = Vector(-sx, 0, -sy);//Vector(b,c);
            normal.normalize();
            glNormal3f(normal.x, normal.y, normal.z);
            glVertex3f(sx, 0, sy);
            glVertex3f(ex, height, ey);
        }
        glEnd();

    }

    int Render(Matrix &m)
    {
        drawCylinder(CylinderSlices,height,baseRadius,topRadius, m);
        return 0;
    }


};






#endif // OPERATION_H
