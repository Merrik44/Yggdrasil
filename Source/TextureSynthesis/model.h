#ifndef MODEL_H
#define MODEL_H

#include "vmath.h"
#include <string>
#include "vector"
#include "iostream"


struct Face;
struct Vertex;
struct Edge;


struct Vertex
{
    std::vector<Edge*> edges;
    std::vector<Vertex*> neighbours;
    std::vector<Face*> faces;
    int index;
    Vector3f position;
    Vector3f normal;
    int weight;

    // joint stuff
    std::string loopIndex;
    Vector3f loopCenter;
    Vector3f loopNormal;

    Vertex( float a, float b, float c, int Index)
    {
        position.x = a;
        position.y = b;
        position.z = c;
        index = Index;
        loopIndex = "";
        weight=1;
    }

    Vertex( Vector3f Position, int Index)
    {
        position=Position;
        index = Index;
        loopIndex = "";
        weight=1;
    }

    Vertex( Vertex* vertexToCopy )
    {
        position = vertexToCopy->position;
        index = vertexToCopy->index;
        loopIndex = vertexToCopy->loopIndex;
        weight=vertexToCopy->weight;
    }

    ~Vertex()
    {
        edges.clear();
        neighbours.clear();
        faces.clear();
    }
};

struct Face
{
    Vertex* vertices[4];
    std::vector<Edge*> edges;
    int id;
    Vector3f normal;

    Face( Vertex* a, Vertex* b, Vertex* c )
    {
        vertices[0] = a;
        vertices[1] = b;
        vertices[2] = c;
        vertices[3] = NULL;

    }

    Face( Vertex* a, Vertex* b, Vertex* c, Vertex* d )
    {
        vertices[0] = a;
        vertices[1] = b;
        vertices[2] = c;
        vertices[3] = d;
    }



    ~Face()
    {
        edges.clear();
        vertices[0] = NULL;
        vertices[1] = NULL;
        vertices[2] = NULL;
        vertices[3] = NULL;
    }

public:
    void calculateNormal( )
    {
        Vector3f A = vertices[0]->position;
        Vector3f B = vertices[1]->position;
        Vector3f C = vertices[2]->position;
        normal = -(A - B ).crossProduct( C- B);
        normal.normalize();
    }

    void addNormalToVertices( )
    {
        for( int i = 0; i < 4; i++)
            if(vertices[i]!= NULL)
                vertices[i]->normal += normal;
    }
};

struct Edge
{
public:
    Face* faces[2];
    Vertex* vertices[2];
    Vertex* midPoint;

    Edge( Vertex* A, Vertex* B, Face* face1  )
    {
        vertices[0] = A;
        vertices[1] = B;
        faces[0] = face1;
        faces[1] = NULL;
    }

    Edge( Vertex* A, Vertex* B )
    {
        vertices[0] = A;
        vertices[1] = B;
        faces[0] = NULL;
        faces[1] = NULL;
    }

    ~Edge( )
    {
        faces[0] = NULL;
        faces[1] = NULL;
        vertices[0] = NULL;
        vertices[1] = NULL;
        midPoint = NULL;
    }

    Vertex* CreateMidPoint( int midPointIndex  )
    {


        //Edge points are constructed on each edge. These points are three eighths of the sum of the
        //two end points of the edge plus one eighth of the sum of the two other points that form the
        //two triangles that share the edge in question.

        Vector3f mid;
        //std::cout << "midV " <<  std::endl;
        // if not a boundary
        if( faces[0] != NULL && faces[1] != NULL )
        {

            // find other points
            Vertex* A = NULL;
            Vertex* B = NULL;
            for ( int i = 0; i < 3; i++)
            {
                if( faces[0]->vertices[i] != vertices[0] && faces[0]->vertices[i] != vertices[1] )
                    A = faces[0]->vertices[i];

                if( faces[1]->vertices[i] != vertices[0] && faces[1]->vertices[i] != vertices[1] )
                    B = faces[1]->vertices[i];

            }
            mid += (vertices[0]->position + vertices[1]->position)*3.0f/8.0f + (A->position + B->position)*1/8.0f;
        }
        else
            mid = (vertices[0]->position + vertices[1]->position)/2.0f; // boundary condition

        midPoint = new Vertex(mid.x, mid.y, mid.z, midPointIndex);

        return midPoint;

    }

};


class Model
{
public:
    std::string name;
    std::string path;
    Vector3f position;
    float scale;

    std::vector< Vertex* > vertices;
    std::vector< Face* > triangles;
    std::vector< Face* > quads;
    std::vector< Edge* > edges;

    std::vector< Vertex* > verticesBackup;
    std::vector< Face* > trianglesBackup;
    std::vector< Face* > quadsBackup;

    Model();
    Model( std::string fileName);
    ~Model();
    void Draw();
    void Load( std::string fileName);
    void ApplyLoopSubvision( int levels );
    void ReconstructMeshDataStructure();
    void ClearNeighourAndEdgeData();
    void ClearSubdivisions();
    int GetFaceCount( );
    void StoreMeshState();
    void RestoreMeshState();
    void ConstructGraphFromFaces( std::vector < Face* >& faces);
    void RecalculateSpecificNormals(std::vector < Face* >& faces, std::vector < Vertex* >& verticesToUpdate);
    void FillMissingFaces();

};

#endif // MODEL_H
