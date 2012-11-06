#include "vector2.h"

Vector2D::Vector2D()
{
    x = 0;
    y = 0;
}
Vector2D::Vector2D(int xPos, int yPos)
{
    x = (float)xPos;
    y = (float)yPos;
}
Vector2D::Vector2D(float xPos, float yPos)
{
    x = xPos;
    y = yPos;
}
int Vector2D::xInt()
{
    return (int) x;
}

int Vector2D::yInt()
{
    return (int) y;
}
Vector2D Vector2D::operator = (const Vector2D v)
{
    x=v.x;
    y=v.y;
    return *this;
}

Vector2D Vector2D::operator /= (float value)
{
    x/=value;
    y/=value;
    return *this;
}
Vector2D Vector2D::operator *= (float value)
{
    x*=value;
    y*=value;
    return *this;
}
Vector2D Vector2D::operator -= (float value)
{
    x-=value;
    y-=value;
    return *this;
}
Vector2D Vector2D::operator += (float value)
{
    x+=value;
    y+=value;
    return *this;
}
bool Vector2D::operator ==(const Vector2D &v)
{
    return (x == v.x) && (y == v.y);
}
Vector2D Vector2D::operator -= (const Vector2D v)
{
    x-=v.x;
    y-=v.y;
    return *this;
}
Vector2D Vector2D::operator += (const Vector2D v)
{
    x+=v.x;
    y+=v.y;
    return *this;
}
