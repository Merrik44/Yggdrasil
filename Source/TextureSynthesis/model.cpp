#include "model.h"
#include "iostream"
#include "fstream"
#include "debug.h"

#include "sstream"
#include <QtOpenGL>
//#include "helper.h"
#include <stdlib.h>
#include <math.h>
using namespace std;


Model::Model(string fileName)
{
    scale = 1.0f;
    Load(fileName);
}

Model::Model()
{
    scale = 1.0f;
}

void Model::Load( std::string fileName)
{
    path = fileName;
    cout << "Loading model: " << fileName << endl;

    ifstream myfile(fileName.c_str());


    if (myfile.is_open())
    {
        string line;
        string token;

        while ( myfile.good() )
        {
            token = "";
            getline (myfile,line);
            stringstream sstream(line);
            sstream >> token;
            if( token == "g")
            {
                sstream >> name;
            }
            else if( token == "v")
            {
                float a, b,c ;
                sstream >> token;
                a = atof(token.c_str());
                sstream >> token;
                b = atof(token.c_str());
                sstream >> token;
                c = atof(token.c_str());

                int index = vertices.size();
                vertices.push_back( new Vertex(a,b,c, index));
            }
            else if( token == "f")
            {
                int a, b,c, d =-1;

                sstream >> token;
                a = atof(token.c_str()) - 1;
                Vertex* vertA = vertices[a];

                sstream >> token;
                b = atof(token.c_str()) - 1;
                Vertex*  vertB = vertices[b];

                sstream >> token;
                c = atof(token.c_str()) - 1;
                Vertex*  vertC = vertices[c];

                sstream >> ws;
                if( sstream.good() ) // if there is a 4th vertex
                {
                    sstream >> token;
                    d = atof(token.c_str()) - 1;
                    Vertex*  vertD = vertices[d];

                    quads.push_back(new Face(vertA, vertB, vertC, vertD));
                }
                else
                    triangles.push_back(new Face(vertA, vertB, vertC));

            }
        }
        myfile.close();

        StoreMeshState();
        ReconstructMeshDataStructure( );

    }

    else cout << "Unable to open file" << fileName <<endl;
}

int Model::GetFaceCount( )
{
    int count = 0;

    count += triangles.size();
    count += quads.size();
    //cout << count << endl;
    return count;
}

void Model::StoreMeshState()
{
    // make a backup of vertices
    for ( uint i =0; i < verticesBackup.size(); i++)
        delete verticesBackup[i];

    //  verticesBackup.reserve( vertices.size() );
    verticesBackup.clear();

    for ( uint i =0; i < vertices.size(); i++)
        verticesBackup.push_back( new Vertex( vertices[i] ) );




    // make a backup of triangles
    for ( uint i =0; i < trianglesBackup.size(); i++)
        delete trianglesBackup[i];

    // trianglesBackup.reserve( triangles.size() );
    trianglesBackup.clear();


    for ( uint i =0; i < triangles.size(); i++)
    {
        int a = triangles[i]->vertices[0]->index;
        Vertex * A = verticesBackup[a];

        int b = triangles[i]->vertices[1]->index;
        Vertex * B = verticesBackup[b];

        //cout << triangles[i]->vertices[1]->position << endl;
        // cout << B->position << endl;

        int c = triangles[i]->vertices[2]->index;
        Vertex * C = verticesBackup[c];

        Face* ABC = new Face(A, B, C);
        ABC->id = triangles[i]->id;


        trianglesBackup.push_back(ABC);

    }




    // make a backup of quads
    for ( uint i =0; i < quadsBackup.size(); i++)
        delete quadsBackup[i];
    quadsBackup.clear();

    // quadsBackup.reserve( quads.size() );

    for ( uint i =0; i < quads.size(); i++)
    {
        int a = quads[i]->vertices[0]->index;
        Vertex * A = verticesBackup[a];

        int b = quads[i]->vertices[1]->index;
        Vertex * B = verticesBackup[b];

        int c = quads[i]->vertices[2]->index;
        Vertex * C = verticesBackup[c];

        int d = quads[i]->vertices[3]->index;
        Vertex * D = verticesBackup[d];

        quadsBackup.push_back(new Face(A, B, C, D) );
    }
}

void Model::RestoreMeshState()
{
    if( verticesBackup.size() == 0)
        cout << "Unable to restor mesh, no state save" << endl;

    // make a backup of vertices
    for ( uint i =0; i < vertices.size(); i++)
        delete vertices[i];
    vertices.clear();

    //vertices.reserve( verticesBackup.size() );

    for ( uint i =0; i < verticesBackup.size(); i++)
        vertices.push_back( new Vertex( verticesBackup[i] ) );


    // make a backup of triangles
    for ( uint i =0; i < triangles.size(); i++)
        delete triangles[i];
    triangles.clear();

    //  triangles.reserve( trianglesBackup.size() );

    for ( uint i =0; i < trianglesBackup.size(); i++)
    {
        int a = trianglesBackup[i]->vertices[0]->index;
        Vertex * A = vertices[a];

        int b = trianglesBackup[i]->vertices[1]->index;
        Vertex * B = vertices[b];

        int c = trianglesBackup[i]->vertices[2]->index;
        Vertex * C = vertices[c];

        Face* ABC = new Face(A, B, C);
        ABC->id = trianglesBackup[i]->id;
        triangles.push_back(ABC);

    }

    // make a backup of quads
    for ( uint i =0; i < quads.size(); i++)
        delete quads[i];
    quads.clear();

    // quads.reserve( quadsBackup.size() );

    for ( uint i =0; i < quadsBackup.size(); i++)
    {
        int a = quadsBackup[i]->vertices[0]->index;
        Vertex * A = vertices[a];

        int b = quadsBackup[i]->vertices[1]->index;
        Vertex * B = vertices[b];

        int c = quadsBackup[i]->vertices[2]->index;
        Vertex * C = vertices[c];

        int d = quadsBackup[i]->vertices[2]->index;
        Vertex * D = vertices[d];

        quads.push_back(new Face(A, B, C, D) );
    }
}

void Model::ReconstructMeshDataStructure( )
{
    // calculate Face Normals
    for ( uint i =0; i < quads.size(); i++)
        quads[i]->calculateNormal();
    for ( uint i =0; i < triangles.size(); i++)
        triangles[i]->calculateNormal();



    // reset vertex normals
    for ( uint i =0; i < vertices.size(); i++)
        vertices[i]->normal = Vector3f();

    // calculate Vertex Normals
    for ( uint i =0; i < quads.size(); i++)
        quads[i]->addNormalToVertices();
    for ( uint i =0; i < triangles.size(); i++)
        triangles[i]->addNormalToVertices();


    for ( uint i =0; i < vertices.size(); i++)
        vertices[i]->normal.normalize();


    // construct the data stucture
    ConstructGraphFromFaces(quads);
    ConstructGraphFromFaces(triangles);

}

void  RecalculateNormalsFaces(vector < Face* >& faces)
{

    //    // calculate Face Normals
    //    for ( uint i =0; i < faces.size(); i++)
    //        faces[i]->calculateNormal();


    //    for ( uint i =0; i < faces.size(); i++)
    //    {
    //        faces->vertices[0]->normal = Vector3f();
    //    }

    //    // reset vertex normals
    //    for ( uint i =0; i < verticesToUpdate.size(); i++)
    //        verticesToUpdate[i]->normal = Vector3f();

    //    // calculate Vertex Normals
    //    for ( uint i =0; i < faces.size(); i++)
    //        faces[i]->addNormalToVertices();

    //    for ( uint i =0; i < verticesToUpdate.size(); i++)
    //        verticesToUpdate[i]->normal.normalize();

}

void Model::ConstructGraphFromFaces( vector < Face* >& faces)
{
    for ( int i =0; i < (int)faces.size(); i++)
    {
        Face* face = faces[i];

        // is it a triangle or a quad
        int sides = 4;
        if(  face->vertices[3] == NULL)
            sides = 3 ;

        for( int j = 0; j< sides; j++ )
        {
            Vertex* A =  face->vertices[j];
            Vertex* B =  face->vertices[(j+1)%sides];

            // add face
            A->faces.push_back(face );

            //  add neigbours and construct edges
            vector<Vertex*>::iterator result = find( A->neighbours.begin(), A->neighbours.end(), B );
            if( result == A->neighbours.end() )
            {
                A->neighbours.push_back(B);
                B->neighbours.push_back(A);

                // create a new edge
                Edge* edge = new Edge(A, B, face);
                face->edges.push_back(edge);
                A->edges.push_back( edge );
                B->edges.push_back( edge );
                edges.push_back(edge);

            }
            else
            {
                // edge already exists, just need to find it and tell it about this face
                for ( uint k =0; k < A->edges.size(); k++)
                {
                    Edge* edge =  A->edges[k];
                    if(edge->vertices[0] == B || edge->vertices[1] == B )
                    {
                        edge->faces[1] = face;
                        face->edges.push_back(edge);
                    }
                }
            }
        }

    }
}


Model::~Model()
{
    cout << "BOOM! model was blown to bits (destroyed)"<< endl;
    for (uint i = 0; i < vertices.size(); i++)
        delete vertices[i];

    for (int i = 0; i < (int)triangles.size(); i++)
    {
        delete triangles[i];
        triangles[i] = NULL;
    }
    for (int i = 0; i < (int)quads.size(); i++)
        delete quads[i];
    for (int i = 0; i < (int)edges.size(); i++)
    {
        delete edges[i];
        edges[i] = NULL;
    }
    triangles.clear();
    edges.clear();


    // delete backups
    for ( uint i =0; i < verticesBackup.size(); i++)
        delete verticesBackup[i];
    for ( uint i =0; i < trianglesBackup.size(); i++)
        delete trianglesBackup[i];
    for ( uint i =0; i < quadsBackup.size(); i++)
        delete quadsBackup[i];
}

void Model::ClearNeighourAndEdgeData()
{

    // clear edge data
    for ( unsigned int i = 0; i < triangles.size(); i++)
        triangles[i]->edges.clear();
    for ( unsigned int i = 0; i < quads.size(); i++)
        quads[i]->edges.clear();

    for ( unsigned int i = 0; i < edges.size(); i++)
    {
        delete edges[i];
        edges[i] = NULL;
    }

    edges.clear();


    // clear face data
    for ( int i = 0; i < (int)vertices.size(); i++ )
    {
        vertices[i]->neighbours.clear();
        vertices[i]->edges.clear();
        vertices[i]->faces.clear();
    }
}


void Model::Draw()
{
    SetColour( CYAN );
    glPushMatrix();
    {
        glTranslatef( position.x,  position.y,  position.z);
        if( true )
        {
            glBegin( GL_TRIANGLES );
            {
                for ( int j = 0; j < (int)triangles.size(); j++ ) // for each triangle
                {
                    Face* tri = triangles[j];

                    for ( int k = 0; k < 3; k++ )
                    {

                        Vector3f normal = tri->vertices[k]->normal;
                        // Vector3f normal = tri->normal;
                        glNormal3f( normal.x, normal.y, normal.z ); // this needs to be fixed
                        // glTexCoord2f( pTri->m_s[k], pTri->m_t[k] );
                        Vector3f vert = tri->vertices[k]->position;
                        vert *= scale;
                        glVertex3f( vert.x, vert.y, vert.z );

                        // cout << vert << endl;
                    }
                }
            }
            glEnd();

            glBegin( GL_QUADS );
            {
                for ( int j = 0; j < (int)quads.size(); j++ ) // for each triangle
                {
                    const Face* quad = quads[j];

                    for ( int k = 0; k < 4; k++ )
                    {

                        Vector3f normal = quad->normal;
                        glNormal3f( normal.x, normal.y, normal.z ); // this needs to be fixed
                        // glTexCoord2f( pTri->m_s[k], pTri->m_t[k] );
                        Vector3f vert = quad->vertices[k]->position;
                        vert *= scale;
                        glVertex3f( vert.x, vert.y, vert.z );
                    }
                }

                glEnd();
            }

            SetColour(RED);
                        for ( uint i =0; i < edges.size(); i++)
                            DrawLine(edges[i]->vertices[0]->position*scale, edges[i]->vertices[1]->position*scale);

            //            for ( uint i =0; i < vertices.size(); i++)
            //                DrawPoint(vertices[i]->position *scale);
            // SetColour(RED);
            //for ( uint i =0; i < vertices.size(); i++)
            // DrawRay(vertices[i]->position,vertices[i]->normal*0.05f );
        }
        glPopMatrix();
    }
}




void Model::ApplyLoopSubvision( int levels)
{
    for( int n = 0; n < levels; n++ )
    {

        int oldVerticesLength = vertices.size();
        // step 1: generate the midpoints

        for( uint i = 0; i < edges.size(); i++ )
        {

            Edge* edge =  edges[i];
            int index = vertices.size();
            Vertex* midpoint = edge->CreateMidPoint(index) ;

            vertices.push_back(midpoint );
        }

        // step 2: create the new faces. NB Specifically for triangles/loop

        int oldLength = triangles.size(); // have to store this because we are adding faces to the vector
        for( int i = 0; i < oldLength; i++ )
        {
            Face* oldFace;
            oldFace = triangles[i];

            for( uint j = 0; j < 3; j++ )
            {

                Edge* AB = oldFace->edges[j];
                Edge* BC = oldFace->edges[(j+1)%3];
                Vertex* A = AB->midPoint;
                Vertex* B;
                Vertex* C = BC->midPoint;

                if( AB->vertices[0] == BC->vertices[0]  ) // find the shared vertex
                    B = AB->vertices[0];

                else if( AB->vertices[1] == BC->vertices[0]  ) // find the shared vertex
                    B = AB->vertices[1];

                else if( AB->vertices[1] == BC->vertices[1]  ) // find the shared vertex
                    B = AB->vertices[1];

                else if( AB->vertices[0] == BC->vertices[1]  ) // find the shared vertex
                    B = AB->vertices[0];

                Face* newFace = new Face(A, B, C);
                newFace->calculateNormal();
                triangles.push_back(newFace);

            }

            // set the oldface to be in the center
            for( int j = 0; j < 3; j++ )
                oldFace->vertices[j] = oldFace->edges[j]->midPoint;

        }
        //  cout << "step2"  << endl;
        // Vertex points are constructed for each old vertex. A given vertex has n neighbor vertices.
        // The new vertex point is one minus n times s times the old vertex, plus s times the sum of the
        //neighboring vertices, where s is a scaling factor. For n equal to three, s is three sixteenths.
        // For n greater than three, s is 1/n (5/8 - (3/8 + 1/4 cos(2π / n))2)

        for( int j = 0; j < oldVerticesLength; j++ )
        {
            Vertex* vertex = vertices[j];

            // check if it lies on a boundary
            vector < Edge* > edgesWithOneFace;
            for( uint k = 0; k < vertex->edges.size(); k++ )
            {
                if( vertex->edges[k]->faces[1] == NULL )
                    edgesWithOneFace.push_back(vertex->edges[k] );
            }

            if( edgesWithOneFace.size() != 0 ) // boundary case
            {
                Vector3f averageSum;
                for( uint k = 0; k < edgesWithOneFace.size(); k++ )
                {
                    if(edgesWithOneFace[k]->vertices[0] != vertex )
                        averageSum += edgesWithOneFace[k]->vertices[0]->position;
                    else
                        averageSum += edgesWithOneFace[k]->vertices[1]->position;


                }

                averageSum /= edgesWithOneFace.size();
                vertex->position = vertex->position*3.0f/4.0f + averageSum/4.0f;

            }
            else // regular case
            {
                float s;
                int n = vertex->neighbours.size();

                if( n== 3)
                    s = 3/16.0f;
                else
                {
                    float alpha = (3.0f/8 + (1.0f/4) * cos(2.0f*PI / n));
                    alpha *= alpha;
                    s = (1.0f/n)*(5.0f/8 - alpha );
                }
                vertex->position = vertex->position*( 1 - n*s);

                Vector3f sum;
                for( int k = 0; k < n; k++ )
                    sum += vertex->neighbours[k]->position;

                vertex->position += sum*s;
            }

        }
        ClearNeighourAndEdgeData();
        ReconstructMeshDataStructure();
    }

}


void Model::FillMissingFaces()
{

    for ( int i = 0; i < edges.size(); i++)
    {
        Edge* edge = edges[i];

        if( edge->faces[1] == NULL )
        {
            vector< Edge* > possibleLeftEdges;
            vector< Edge* > possibleRightEdges;

            Vertex* vert1 =edge->vertices[0];
            Vertex* vert2 =edge->vertices[1];
             cout << "--33--" << endl;
            for( int j = 0; j < vert1->edges.size(); j++)
            {
                Edge* other = vert1->edges[j];
                if( edge == other )
                    continue;

                if( other->faces[1] == NULL  )
                    possibleLeftEdges.push_back(other);
            }
            for( int j = 0; j < vert2->edges.size(); j++)
            {
                Edge* other = vert2->edges[j];
                if( edge == other )
                    continue;

                if( other->faces[1] == NULL  )
                    possibleRightEdges.push_back(other);
            }


            // --- try find a valid combo of left and right ---
            for ( int l = 0; l < possibleLeftEdges.size(); l++ )
            {
                Edge* A = possibleLeftEdges[l];
                for ( int r = 0; r < possibleRightEdges.size(); r++ )
                {
                    Edge* B = possibleRightEdges[r];
                    Vertex* vert3 = NULL;


                    // --- if the share a vertex then they form a triangle ---

                    if( B->vertices[0] ==  A->vertices[0] )
                        vert3 = B->vertices[0];

                    else if( B->vertices[1] ==  A->vertices[1] )
                        vert3 = B->vertices[1];

                    else if( B->vertices[1] ==  A->vertices[0] )
                        vert3 = B->vertices[1];

                    else if( B->vertices[0] ==  A->vertices[1] )
                        vert3 = B->vertices[0];

                    // --- create the face ---
                    if (vert3 != NULL )
                    {
                        Face* newFace = new Face(vert2,  vert1, vert3 );
                        A->faces[1] = newFace;
                        B->faces[1] = newFace;
                        edge->faces[1] = newFace;
                        triangles.push_back(newFace);
                    }

                }
            }
        }

    }
}
