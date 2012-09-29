#ifndef DISPLAYBRANCH_H
#define DISPLAYBRANCH_H

#include <iostream>
#include <fstream>

#include <string>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <QtOpenGL/QGLWidget>

#include "point.h"
#include "vector.h"
#include "matrix.h"


class DisplayBranch
{

private:

    std::vector<Point> branchPoints;
    std::vector<float> branchEndRadius;
    std::vector<float> branchStartRadius;
    std::vector<float> branchLength;

    std::vector<Point> curvePoints;
    std::vector<float> curvePointDistances;

    std::vector<Point> branchCurve;
    std::vector<float> branchRadius;
    std::vector<float> branchTexCoord;

    std::vector<Vector> branchTangents;
    std::vector<Vector> branchNormals1;
    std::vector<Vector> branchNormals2;


    std::vector< std::vector<Point> > renderPoints;
    std::vector< std::vector<Vector> > renderPointNormals;
    std::vector< std::vector<Point> > texturePoints;

    Vector startNormal;
    Vector endNormal;

    bool hasInVector;
    Vector inVector;
    float parentRadius;
    DisplayBranch *branchParent;
    bool renderable;

    Vector baseNormal1;
    Vector baseNormal2;

    std::vector< std::vector<DisplayBranch*> > children;
    std::vector< std::vector<float> > childRelativePosition;

    float generateCurvePoints(Point P0, Point P1, Point P2, Point P3, float cumulativeLength = 0.0f);
    float bezierCurveLength(Point P0, Point P1, Point P2, Point P3);

    void moveToPoint(const Point &p);

    Matrix transformation;

public:
    //DisplayBranch();
    DisplayBranch(DisplayBranch *parent=0, float relativePosition = 0.0f);
    ~DisplayBranch();
    void addBranch(float startRadius, float endRadius, float length);
    void applyTranslation(float x, float y, float z);
    void applyRotation(float angle, float x, float y, float z);
    void constructCompleteBranch();
    void Render();
    void modifyGravity(float &x, float &y, float &z, int &count);
    void findBounds(float cx, float cy, float cz, float &mx, float &my, float &mz);

    void writeToObj(std::ofstream &fout);

    void setInVector(Vector &in);



};

#endif // DISPLAYBRANCH_H
