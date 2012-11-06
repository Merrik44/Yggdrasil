#ifndef Vector2_H
#define Vector2_H

class Vector2D
{
public:
    Vector2D();
    Vector2D(int xPos, int yPos);
    Vector2D(float xPos, float yPos);
    float x;
    float y;
    int xInt();
    int yInt();
    Vector2D operator = (const Vector2D v);
    Vector2D operator /= (float value);
    Vector2D operator *= (float value);
    Vector2D operator += (float value);
    Vector2D operator -= (float value);
    Vector2D operator -= (const Vector2D v);
    Vector2D operator += (const Vector2D v);
    bool operator ==(const Vector2D &v);
};

#endif // Vector2_H
