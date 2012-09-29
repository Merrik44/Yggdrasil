#include "displaybranch.h"

#include <string>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <QDebug>

const int CylinderSlices = 16;


using namespace std;



const float TOLERANCE = 1e-5;
const float CURVE_FACTOR = 3e-1;
const float TEXTURE_LENGTH_FACTOR = 5.0f;
const char* FILE_OUTPUT_FORMAT_SPECIFIER = "%.6f";




float DisplayBranch::generateCurvePoints(Point P0, Point P1, Point P2, Point P3, float cumulativeLength)
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


float DisplayBranch::bezierCurveLength(Point P0, Point P1, Point P2, Point P3)
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


DisplayBranch::~DisplayBranch()
{

}


DisplayBranch::DisplayBranch(DisplayBranch *parent, float relativePosition)
{
    renderable = false;
    hasInVector = false;
    if (parent != 0)
    {
        DisplayBranch *populatedParent = parent;
        while (populatedParent != 0 && populatedParent->branchPoints.empty())
        {
            populatedParent = populatedParent->branchParent;
        }
        if (populatedParent != 0)
        {
            parent = populatedParent;
        }
        transformation = Matrix(parent->transformation);
        if (!parent->branchPoints.empty())
        {
            hasInVector = true;
            int s = parent->branchPoints.size();
            inVector = Vector(parent->branchPoints[s-2], parent->branchPoints[s-1]);
            inVector.normalize();
            baseNormal1 = transformation * Vector(1.0f, 0.0f, 0.0f) - transformation * Vector(0.0f, 0.0f, 0.0f);
            baseNormal2 = transformation * Vector(0.0f, 0.0f, 1.0f) - transformation * Vector(0.0f, 0.0f, 0.0f);

            parentRadius = parent->branchEndRadius[s-1];
        }
        if (parent->branchPoints.size() > 1)
        {
            parent->children[parent->branchPoints.size()-1].push_back(this);
            parent->childRelativePosition[parent->branchPoints.size()-1].push_back(relativePosition);
        }
    }
    else
    {
        transformation = Matrix::identityMatrix();
    }
    branchParent = parent;
}

void DisplayBranch::addBranch(float startRadius, float endRadius, float length)
{
    if (length < 0.01)
    {
        return;
    }
    bool mustPostTransformByRadius = false;
    if (branchPoints.empty())
    {
        branchPoints.push_back( transformation * Point(0.0f, 0.0f, 0.0f));
        children.push_back(vector<DisplayBranch*>());
        childRelativePosition.push_back(vector<float>());

        branchEndRadius.push_back(0.0f);
        branchStartRadius.push_back(0.0f);
        branchLength.push_back(0.0f);

        if (!hasInVector)
        {
            baseNormal1 = transformation * Vector(1.0f, 0.0f, 0.0f) - transformation * Vector(0.0f, 0.0f, 0.0f);
            baseNormal2 = transformation * Vector(0.0f, 0.0f, 1.0f) - transformation * Vector(0.0f, 0.0f, 0.0f);
        }
        else
        {
            length += parentRadius;
            mustPostTransformByRadius = true;
        }
    }
    branchStartRadius.push_back(startRadius);
    branchEndRadius.push_back(endRadius);
    branchLength.push_back(length);
    //transform point (0,length,0) and add to branchPoints

    branchPoints.push_back( transformation * Point(0.0f, length, 0.0f) );
    children.push_back(vector<DisplayBranch*>());
    childRelativePosition.push_back(vector<float>());

    if (mustPostTransformByRadius)
    {
        applyTranslation(0.0f, parentRadius, 0.0f);
    }

}

void DisplayBranch::applyTranslation(float x, float y, float z)
{
    Matrix trans = Matrix::translationMatrix(x, y, z);
    transformation = transformation * trans;
}

void DisplayBranch::applyRotation(float angle, float x, float y, float z)
{
    Matrix rot = Matrix::rotationMatrix(angle, x, y, z);
    transformation = transformation * rot;
}

void DisplayBranch::constructCompleteBranch()
{

    if (branchPoints.size() <= 1)
    {
        //something
        return;
    }

    Vector in;
    //CHANGE?
    //Vector out(0.0f, 1.0f, 0.0f);
    Vector out = Vector(branchPoints[0], branchPoints[1]);
    if (hasInVector)
        out = inVector;
    out.normalize();

    Point a,b,c,d, temp;
    float startR, endR, totalLength, radius, curveLength, previousTex, length;

    branchCurve.push_back(branchPoints[0]);
    curvePoints.push_back(branchPoints[0]);
    branchRadius.push_back(branchStartRadius[1]);
    branchTexCoord.push_back(0.0f);
    previousTex = 0.0f;

    totalLength = 0.0f;
    for (int i=1; i<(int)(branchPoints.size()); ++i)
    {
        in = out;
        if (i+1 == (int)(branchPoints.size()))
        {
            //out = Vector(branchPoints[i-1], branchPoints[i]);
            out = Vector::average( Vector(branchPoints[i-1], branchPoints[i]), out );
            out.normalize();
        }
        else
            out = Vector::average( Vector(branchPoints[i-1], branchPoints[i]), Vector(branchPoints[i], branchPoints[i+1]) );

        a = branchPoints[i-1];
        d = branchPoints[i];

        //CHANGE CONTINUITY? AVERAGE FROM PREVIOUS BRANCH LENGTH AS WELL?
        b = branchPoints[i-1];
        b.x += in.x * CURVE_FACTOR * branchLength[i];
        b.y += in.y * CURVE_FACTOR * branchLength[i];
        b.z += in.z * CURVE_FACTOR * branchLength[i];

        c = branchPoints[i];
        c.x -= out.x * CURVE_FACTOR * branchLength[i];
        c.y -= out.y * CURVE_FACTOR * branchLength[i];
        c.z -= out.z * CURVE_FACTOR * branchLength[i];

        temp = curvePoints.back();
        curvePoints.clear();
        curvePoints.push_back(temp);

        curvePointDistances.clear();
        curvePointDistances.push_back(0.0f);

        curveLength = generateCurvePoints(a, b, c, d);

        startR = branchStartRadius[i];
        endR = branchEndRadius[i];

        for (int j=1; j<(int)(curvePoints.size()); ++j)
        {
            radius = startR + (endR - startR) * curvePointDistances[j-1]/curveLength;
            length = curvePointDistances[j] - curvePointDistances[j-1];

            float texLength = radius * TEXTURE_LENGTH_FACTOR;

            if (length > ((1-previousTex) * texLength))
            {
                float intermediateLength = (1-previousTex) * texLength;
                Point newPoint;
                while (intermediateLength < length)
                {
                    newPoint = Vector(curvePoints[j-1], curvePoints[j]) * (intermediateLength / length) + curvePoints[j-1];
                    radius = startR + (endR - startR) * (curvePointDistances[j-1] + intermediateLength) / curveLength;

                    branchCurve.push_back(newPoint);
                    branchRadius.push_back(radius);
                    branchTexCoord.push_back(1.0f);

                    texLength = radius * TEXTURE_LENGTH_FACTOR;
                    intermediateLength += texLength;
                }

                previousTex = 0.0f;
                intermediateLength -= texLength;
                length -= intermediateLength;

            }

            float low = curvePointDistances[j-1]/curveLength;
            float high = curvePointDistances[j]/curveLength;

            int e = children[i].size();
            for(int k = 0; k != e; k++)
            {
                if (childRelativePosition[i][k] >= low && childRelativePosition[i][k] <= high)
                {
                    Point newPoint = (Vector(curvePoints[j-1], curvePoints[j]) * childRelativePosition[i][k]) + curvePoints[j-1];
                    children[i][k]->moveToPoint(newPoint);
                }
            }

            branchCurve.push_back(curvePoints[j]);
            branchRadius.push_back(startR + (endR - startR) * curvePointDistances[j]/curveLength);
            //branchTexCoord.push_back(curvePointDistances[j] / curveLength);
            branchTexCoord.push_back(previousTex + length / texLength);

            previousTex = previousTex + length / texLength;
        }

        totalLength += curveLength;
    }

    Vector v = Vector(branchCurve[0], branchCurve[1]);
    v.normalize();
    branchTangents.push_back(v);
    for (int i=1; i<(int)(branchCurve.size())-1; ++i)
    {
        branchTangents.push_back( Vector::average( Vector(branchCurve[i-1], branchCurve[i]), Vector(branchCurve[i], branchCurve[i+1]) ) );
    }
    branchTangents.push_back(branchTangents.back());

    //branchNormals1.push_back( Vector(1.0f, 0.0f, 0.0f) );
    //branchNormals2.push_back( Vector(0.0f, 0.0f, 1.0f) );
    branchNormals1.push_back( baseNormal1 );
    branchNormals2.push_back( baseNormal2 );

    Vector B;
    float magB, theta;
    for (int i=0; i<(int)(branchCurve.size())-1; ++i)
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

    for (int b=0; b<(int)(branchCurve.size()); ++b)
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


    Vector sum;
    Vector v1,v2,v3,v4;
    int size = (int)(branchCurve.size())-1;

    renderPointNormals.push_back(vector<Vector>());

    for (int i = 0; i < CylinderSlices; ++i)
    {
        v3 = Vector(renderPoints[0][i], renderPoints[1][i]);

        if (i < CylinderSlices - 1)
            v2 = Vector(renderPoints[0][i], renderPoints[0][i+1]);
        else
            v2 = Vector(renderPoints[0][i], renderPoints[0][0]);

        if (i > 0)
            v4 = Vector(renderPoints[0][i], renderPoints[0][i-1]);
        else
            v4 = Vector(renderPoints[0][i], renderPoints[0][CylinderSlices - 1]);

        sum = Vector(0.0f, 0.0f, 0.0f);

        sum = sum + Vector::cross(v2, v3);
        sum = sum + Vector::cross(v3, v4);

        sum = sum * 0.5;

        sum.normalize();

        renderPointNormals[0].push_back(sum);
    }

    for (int b=1; b<size; ++b)
    {

        renderPointNormals.push_back(vector<Vector>());

        for (int i = 0; i < CylinderSlices; ++i)
        {
            v1 = Vector(renderPoints[b][i], renderPoints[b-1][i]);
            v3 = Vector(renderPoints[b][i], renderPoints[b+1][i]);

            if (i < CylinderSlices - 1)
                v2 = Vector(renderPoints[b][i], renderPoints[b][i+1]);
            else
                v2 = Vector(renderPoints[b][i], renderPoints[b][0]);

            if (i > 0)
                v4 = Vector(renderPoints[b][i], renderPoints[b][i-1]);
            else
                v4 = Vector(renderPoints[b][i], renderPoints[b][CylinderSlices - 1]);

            sum = Vector(0.0f, 0.0f, 0.0f);

            sum = sum + Vector::cross(v1, v2);
            sum = sum + Vector::cross(v2, v3);
            sum = sum + Vector::cross(v3, v4);
            sum = sum + Vector::cross(v4, v1);

            sum = sum * 0.25;

            sum.normalize();

            renderPointNormals[b].push_back(sum);
        }
    }

    renderPointNormals.push_back(vector<Vector>());

    for (int i = 0; i < CylinderSlices; ++i)
    {
        v1 = Vector(renderPoints[size][i], renderPoints[size-1][i]);

        if (i < CylinderSlices - 1)
            v2 = Vector(renderPoints[size][i], renderPoints[size][i+1]);
        else
            v2 = Vector(renderPoints[size][i], renderPoints[size][0]);

        if (i > 0)
            v4 = Vector(renderPoints[size][i], renderPoints[size][i-1]);
        else
            v4 = Vector(renderPoints[size][i], renderPoints[size][CylinderSlices - 1]);

        sum = Vector(0.0f, 0.0f, 0.0f);

        sum = sum + Vector::cross(v1, v2);
        sum = sum + Vector::cross(v4, v1);

        sum = sum * 0.5;

        sum.normalize();

        renderPointNormals[size].push_back(sum);
    }

    startNormal = Vector::cross( Vector(renderPoints[0][0], renderPoints[0][2]), Vector(renderPoints[0][0], renderPoints[0][1]) );
    startNormal.normalize();

    endNormal = Vector::cross( Vector(renderPoints[size][0], renderPoints[size][1]), Vector(renderPoints[size][0], renderPoints[size][2]) );
    endNormal.normalize();


    renderable = true;

}


void DisplayBranch::Render()
{
    Point a,b,c;
    Vector an,bn,cn;
    Point texA,texB;

    if (!renderable)
        return;

    //bottom
    glBegin(GL_TRIANGLES);
    c = branchCurve[0];
    glNormal3f(startNormal.x, startNormal.y, startNormal.z);
    glTexCoord2d(0.0f, 0.0f);
    for (int i = 0; i < CylinderSlices-1; ++i)
    {
        a = renderPoints[0][i];
        b = renderPoints[0][i+1];
        glVertex3f(c.x, c.y, c.z);
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
    }
    a = renderPoints[0][CylinderSlices-1];
    b = renderPoints[0][0];
    glVertex3f(c.x, c.y, c.z);
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glEnd();

    //trunk
    int s = (int)(branchCurve.size())-1;
    for (int j=0; j<s; ++j)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i < CylinderSlices; ++i)
        {
            a = renderPoints[j][i];
            b = renderPoints[j+1][i];
            an = renderPointNormals[j][i];
            bn = renderPointNormals[j+1][i];
            texA = texturePoints[j][i];
            texB = texturePoints[j+1][i];
            if (texA.y > 0.999)
                glTexCoord2d(texA.x, 0.0f);
            else
                glTexCoord2d(texA.x, texA.y);
            glNormal3f(an.x, an.y, an.z);
            glVertex3f(a.x, a.y, a.z);
            glTexCoord2d(texB.x, texB.y);
            glNormal3f(bn.x, bn.y, bn.z);
            glVertex3f(b.x, b.y, b.z);
        }
        a = renderPoints[j][0];
        b = renderPoints[j+1][0];
        an = renderPointNormals[j][0];
        bn = renderPointNormals[j+1][0];
        texA = texturePoints[j][0];
        texB = texturePoints[j+1][0];
        if (texA.y > 0.999)
            glTexCoord2d(1.0f, 0.0f);
        else
            glTexCoord2d(1.0f, texA.y);
        glNormal3f(an.x, an.y, an.z);
        glVertex3f(a.x, a.y, a.z);
        glTexCoord2d(1.0f, texB.y);
        glNormal3f(bn.x, bn.y, bn.z);
        glVertex3f(b.x, b.y, b.z);
        glEnd();
    }

    //top
    glBegin(GL_TRIANGLES);
    c = branchCurve[s];
    glNormal3f(endNormal.x, endNormal.y, endNormal.z);
    glTexCoord2d(0.0f, 0.0f);
    for (int i = 0; i < CylinderSlices-1; ++i)
    {
        a = renderPoints[s][i];
        b = renderPoints[s][i+1];
        glVertex3f(c.x, c.y, c.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(a.x, a.y, a.z);
    }
    a = renderPoints[s][CylinderSlices-1];
    b = renderPoints[s][0];
    glVertex3f(c.x, c.y, c.z);
    glVertex3f(b.x, b.y, b.z);
    glVertex3f(a.x, a.y, a.z);
    glEnd();
}


void DisplayBranch::setInVector(Vector &in)
{
    hasInVector = true;
    inVector = Vector(in.x, in.y, in.z);
}

void DisplayBranch::modifyGravity(float &x, float &y, float &z, int &count)
{
    if (branchPoints.size() > 1)
    {
        Point p;
        for (int i=1; i<branchPoints.size(); ++i)
        {
            p = branchPoints[i];
            x += p.x;
            y += p.y;
            z += p.z;
        }
        count += branchPoints.size() - 1;
    }
}

void DisplayBranch::findBounds(float cx, float cy, float cz, float &mx, float &my, float &mz)
{
    if (branchPoints.size() > 1)
    {
        Point p;
        for (int i=0; i<branchPoints.size(); ++i)
        {
            p = branchPoints[i];
            mx = max(mx, fabsf(cx-p.x));
            my = max(my, fabsf(cy-p.y));
            mz = max(mz, fabsf(cz-p.z));
        }
    }
}

const char* VERTEX_FORMAT_SPECIFIER = "v %.6f %.6f %.6f";
const char* TEX_COORD_FORMAT_SPECIFIER = "vt %.6f %.6f";

void writeVertex(ofstream &fout, const Point &p)
{
    char line[256];
    sprintf(line, VERTEX_FORMAT_SPECIFIER, p.x, p.y, p.z);
    fout << line << endl;
}

void writeTextureCoord(ofstream &fout, const Point &p)
{
    char line[256];
    sprintf(line, TEX_COORD_FORMAT_SPECIFIER, p.x, p.y);
    fout << line << endl;
}

void DisplayBranch::writeToObj(ofstream &fout)
{
    char standardface[256], lastface[256], endface[256];

    if (!renderable)
        return;

    writeVertex(fout, branchCurve[0]);
    writeTextureCoord(fout, Point(0.0f, 0.0f, 0.0f));

    writeVertex(fout, renderPoints[0][CylinderSlices-1]);
    writeTextureCoord(fout, texturePoints[0][CylinderSlices-1]);

    for (int i = 0; i < CylinderSlices; ++i)
    {
        writeVertex(fout, renderPoints[0][i]);
        writeTextureCoord(fout, texturePoints[0][i]);
        sprintf(endface, "f %d/%d %d/%d %d/%d", -i-3, -i-3, -2, -2, -1, -1 );
        fout << endface << endl;
    }



    for (int i = 0; i < CylinderSlices; ++i)
    {
        writeVertex(fout, renderPoints[0][i]);
        writeTextureCoord(fout, texturePoints[0][i]);
    }
    sprintf(standardface, "f %d/%d %d/%d %d/%d %d/%d", -CylinderSlices-2, -CylinderSlices-2, -CylinderSlices-1, -CylinderSlices-1,
                                                       -1, -1, -2, -2);

    sprintf(lastface, "f %d/%d %d/%d %d/%d %d/%d", -1, -1, -CylinderSlices, -CylinderSlices,
                                                   -CylinderSlices-CylinderSlices, -CylinderSlices-CylinderSlices, -CylinderSlices-1, -CylinderSlices-1);



    int s = (int)(branchCurve.size());
    for (int j=1; j<s; ++j)
    {
        writeVertex(fout, renderPoints[j][0]);
        writeTextureCoord(fout, texturePoints[j][0]);
        for (int i = 1; i < CylinderSlices; ++i)
        {
            writeVertex(fout, renderPoints[j][i]);
            writeTextureCoord(fout, texturePoints[j][i]);
            fout << standardface << endl;
        }
        fout << lastface << endl;
    }

    --s;
    writeVertex(fout, branchCurve[s]);
    writeTextureCoord(fout, Point(0.0f, 0.0f, 0.0f));

    writeVertex(fout, renderPoints[s][CylinderSlices-1]);
    writeTextureCoord(fout, texturePoints[s][CylinderSlices-1]);

    for (int i = 0; i < CylinderSlices; ++i)
    {
        writeVertex(fout, renderPoints[s][i]);
        writeTextureCoord(fout, texturePoints[s][i]);
        sprintf(endface, "f %d/%d %d/%d %d/%d", -i-3, -i-3, -2, -2, -1, -1 );
        fout << endface << endl;
    }
}


void DisplayBranch::moveToPoint(const Point &p)
{
    if (branchPoints.size() > 0)
    {
        Vector d(branchPoints[0], p);
        Matrix shift = Matrix::translationMatrix(d.x, d.y, d.z);
        int e = branchPoints.size();
        for (int i=0; i<e; ++i)
        {
            branchPoints[i] = shift * branchPoints[i];
        }
    }
}

