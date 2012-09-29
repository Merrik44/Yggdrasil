#ifndef POINT_H
#define POINT_H

class Point
{
public:

    float x, y, z;

    Point();
    Point(float ax, float ay, float az);

    static Point average(const Point &a, const Point &b);
    static float distance(const Point &a, const Point &b);

    void print();

};





#endif // POINT_H
