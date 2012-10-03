#include "vector2.h"

Vector2::Vector2()
{
    x = 0;
    y = 0;
}
Vector2::Vector2(int xPos, int yPos)
{
    x = (float)xPos;
    y = (float)yPos;
}
Vector2::Vector2(float xPos, float yPos)
{
    x = xPos;
    y = yPos;
}
int Vector2::xInt()
{
    return (int) x;
}

int Vector2::yInt()
{
    return (int) y;
}
Vector2 Vector2::operator = (const Vector2 v)
{
    x=v.x;
    y=v.y;
    return *this;
}

Vector2 Vector2::operator /= (float value)
{
    x/=value;
    y/=value;
    return *this;
}
Vector2 Vector2::operator *= (float value)
{
    x*=value;
    y*=value;
    return *this;
}
Vector2 Vector2::operator -= (float value)
{
    x-=value;
    y-=value;
    return *this;
}
Vector2 Vector2::operator += (float value)
{
    x+=value;
    y+=value;
    return *this;
}
bool Vector2::operator ==(const Vector2 &v)
{
    return (x == v.x) && (y == v.y);
}
Vector2 Vector2::operator -= (const Vector2 v)
{
    x-=v.x;
    y-=v.y;
    return *this;
}
Vector2 Vector2::operator += (const Vector2 v)
{
    x+=v.x;
    y+=v.y;
    return *this;
}
