#ifndef Vector2_H
#define Vector2_H

class Vector2
{
public:
    Vector2();
    Vector2(int xPos, int yPos);
    Vector2(float xPos, float yPos);
    float x;
    float y;
    int xInt();
    int yInt();
    Vector2 operator = (const Vector2 v);
    Vector2 operator /= (float value);
    Vector2 operator *= (float value);
    Vector2 operator += (float value);
    Vector2 operator -= (float value);
    Vector2 operator -= (const Vector2 v);
    Vector2 operator += (const Vector2 v);
    bool operator ==(const Vector2 &v);
};

#endif // Vector2_H
