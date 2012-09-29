#ifndef MATRIX_H
#define MATRIX_H

#include "point.h"
#include "vector.h"

class Matrix
{
public:
    float m[16];


    Matrix();
    Matrix(const Matrix &other);

    static Matrix identityMatrix();

    static Matrix translationMatrix(float x, float y, float z);
    static Matrix rotationMatrix(float angle, float x, float y, float z);
    Matrix operator* (const Matrix &other) const;
    Point operator* (const Point &p) const;
    Vector operator* (const Vector &p) const;
    void print();
};


#endif // MATRIX_H
