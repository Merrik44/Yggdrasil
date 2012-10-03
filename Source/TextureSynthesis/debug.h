#ifndef DEBUG_H
#define DEBUG_H

#include "vmath.h"
#include "vector"
#include "string"

enum Colours { WHITE, BLUE, RED, GREEN, CYAN, YELLOW, MAGENTA, BLACK };

struct Line
{
public:
    Vector3f start, end;
    Colours colour;
};
struct Point
{
public:
    Vector3f position;
    Colours colour;
};


static std::vector<Line> lines;
static std::vector<Point> points;

void DrawLine( Vector3f start, Vector3f end);
void DrawRay( Vector3f origin, Vector3f direction);
void DrawPoint( Vector3f start);

void AddLine( Vector3f start, Vector3f end, Colours colour);
void AddRay( Vector3f origin, Vector3f direction, Colours colour);
void AddPoint( Vector3f position, Colours colour);

void SetColour( Colours colour );
void Print(std::string message );
void DebugDraw();
void DebugClear();

#endif // DEBUG_H
