#include "jointgenerator.h"
#include "stack"
#include "vmath.h"
#include "iostream"
#include "vector"
#include "debug.h"
#include "glwidget.h"
#include "math.h"
#include "algorithm"
#include <sys/time.h>

using namespace std;

extern int faces, Count;
bool enableProgressControl = false;



Vertex* weightedmMergeAwithedB( Vertex* A, Vertex* B)
{

//cout << A->position  << endl;
//cout << B->position  << endl;

    // ------- Add A to B ---------
    B->position*= B->weight;
    B->finalPosition*= B->weight;
    B->normal*=B->weight;


    B->position += A->position*A->weight;
    B->finalPosition += A->finalPosition*A->weight;
    B->normal += A->normal*A->weight;

    // ------ Normalize B, which now represents the merged point-----


    B->position /= B->weight +A->weight;
    B->finalPosition /= B->weight +A->weight;
    B->normal /= B->weight +A->weight;
    B->weight += A->weight;

   // cout << B->position  << endl;
   // AddPoint( A->position, GREEN);
   // AddPoint( Vector3f(0,0,0), GREEN);
   // AddLine( B->position,A->position, BLUE);
    stringstream ss;
    ss << A->position  << A->loopID;

    B->loopID = ss.str();


    return B;
}


void getRootBoundaryLoopsAndRootVertices(vector< vector<Vertex*>* >& branches, vector<vector<Vertex*>* >& rootBoundaryLoops,  vector<Vertex*>& rootVertices)
{

    for (unsigned int j = 0; j < branches.size(); j++)
    {
        vector<Vertex*>*& boundary =  branches[j];
        vector<Vertex*>* branchRootVerts = new vector<Vertex*>();
        for (unsigned int k = 0; k < boundary->size(); k++)
        {
            Vertex * vertex = (*boundary)[k];
            branchRootVerts->push_back(vertex);
            rootVertices.push_back(vertex);
        }

        // neighbors are need for calculating the visible
        int len = branchRootVerts->size();
        for (int i = 0; i < len; i++)
        {
            Vertex* left = (*branchRootVerts)[(i + 1 + len) % len];
            Vertex* right = (*branchRootVerts)[(i - 1 + len) % len];
            Vertex* current = (*branchRootVerts)[i];
            current->neighbours.push_back(left);
            current->neighbours.push_back(right);
        }


        rootBoundaryLoops.push_back(branchRootVerts);
    }

}

// ---- would like to implement quick sort at some point ----
void SortByLength( vector< Edge*>& edges )
{

    for( uint i = 0; i < edges.size(); i++)
    {
        Vector3f edgesVecA = edges[i]->vertices[0]->position - edges[i]->vertices[1]->position;
        float minSqrLength = edgesVecA.lengthSq();
        int minIndex = i;

        for( uint j = i+1; j < edges.size(); j++)
        {
            Vector3f edgesVecB = edges[j]->vertices[0]->position - edges[j]->vertices[1]->position;
            float sqLength = edgesVecB.lengthSq();
            if( sqLength < minSqrLength )
            {
                minIndex = j;
                minSqrLength = sqLength;
            }
        }

        Edge* temp = edges[i];
        edges[i] = edges[minIndex];
        edges[minIndex] = temp;
    }


}

bool DoStringsShareCharacters(string A, string B)
{
    bool sharedCharFound = false;
    for( uint k =0; k<  B.length(); k++ )
    {
        if( A.find_first_of( B[k], 0) != -1)
        {
            sharedCharFound = true;
            break;
        }
    }

    return sharedCharFound;
}

int counter2 = 300;
bool collapseCounter = true;
bool CollapseJoint(Mesh* jointModel, vector< Face* >& incomingBranchFaces)
{


    bool AtleastOneCollapse = false;
    SortByLength(jointModel->edges);
    //    for (uint i = 0; i < jointModel->edges.size(); i++ )
    //    {
    //        float length =( jointModel->edges[i]->vertices[0]->position - jointModel->edges[i]->vertices[1]->position).length();
    //        cout << length << endl;
    //    }
    vector< Face* > facesToDelete;
    for (uint i = 0; i < jointModel->edges.size(); i++ )
    {
        if(collapseCounter)
        {
            // cout << "s---" <<  counter2<< endl;
            counter2--;
            if( counter2 < 0)
                break;

            if( counter2 == 0 )
            {
              //  for (uint r = 0; r < jointModel->edges.size(); r++ )
                    //AddLine(jointModel->edges[r]->vertices[0]->position, jointModel->edges[r]->vertices[1]->position, CYAN);
            }
        }

        Edge* edge = jointModel->edges[i];



        Vertex* A = edge->vertices[0];
        Vertex* B = edge->vertices[1];


        if( DoStringsShareCharacters( A->loopID, B->loopID ) )
            continue;

        //AddLine(A->position, B->position, GREEN );
        Face* faceA = edge->faces[0];
        Face* faceB = edge->faces[1];

        if( faceA== NULL || faceB == NULL )
            continue;
        if( faceA->markedForDeletion )
            continue;

        if( faceB->markedForDeletion )
            continue;

        // ---- collapse ----

        B  = weightedmMergeAwithedB( A, B );
        AtleastOneCollapse = true;


        for( uint j = 0; j < jointModel->triangles.size(); j++)
        {
            Face* face = jointModel->triangles[j];

            for( uint k = 0; k < 3; k++)
            {

                if( face->vertices[k] == A)
                    face->vertices[k] = B;
            }

        }

        for( uint j = 0; j < incomingBranchFaces.size(); j++)
        {
            Face* face = incomingBranchFaces[j];

            for( uint k = 0; k < 3; k++)
            {

                if( face->vertices[k] == A)
                    face->vertices[k] = B;
            }

        }



        //  }
        // cout << counter2 << endl;


        //
        for (uint k = 0; k < jointModel->edges.size(); k++ )
        {
            if( jointModel->edges[k]->vertices[0] == A )
                jointModel->edges[k]->vertices[0] = B;

            if( jointModel->edges[k]->vertices[1] == A )
                jointModel->edges[k]->vertices[1] = B;
        }
        //   AddLine(jointModel->edges[r]->vertices[0]->position, jointModel->edges[r]->vertices[1]->position, CYAN);

        if( edge->faces[0]->markedForDeletion == false)
        {

            // ---- delete face and adjacent faces -----
            facesToDelete.push_back(edge->faces[0]);
            edge->faces[0]->markedForDeletion = true;

        }
        if( edge->faces[1]->markedForDeletion == false)
        {

            // ---- delete face and adjacent faces -----
            facesToDelete.push_back(edge->faces[1]);
            edge->faces[1]->markedForDeletion = true;
        }

        //jointModel->ClearNeighourAndEdgeData();
        //  jointModel->ReconstructMeshDataStructure();
        if( counter2 == 0)
            break;

    }

    for (int i = jointModel->triangles.size() -1; i >= 0; i-- )
    {
        Face* face = jointModel->triangles[i];
        for (int m = facesToDelete.size() -1; m >= 0; m-- )
        {
            if( face == facesToDelete[m] )
            {
                jointModel->triangles.erase(jointModel->triangles.begin() + i);
                facesToDelete.erase(facesToDelete.begin() + m);
                delete face;
                break;
            }
        }
    }

    // cout << "---" <<endl;

    return AtleastOneCollapse;

}

void RetriangulateSharpEdges(Mesh* jointModel)
{
    //  int counter = Count;
    for ( int m =0; m < 30; m++ )
    {
        float gratesAbs = -1;
        Edge* sharpestEdge = NULL;
        //cout << "dpt "  << endl;
        for (uint i = 0; i < jointModel->edges.size(); i++ )
        {
            Edge* edge = jointModel->edges[i];
            Face* A = edge->faces[0];
            Face* B = edge->faces[1];

            if(A == NULL || B== NULL)
                continue;

            //        if(A->reTriangulated|| B->reTriangulated)
            //            continue;

            float dot =( A->normal.dotProduct(B->normal ));
            //

            if(dot > gratesAbs)
            {
                gratesAbs = dot;
                sharpestEdge = edge;
            }
        }

        if( sharpestEdge != NULL)
        {
            //cout << "dpt " << gratesAbs << endl;
            Edge* edge = sharpestEdge;
            Face* A = edge->faces[0];
            Face* B = edge->faces[1];
            AddLine(edge->vertices[0]->position,  edge->vertices[1]->position,  MAGENTA);
            // AddPoint( (A->vertices[0]->position + A->vertices[1]->position + A->vertices[2]->position )/3.0f, MAGENTA );
            // AddPoint( (B->vertices[0]->position + B->vertices[1]->position + B->vertices[2]->position )/3.0f, MAGENTA );
            //break;
            Vertex* a = edge->vertices[0];
            Vertex* b = edge->vertices[1];
            Vertex* c = NULL;
            Vertex* d = NULL;
            for ( int j = 0; j < 3; j++  )
            {
                if( A->vertices[j] != a && A->vertices[j] != b )
                    c = A->vertices[j];

                if( B->vertices[j] != a && B->vertices[j] != b )
                    d = B->vertices[j];
            }
            Vector3f cd = d->position - c->position;
            Vector3f ca = a->position - c->position;
            Vector3f cb = b->position - c->position;

            Vector3f norm1= cd.crossProduct(ca);
            norm1.normalize();
            Vector3f norm2= cb.crossProduct(cd);

            norm2.normalize();

            float newdot =norm1.dotProduct(norm2);
            cout << newdot << endl;

            if(newdot < gratesAbs)
            {
                for (uint i = 0; i < jointModel->edges.size(); i++ )
                {
                    if( jointModel->edges[i] ==sharpestEdge );
                    jointModel->edges.erase(jointModel->edges.begin() + i);
                }
                continue;
            }
            //AddLine(c->position, d->position, RED);

            // ----- change faces-------
            A->SetVertices(a, d, c );
            B->SetVertices(b, c, d );

            A->reTriangulated = true;
            B->reTriangulated = true;

            // ----- delete old faces ---------
            //                for ( int j = 0; j <  A->edges.size(); j++  )
            //                    A->edges[j] = NULL;

            //                for ( int j = jointModel->triangles.size() - 1; j <= 0; j--  )
            //                {
            //                    if( jointModel->triangles[j] == A )
            //                        delete jointModel->triangles[j];

            //                    if( jointModel->triangles[j] == B )
            //                        delete jointModel->triangles[j];

            //                }

            jointModel->ClearNeighourAndEdgeData();
            jointModel->ReconstructMeshDataStructure();
            jointModel->CalculateNormals();



        }

        //          if(counter <= 0)
        //              break;
        //          counter--;
    }


}


extern bool tightJoints;
bool retriangulate = false;

Vector3f GetHullCenter(vector< Face* >& hull)
{
    Vector3f center;
    for(int i = 0; i < hull.size(); i++)
    {
        Face* face = hull[i];
        for(int j = 0; j < 3; j++)
            center += face->vertices[j]->position;
    }
    center /= hull.size()*3;
    return center;
}


Mesh* GenerateJoint2(vector< vector<Vertex*>* >& branches, vector< Face* >& incomingBranchFaces, vector< Vertex* >& otherVertices, Vector3f center)
{


    faces = Count;

    // ------------------- data structures -----------------
    vector< vector<Vertex*>* > rootBoundaryLoops; // dont forget to delete the internal vector
    vector<Vertex*> rootVertices;

    // ----------------- get the root vertices and the the root loops ----------------
    getRootBoundaryLoopsAndRootVertices(branches, rootBoundaryLoops, rootVertices);

    Mesh* jointModel = new Mesh();
    for (unsigned int k = 0; k < rootVertices.size(); k++)
        jointModel->vertices.push_back( rootVertices[k] );


    jointModel->ClearNeighourAndEdgeData();
    jointModel->ReconstructMeshDataStructure();

    // ------------------ initialize the visible vertices to everything -------------
    for (unsigned int k = 0; k < rootVertices.size(); k++)
    {
        rootVertices[k]->calculateVisible(rootVertices);


        rootVertices[k]->Id = k;
    }


    // ------------------- Initialize Edges----------------------------

    for (unsigned int i = 0; i < rootBoundaryLoops.size(); i++)
    {
        vector<Vertex*>& loop = *rootBoundaryLoops[i];
        for (unsigned int k = 0; k < loop.size(); k++)
        {
             stringstream sstream;
            // get the vertices
            Vertex* v1 = loop[k];
            v1->loopIndex = i;
            sstream << k;
            sstream >> v1->loopID;
        }


    }




    // ----------- Initialize Polyhedron --------------



    vector<Vertex*> initialPolyhedron;
    for(int i = 0; i < 4; i++)
    {
        int loopIndex = i%rootBoundaryLoops.size();
        int index = i - loopIndex;
        Vertex* vert = (*rootBoundaryLoops[loopIndex])[index];
        initialPolyhedron.push_back(vert);
    }


    Face* face;
    face = new Face( initialPolyhedron[0], initialPolyhedron[1], initialPolyhedron[2] );
    jointModel->triangles.push_back(face);
    face = new Face( initialPolyhedron[3], initialPolyhedron[1], initialPolyhedron[2] );
    jointModel->triangles.push_back(face);
    face = new Face( initialPolyhedron[0], initialPolyhedron[3], initialPolyhedron[2] );
    jointModel->triangles.push_back(face);
    face = new Face( initialPolyhedron[0], initialPolyhedron[1], initialPolyhedron[3] );
    jointModel->triangles.push_back(face);

    //  cout << "DD DD " << rootVertices.size()<< endl;

    // --------- earse vertices from initial polyhedron -----------
    for( int i = rootVertices.size() - 1; i >= 0; i--)
    {
        for(int j = 0; j < 4; j++)
        {
            if( rootVertices[i] == initialPolyhedron[j] )
                rootVertices.erase(rootVertices.begin() + i);
        }
    }

    jointModel->ClearNeighourAndEdgeData();
    jointModel->ReconstructMeshDataStructure();

    Vector3f hullCenter = GetHullCenter( jointModel->triangles );
    for(int i = 0; i < jointModel->triangles.size(); i++)
    {
        jointModel->triangles[i]->UpdateWinding(hullCenter);
    }



    // --------------- create convex hull -----------------
    // cout << "DD DD " << rootVertices.size()<< endl;
    for(int i = 0; i < rootVertices.size(); i++)
    {
        Vertex* vertexToAdd = rootVertices[i];
        //  AddPoint( vertexToAdd->position, MAGENTA);

        faces--;

        // if( faces <= 0 )
        //    continue;


        for( int j = jointModel->triangles.size() - 1; j >= 0; j--)
        {

            Face* faceToTest = jointModel->triangles[j];
            Vector3f centroid = faceToTest->vertices[0]->position + faceToTest->vertices[1]->position +faceToTest->vertices[2]->position;
            centroid /= 3;
            //   AddPoint(centroid, RED);
            Vector3f ray = centroid - vertexToAdd->position;
            if( faces == 1  )
            {
                //  AddRay(centroid, faceToTest->normal*0.1f, YELLOW);


            }

            ray.normalize();
            float dot = ray.dotProduct(faceToTest->normal);
            if( dot >= 0 )
                continue;

            jointModel->triangles.erase( jointModel->triangles.begin() + j);
        }
        jointModel->ClearNeighourAndEdgeData();
        jointModel->ReconstructMeshDataStructure();

        for(int j = 0; j < jointModel->edges.size(); j++)
        {

            Edge* edge = jointModel->edges[j];
            if( edge->faces[0] != NULL && edge->faces[1] != NULL)
            {
                continue;
            }
            if( edge->vertices[0]->index == edge->vertices[1]->index)
                continue;

            if( faces == 1 )
            {
                //AddLine( edge->vertices[1]->position,  edge->vertices[0]->position, CYAN );
              //  AddLine( vertexToAdd->position,  edge->vertices[0]->position, GREEN );
               // AddLine( edge->vertices[1]->position,  vertexToAdd->position, GREEN );
            }
            Face* newFace = new Face(edge->vertices[0], edge->vertices[1], vertexToAdd );


            jointModel->triangles.push_back(newFace);

            Vector3f hullCenter = GetHullCenter( jointModel->triangles );
            newFace->UpdateWinding(hullCenter);
        }

    }


    // --------------- remove illegal triangles ----------------
    for( int j = jointModel->triangles.size() - 1; j >= 0; j--)
    {
        Face* faceToTest = jointModel->triangles[j];
        if( faceToTest->vertices[0]->loopIndex == faceToTest->vertices[1]->loopIndex && faceToTest->vertices[0]->loopIndex == faceToTest->vertices[2]->loopIndex)
            jointModel->triangles.erase( jointModel->triangles.begin() + j);

    }

    counter2 = Count;
    for( int n = 0; n < 20; n++)
    {
        jointModel->ClearNeighourAndEdgeData();
        jointModel->ReconstructMeshDataStructure();
        bool unstable = CollapseJoint( jointModel, incomingBranchFaces );
        if( unstable == false)
            break;
        if( n == 10)
            cout << "warning: too many joint collapse" << endl;
    }

    if( tightJoints )
    {
        for (unsigned int i = 0; i <branches.size(); i++)
        {
            vector< Vertex*>&  loop = *branches[i];
            for (unsigned int j = 0; j <loop.size(); j++)
                loop[j]->position = loop[j]->finalPosition;
        }
    }


    //    if( retriangulate)
    //    {
    //        RetriangulateSharpEdges(jointModel );
    //    }



    jointModel->ClearNeighourAndEdgeData();
    jointModel->ReconstructMeshDataStructure();




    for (unsigned int i = 0; i < rootBoundaryLoops.size(); i++)
        delete rootBoundaryLoops[i];



    //for( int j = jointModel->triangles.size() - 1; j >= 0; j--)
    //  cout <<  jointModel->triangles[j]->vertices[0] << " " <<  jointModel->triangles[j]->vertices[1] << " " <<  jointModel->triangles[j]->vertices[2] <<  endl;

    return jointModel;

}
