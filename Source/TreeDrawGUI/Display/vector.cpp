#include "vector.h"
#include "math.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <QDebug>


void Vector::print()
{
    char t[100];
    sprintf(t,"%.2f %.2f %.2f",x,y,z);
    qDebug(t);
}

Vector::Vector()
{

}

Vector::Vector(float ax, float ay, float az) :
    x(ax), y(ay), z(az)
{

}

Vector::Vector(const Point &a, const Point &b) :
    x(b.x-a.x), y(b.y-a.y), z(b.z-a.z)
{

}

void Vector::normalize()
{
    float m = sqrtf(x*x + y*y + z*z);
    x /= m;
    y /= m;
    z /= m;
}

Vector Vector::average(Vector a, Vector b)
{
    a.normalize();
    b.normalize();

    return Vector((a.x+b.x)/2, (a.y+b.y)/2, (a.z+b.z)/2);
}

Vector Vector::cross(const Vector &a, const Vector &b)
{
    return Vector( a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x );
}

float Vector::dot(const Vector &a, const Vector &b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

float Vector::magnitude()
{
    return sqrtf(x*x + y*y + z*z);
}

Vector Vector::operator* (const float &m)
{
    return Vector( x*m, y*m, z*m );
}

Vector Vector::operator+ (const Vector &other)
{
    return Vector( x + other.x, y + other.y, z + other.z );
}

Vector Vector::operator- (const Vector &other)
{
    return Vector( x - other.x, y - other.y, z - other.z );
}

Point Vector::operator+ (const Point &other)
{
    return Point( x + other.x, y + other.y, z + other.z );
}
