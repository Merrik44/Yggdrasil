#include "debug.h"

#include <QtOpenGL>
#include "iostream"

using namespace std;


void Print(string message )
{
    cout << message << endl;
}

void AddLine( Vector3f start, Vector3f end, Colours colour)
{
    Line line;
    line.end = end;
    line.start = start;
    line.colour = colour;
    lines.push_back( line);

}

void AddRay( Vector3f origin, Vector3f direction, Colours colour)
{
    Line line;
    line.end = origin + direction;
    line.start = origin;
    line.colour = colour;
    lines.push_back( line);

}
void AddPoint( Vector3f position, Colours colour)
{
    Point point;
    point.position= position;
    point.colour= colour;
    points.push_back(point);
}

void DebugClear()
{
    lines.clear();
    points.clear();
}

void DebugDraw()
{
    for ( unsigned int i = 0; i < lines.size(); i++ )
    {
        SetColour(lines[i].colour);
        DrawLine( lines[i].start, lines[i].end);
    }


    for ( unsigned int i = 0; i < points.size(); i++ )
    {
        SetColour(points[i].colour);
        DrawPoint( points[i].position );
    }
}

void DrawLine( Vector3f start, Vector3f end)
{
    glBegin(GL_LINES);
    glVertex3f(start.x, start.y, start.z );
    glVertex3f(end.x, end.y, end.z );
    glEnd( );
}


void DrawRay( Vector3f origin, Vector3f direction)
{
    glBegin(GL_LINES);
    glVertex3f(origin.x, origin.y, origin.z );
    direction += origin;
    glVertex3f(direction.x, direction.y, direction.z );
    glEnd( );
}

void DrawPoint( Vector3f start)
{
    //SetColour(RED);
    //glColor4( 0.95f, 0.207, 0.031f, 1.0f );
    //glColor4( colour );
    glEnable( GL_POINT_SMOOTH );
    glPointSize( 6.0 );

    glBegin(GL_POINTS);
    glVertex3f(start.x, start.y, start.z );
    glEnd( );
}

void SetColour( Colours colour )
{
    GLfloat glCol[] = { 0, 0, 0, 1 };
    switch( colour )
    {

    case RED:
        glCol[0] =1;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glCol);
        break;
    case BLUE:
        glCol[2] =1;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glCol);
        break;
    case GREEN:
        glCol[1] =1;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glCol);
        break;
    case CYAN:
        glCol[1] =1; glCol[2] =1;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glCol);
        break;
    case MAGENTA:
        glCol[0] =1; glCol[2] =1;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glCol);
        break;
    case YELLOW:
        glCol[0] =1; glCol[1] =1;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glCol);
        break;
    case WHITE:
        glCol[0] =1; glCol[1] =1; glCol[2] =1;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glCol);
        break;
    case BLACK:
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glCol);
        break;
    }

}
