#ifndef OPERATION_H
#define OPERATION_H

#include <string>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <vector>

const int CylinderSlices = 16;

using namespace std;


class Operation
{

public:
    Operation(){}
    virtual void Render(){}

};


//EB()
class PopOperation: public Operation
{

public:
    PopOperation()
    {

    }

    void Render()
    {
        glPopMatrix();
    }


};

//SB()
class PushOperation: public Operation
{

public:
    PushOperation()
    {

    }

    void Render()
    {
        glPushMatrix();
    }


};

//Cut()
class CutOperation: public Operation
{

public:
    CutOperation()
    {

    }

    void Render()
    {
        //???
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

    void Render()
    {
        glTranslatef(x,y,z);
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

    void Render()
    {
        glRotatef(angle*180/M_PI,x,y,z);
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

    void drawCylinder(int slices, float height, float startRadius, float endRadius)
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
            glVertex3f(sx, 0, sy);
            glVertex3f(ex, height, ey);
        }
        glEnd();

    }

    void Render()
    {
        drawCylinder(CylinderSlices,height,baseRadius,topRadius);
    }


};



const float TOLERANCE = 1e-5;
const float CURVE_FACTOR = 3e-1;


struct Point
{
    float x, y, z;

    void print()
    {
        char t[100];
        sprintf(t,"%.2f %.2f %.2f",x,y,z);
        qDebug(t);
    }

    Point()
    {

    }

    Point(float ax, float ay, float az) : x(ax), y(ay), z(az)
    {

    }

    static Point average(const Point &a, const Point &b)
    {
        return Point( (a.x + b.x) / 2,
                      (a.y + b.y) / 2,
                      (a.z + b.z) / 2 );
    }

    static float distance(const Point &a, const Point &b)
    {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        float dz = a.z - b.z;
        return sqrtf(dx*dx + dy*dy + dz*dz);
    }
};


struct Vector
{
    float x,y,z;

    void print()
    {
        char t[100];
        sprintf(t,"%.2f %.2f %.2f",x,y,z);
        qDebug(t);
    }

    Vector()
    {

    }

    Vector(float ax, float ay, float az) :
        x(ax), y(ay), z(az)
    {

    }

    Vector(const Point &a, const Point &b) :
        x(b.x-a.x), y(b.y-a.y), z(b.z-a.z)
    {

    }

    void normalize()
    {
        float m = sqrt(x*x + y*y + z*z);
        x /= m;
        y /= m;
        z /= m;
    }

    static Vector average(Vector a, Vector b)
    {
        a.normalize();
        b.normalize();

        return Vector((a.x+b.x)/2, (a.y+b.y)/2, (a.z+b.z)/2);
    }

    static Vector cross(const Vector &a, const Vector &b)
    {
        return Vector( a.y*b.z - a.z*b.y, a.x*b.z - a.z*b.x, a.x*b.y - a.y*b.x );
    }

    static float dot(const Vector &a, const Vector &b)
    {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }

    float magnitude()
    {
        return sqrt(x*x + y*y + z*z);
    }

    Vector operator* (const float &m)
    {
        return Vector( x*m, y*m, z*m );
    }

    Vector operator+ (const Vector &other)
    {
        return Vector( x + other.x, y + other.y, z + other.z );
    }

    Point operator+ (const Point &other)
    {
        return Point( x + other.x, y + other.y, z + other.z );
    }

};

struct Matrix
{
    float m[16];

    void print()
    {
        char t[100];

        for (int i=0;i<16; i+=4)
        {
            sprintf(t,"%.2f %.2f %.2f %.2f",m[i+0],m[i+1],m[i+2],m[i+3]);
            qDebug(t);
        }
    }


    Matrix()
    {

    }

    static Matrix identityMatrix()
    {
        Matrix output;

        for (int i=0; i<16; ++i)
            output.m[i] = 0.0f;
        output.m[0] = 1.0f;
        output.m[5] = 1.0f;
        output.m[10] = 1.0f;
        output.m[15] = 1.0f;

        return output;
    }

    static Matrix translationMatrix(float x, float y, float z)
    {
        Matrix output = identityMatrix();

        output.m[3] = x;
        output.m[7] = y;
        output.m[11] = z;

        return output;
    }

    static Matrix rotationMatrix(float angle, float x, float y, float z)
    {
        Matrix output = identityMatrix();

        float cosA = cosf(angle);
        float sinA = sinf(angle);
        float m = 1.0f - cosA;
        output.m[0] = cosA + x*x*m;
        output.m[5] = cosA + y*y*m;
        output.m[10]= cosA + z*z*m;

        float tmp1 = x*y*m;
        float tmp2 = z*sinA;
        output.m[4] = tmp1 + tmp2;
        output.m[1] = tmp1 - tmp2;

        tmp1 = x*z*m;
        tmp2 = y*sinA;
        output.m[8] = tmp1 - tmp2;
        output.m[2] = tmp1 + tmp2;

        tmp1 = y*z*m;
        tmp2 = x*sinA;
        output.m[9] = tmp1 + tmp2;
        output.m[6] = tmp1 - tmp2;

        return output;
    }

    Matrix operator* (const Matrix &other) const
    {
        Matrix output;
        for (int i=0; i<16; ++i)
            output.m[i] = 0.0f;

        for (int i=0; i<16; i+=4)
        {
            for (int j=0; j<4; ++j)
            {
                output.m[i + j] = m[i + 0] * other.m[0 + j] + m[i + 1] * other.m[4 + j]
                                + m[i + 2] * other.m[8 + j] + m[i + 3] * other.m[12 + j];
            }
        }

        return output;
    }

    Point operator* (const Point &p) const
    {
        Point output;

        output.x = p.x*m[0] + p.y*m[1] + p.z*m[2]  + m[3];
        output.y = p.x*m[4] + p.y*m[5] + p.z*m[6]  + m[7];
        output.z = p.x*m[8] + p.y*m[9] + p.z*m[10] + m[11];

        return output;
    }

    Vector operator* (const Vector &p) const
    {
        Vector output;

        output.x = p.x*m[0] + p.y*m[1] + p.z*m[2]  + m[3];
        output.y = p.x*m[4] + p.y*m[5] + p.z*m[6]  + m[7];
        output.z = p.x*m[8] + p.y*m[9] + p.z*m[10] + m[11];

        return output;
    }


};

//GeneralisedCylinder()
class GeneralisedCylinderOperation: public Operation
{


private:


    vector<Point> branchPoints;
    vector<float> branchEndRadius;
    vector<float> branchStartRadius;
    vector<float> branchLength;

    vector<Point> curvePoints;
    vector<float> curvePointDistances;

    vector<Point> branchCurve;
    vector<float> branchRadius;
    vector<float> branchTexCoord;

    vector<Vector> branchTangents;
    vector<Vector> branchNormals1;
    vector<Vector> branchNormals2;

    Matrix transformation;

    vector< vector<Point> > renderPoints;
    vector< vector<Point> > texturePoints;



    float generateCurvePoints(Point P0, Point P1, Point P2, Point P3, float cumulativeLength = 0.0f)
    {
        float LP = Point::distance(P0,P1) + Point::distance(P1,P2) + Point::distance(P2,P3);
        float LC = Point::distance(P0,P3);

        if (LP - LC < TOLERANCE) //if points are approximately co-linear
        {
            cumulativeLength += (LP + LC) / 2;
            curvePoints.push_back(P3);
            curvePointDistances.push_back(cumulativeLength);

            return cumulativeLength;
        }
        else
        {
            Point P01 = Point::average(P0,P1);
            Point P12 = Point::average(P1,P2);
            Point P23 = Point::average(P2,P3);

            Point P012 = Point::average(P01,P12);
            Point P123 = Point::average(P12,P23);

            Point Pmid = Point::average(P012,P123);

            cumulativeLength = generateCurvePoints(P0, P01, P012, Pmid, cumulativeLength);
            cumulativeLength = generateCurvePoints(Pmid, P123, P23, P3, cumulativeLength);
            return cumulativeLength;
        }

    }


    float bezierCurveLength(Point P0, Point P1, Point P2, Point P3)
    {
        float LP = Point::distance(P0,P1) + Point::distance(P1,P2) + Point::distance(P2,P3);
        float LC = Point::distance(P0,P3);

        if (LP - LC < TOLERANCE)
        {
            return (LP + LC) / 2;
        }
        else
        {
            Point P01 = Point::average(P0,P1);
            Point P12 = Point::average(P1,P2);
            Point P23 = Point::average(P2,P3);

            Point P012 = Point::average(P01,P12);
            Point P123 = Point::average(P12,P23);

            Point Pmid = Point::average(P012,P123);

            return bezierCurveLength(P0,P01,P012,Pmid) + bezierCurveLength(Pmid,P123,P23,P3);
        }
    }

public:
    GeneralisedCylinderOperation()
    {
        transformation = Matrix::identityMatrix();
    }

    void addBranch(float startRadius, float endRadius, float length)
    {
        if (branchPoints.empty())
        {
            branchPoints.push_back(Point(0.0f, 0.0f, 0.0f));
            branchEndRadius.push_back(0.0f);
            branchStartRadius.push_back(0.0f);
            branchLength.push_back(0.0f);
        }
        branchStartRadius.push_back(startRadius);
        branchEndRadius.push_back(endRadius);
        branchLength.push_back(length);
        //transform point (0,length,0) and add to branchPoints

        branchPoints.push_back( transformation * Point(0.0f, length, 0.0f) );
    }

    void applyTranslation(float x, float y, float z)
    {
        Matrix trans = Matrix::translationMatrix(x, y, z);
        transformation = transformation * trans;
    }

    void applyRotation(float angle, float x, float y, float z)
    {
        Matrix rot = Matrix::rotationMatrix(angle, x, y, z);
        transformation = transformation * rot;
    }

    void constructCompleteBranch()
    {


        if (branchPoints.size() <= 1)
        {
            //something
            return;
        }

        Vector in;
        Vector out(0.0f, 1.0f, 0.0f);
        Point a,b,c,d;
        float startR, endR, totalLength, radius;

        branchCurve.push_back(Point(0.0f, 0.0f, 0.0f));
        branchRadius.push_back(branchStartRadius[1]);
        branchTexCoord.push_back(0.0f);

        for (int i=1; i<branchPoints.size(); ++i)
        {
            in = out;
            if (i+1 == branchPoints.size())
                out = Vector::average( Vector(branchPoints[i-1], branchPoints[i]), Vector(0.0f, 1.0f, 0.0f) );
            else
                out = Vector::average( Vector(branchPoints[i-1], branchPoints[i]), Vector(branchPoints[i], branchPoints[i+1]) );

            a = branchPoints[i-1];
            d = branchPoints[i];

            b = branchPoints[i-1];
            b.x += in.x * CURVE_FACTOR * branchLength[i];
            b.y += in.y * CURVE_FACTOR * branchLength[i];
            b.z += in.z * CURVE_FACTOR * branchLength[i];

            c = branchPoints[i];
            c.x -= out.x * CURVE_FACTOR * branchLength[i];
            c.y -= out.y * CURVE_FACTOR * branchLength[i];
            c.z -= out.z * CURVE_FACTOR * branchLength[i];

            curvePoints.clear();
            curvePointDistances.clear();

            totalLength = generateCurvePoints(a, b, c, d);

            startR = branchStartRadius[i];
            endR = branchEndRadius[i];

            for (int i=0; i<curvePoints.size(); ++i)
            {
                branchCurve.push_back(curvePoints[i]);
                radius = startR + (endR - startR) * curvePointDistances[i]/totalLength;
                branchRadius.push_back(radius);
                branchTexCoord.push_back(curvePointDistances[i]/totalLength);
            }

        }

        branchTangents.push_back(Vector(0.0f, 1.0f, 0.0f));
        for (int i=1; i<branchCurve.size()-1; ++i)
        {
            branchTangents.push_back( Vector::average( Vector(branchCurve[i-1], branchCurve[i]), Vector(branchCurve[i], branchCurve[i+1]) ) );
        }
        branchTangents.push_back(branchTangents.back());

        branchNormals1.push_back( Vector(1.0f, 0.0f, 0.0f) );
        branchNormals2.push_back( Vector(0.0f, 0.0f, 1.0f) );

        Vector B;
        float magB, theta;
        for (int i=0; i<branchCurve.size()-1; ++i)
        {
            B = Vector::cross(branchTangents[i],branchTangents[i+1]);
            magB = B.magnitude();

            if (magB < 1e-3)
            {
                branchNormals1.push_back(branchNormals1.back());
                branchNormals2.push_back(branchNormals2.back());
            }
            else
            {
                B.x /= magB;
                B.y /= magB;
                B.z /= magB;

                theta = acosf( Vector::dot(branchTangents[i],branchTangents[i+1]) );

                Matrix rot = Matrix::rotationMatrix(theta, B.x, B.y, B.z);
                branchNormals1.push_back( rot * branchNormals1.back() );
                branchNormals2.push_back( rot * branchNormals2.back() );

            }

        }

        float step = 2.0f * M_PI / CylinderSlices;

        Point temp;
        for (int b=0; b<branchCurve.size(); ++b)
        {
            radius = branchRadius[b];

            renderPoints.push_back(vector<Point>());
            texturePoints.push_back(vector<Point>());

            for (int i = 0; i < CylinderSlices; ++i)
            {
                float a = i * step;
                float x = radius * cos(a);
                float y = radius * sin(a);

                temp = ((branchNormals1[b] * x) + (branchNormals2[b] * y)) + branchCurve[b];
                renderPoints[b].push_back( temp );
                texturePoints[b].push_back( Point(i * 1.0f / CylinderSlices, branchTexCoord[b], 0.0f) );
            }
        }




    }


    void Render()
    {
        Point a,b;
        Point texA,texB;
        for (int j=0; j<branchCurve.size()-1; ++j)
        {
            glBegin(GL_TRIANGLE_STRIP);
            for (int i = 0; i < CylinderSlices; ++i)
            {
                a = renderPoints[j][i];
                b = renderPoints[j+1][i];
                texA = texturePoints[j][i];
                texB = texturePoints[j+1][i];
                if (texA.y > 0.999)
                    glTexCoord2d(texA.x, 0.0f);
                else
                    glTexCoord2d(texA.x, texA.y);
                glVertex3f(a.x, a.y, a.z);
                glTexCoord2d(texB.x, texB.y);
                glVertex3f(b.x, b.y, b.z);
            }
            a = renderPoints[j][0];
            b = renderPoints[j+1][0];
            texA = texturePoints[j][0];
            texB = texturePoints[j+1][0];
            if (texA.y > 0.999)
                glTexCoord2d(1.0f, 0.0f);
            else
                glTexCoord2d(1.0f, texA.y);
            glVertex3f(a.x, a.y, a.z);
            glTexCoord2d(1.0f, texB.y);
            glVertex3f(b.x, b.y, b.z);
            glEnd();
        }
    }


};







#endif // OPERATION_H
