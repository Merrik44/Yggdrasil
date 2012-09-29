#include "point.h"
#include "math.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <QDebug>


void Point::print()
{
    char t[100];
    sprintf(t,"%.2f %.2f %.2f",x,y,z);
    qDebug(t);
}

Point::Point()
{

}

Point::Point(float ax, float ay, float az) : x(ax), y(ay), z(az)
{

}

Point Point::average(const Point &a, const Point &b)
{
    return Point( (a.x + b.x) / 2,
                  (a.y + b.y) / 2,
                  (a.z + b.z) / 2 );
}

float Point::distance(const Point &a, const Point &b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return sqrtf(dx*dx + dy*dy + dz*dz);
}
