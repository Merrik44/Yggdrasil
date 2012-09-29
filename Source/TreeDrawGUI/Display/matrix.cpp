#include "matrix.h"
#include "math.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <QDebug>

void Matrix::print()
{
    char t[100];

    for (int i=0;i<16; i+=4)
    {
        sprintf(t,"%.2f %.2f %.2f %.2f",m[i+0],m[i+1],m[i+2],m[i+3]);
        qDebug(t);
    }
    qDebug() << endl;
}


Matrix::Matrix()
{

}

Matrix::Matrix(const Matrix &other)
{
    for (int i=0; i<16; ++i)
        m[i] = other.m[i];
}

Matrix Matrix::identityMatrix()
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

Matrix Matrix::translationMatrix(float x, float y, float z)
{
    Matrix output = identityMatrix();

    output.m[3] = x;
    output.m[7] = y;
    output.m[11] = z;

    return output;
}

Matrix Matrix::rotationMatrix(float angle, float x, float y, float z)
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

Matrix Matrix::operator* (const Matrix &other) const
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

Point Matrix::operator* (const Point &p) const
{
    Point output;

    output.x = p.x*m[0] + p.y*m[1] + p.z*m[2]  + m[3];
    output.y = p.x*m[4] + p.y*m[5] + p.z*m[6]  + m[7];
    output.z = p.x*m[8] + p.y*m[9] + p.z*m[10] + m[11];

    return output;
}

Vector Matrix::operator* (const Vector &p) const
{
    Vector output;

    output.x = p.x*m[0] + p.y*m[1] + p.z*m[2]  + m[3];
    output.y = p.x*m[4] + p.y*m[5] + p.z*m[6]  + m[7];
    output.z = p.x*m[8] + p.y*m[9] + p.z*m[10] + m[11];

    return output;
}


