#ifndef VECTOR_H
#define VECTOR_H

#include "point.h"

class Vector
{
public:
    float x,y,z;

    Vector();
    Vector(float ax, float ay, float az);
    Vector(const Point &a, const Point &b);

    void normalize();
    static Vector average(Vector a, Vector b);
    static Vector cross(const Vector &a, const Vector &b);
    static float dot(const Vector &a, const Vector &b);
    float magnitude();
    Vector operator* (const float &m);
    Vector operator+ (const Vector &other);
    Vector operator- (const Vector &other);
    Point operator+ (const Point &other);

    void print();

};



#endif // VECTOR_H
