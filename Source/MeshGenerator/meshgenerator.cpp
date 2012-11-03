// Author: Richard Pieterse
// Date: October 2012

#include "meshgenerator.h"
#include "stack"
#include "vmath.h"
#include "iostream"
#include "vector"
#include "debug.h"
#include "math.h"
#include "jointgenerator.h"
using namespace std;


#define radToDegree 57.2957795
int Count = 3;
int countdown = 3;
int faces;
float SubDivControl = 0.5f;

bool display4Texture = true;
bool displayMesh = true;
bool displayGraph = true;
bool tightJoints = true;
bool alternatingTriangulation = false;
int noOfSides = 5;

void InterpolateTexCoordsAccrosRemainingFaces( Mesh* model )
{
    for( uint i = 0; i < model->triangles.size(); i++ )
    {
        Face* face = model->triangles[i];
        if( face->texCoords[0] != Vector2f(0,0) || face->texCoords[1] != Vector2f(0,0) ||
                face->texCoords[2] != Vector2f(0,0) || face->texCoords[3] != Vector2f(0,0)) // face is already parameterised so continue
            continue;


        // find longest edge with tex coords
        // ------- sort edges by length --------
        SortByLength( face->edges );
        reverse( face->edges.begin() , face->edges.end( ));


        for( uint j = 0; j < face->edges.size(); j++ )
        {

            Edge* edge = face->edges[j];
            if(edge->faces[0] == NULL || edge->faces[1] == NULL  )
                continue;

            // ------ find opposite face ----------
            Face* oppositeFace = NULL;

            if(edge->faces[0] == face ) oppositeFace = edge->faces[1] ;
            if(edge->faces[1] == face ) oppositeFace = edge->faces[0] ;

            // ------ check that it has tex coords ------
            if( oppositeFace->texCoords[0] == Vector2f(0,0) && oppositeFace->texCoords[1] == Vector2f(0,0) &&
                    oppositeFace->texCoords[2] == Vector2f(0,0) && oppositeFace->texCoords[3] == Vector2f(0,0))

                continue;

            // ------ find corrosponding verts in opposite face ----------
            for( uint a = 0; a < 3; a++ )
            {
                face->texCoords[a] = Vector2f(-1, -1);
                for( uint k = 0; k < 3; k++ )
                {

                    if( oppositeFace->vertices[k] == face->vertices[a] )
                    {
                        face->texCoords[a] = oppositeFace->texCoords[k] ;
                        //  AddPoint( oppositeFace->vertices[k]->position, RED);
                    }
                }
            }

            // --- interpolate the missing coord ---
            int indexOfMissingCoord = -1;
            for( uint a = 0; a < 3; a++ )
            {

                if( face->texCoords[a] == Vector2f(-1, -1) )
                    indexOfMissingCoord = a;
            }



            int bIndex = (indexOfMissingCoord + 1)%3;
            int cIndex = (indexOfMissingCoord + 2)%3;

            Vector2f& texA =face->texCoords[indexOfMissingCoord];
            Vector2f& texB =face->texCoords[bIndex];
            Vector2f& texC =face->texCoords[cIndex];

            Vertex* vertA =face->vertices[indexOfMissingCoord];
            Vertex* vertB =face->vertices[bIndex];
            Vertex* vertC =face->vertices[cIndex];

            Vector3f BA = vertA->position - vertB->position;
            Vector3f BC =  vertC->position - vertB->position;
            Vector3f CA =  vertA->position - vertC->position;

            float lengthBA = BA.length();
            float lengthBC = BC.length();
            float lengthCA = CA.length();

            BA/=lengthBA;
            BC /=lengthBC;
            CA /=lengthCA;
            float theta = -acos(BC.dotProduct(BA));

            Vector2f bc = texC - texB;


            float length_bc = bc.length();
            float length_ba = (lengthBA)*(length_bc/lengthBC);

            bc /= length_bc;
            bc *= length_ba;

            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            Vector2f ba;
            ba.x = bc.x*cosTheta - bc.y*sinTheta;
            ba.y = bc.x*sinTheta + bc.y*cosTheta;

            texA = texB + ba;
            break;
        }
    }
}


void CreateLoop( int sides, float radius, Matrix3f rotation, Vector3f offset, float texY,
                 Mesh* model, vector< vector< Vertex*>*>& vertexLoops, vector< vector< Vector2f>* >& texLoops )
{
    vector<Vertex*>* loop = new vector<Vertex*>(sides);
    vector<Vector2f>* texLoop = new vector<Vector2f>(sides);

    // --- for every vertex that must be added to the loop ---
    for (int i = 0; i < sides; i++)
    {
        // vertices are created in a circle
        float interval = PI*2/sides;
        float x = cos( interval*i);
        float z = sin( interval*i);

        Vector3f position(x, 0, z);

        position *= radius;
        position = rotation*position;

        position += offset;

        (*loop)[i] = new Vertex( position, model->vertices.size() );

        model->vertices.push_back( (*loop)[i] );

        float texX = (float)i/(sides);

        float tileY = 1;
        (*texLoop)[i] = Vector2f(   texX, texY*tileY );
    }

    // --- push the created loops onto the list of loops ---
    vertexLoops.push_back(loop);
    texLoops.push_back(texLoop);
}



// this funtion creates faces between a set of vertex loops

void GenerateFaces(  Mesh* model, vector< vector< Vertex*>*>& vertexLoops, vector< vector< Vector2f>* >& texLoops, BranchNode* branch  )
{
    int sides = noOfSides;

    // ---------------- for every consecutive pair of loops --------------
    for( uint m = 0; m <vertexLoops.size() -1; m++ )
    {
        // --- start loop ---
        vector< Vertex*>& startLoop = *vertexLoops[m];
        vector< Vertex*>& endLoop = *vertexLoops[m+1];
        // --- end loops ---
        vector< Vector2f>&  startTexLoop = *texLoops[m];
        vector< Vector2f>&  endTexLoop = *texLoops[m+1];

        // --- for every side create two triangles---
        for (int i = 0; i < sides; i++)
        {
            // ---- get the vertices ----
            Vertex* A = startLoop[i];
            Vertex* B = startLoop[(i+1)%sides];
            Vertex* C = endLoop[i];
            Vertex* D = endLoop[(i+1)%sides];

            // ---- get the texture coordinates ----
            Vector2f texA   = startTexLoop[i];
            Vector2f texB   = startTexLoop[(i+1)%sides];
            Vector2f texC   = endTexLoop[i];
            Vector2f texD   = endTexLoop[(i+1)%sides];
            Vector2f zero;

            // ---- handle wrapping textures ----
            if( i == sides - 1)
            {
                texB.x +=1;
                texD.x +=1;
            }


            // ----- create the faces with clockwise winding ------
            Face * ABC ;
            Face * BCD ;

            ABC = new Face( B, A, C ) ;
            BCD = new Face( B, C, D ) ;

            ABC->SetTextureCoords( texB,  texA, texC, zero);
            BCD->SetTextureCoords( texB,texC, texD, zero);


            model->triangles.push_back( ABC );
            model->triangles.push_back( BCD );


            // ----- Store the starting and ending face loops ----
            if( m == 0  )
            {
                branch->startFaces.push_back(ABC);
                branch->startFaces.push_back(BCD);
            }

            if(  m ==  vertexLoops.size() -2 )
            {
                branch->endFaces.push_back(ABC);
                branch->endFaces.push_back(BCD);
            }
        }
    }

}

// This function creates the polygons for a branh of the tree
// These polygons are added to a provided model

void GenerateBranch( Mesh* model, BranchNode* branch, float& vOffset )
{

    int sides = noOfSides;

    // ------- oredered lists of the vertex loops and the texture loops -------
    vector< vector< Vector2f>* > texLoops;
    vector< vector< Vertex*>*> vertexLoops;

    Vector3f offset;
    float ratio, radius;
    float averageRadius =(branch->startRadius + branch->endRadius)/2.0f;
    int noOfSegments = ceil(branch->length/(averageRadius*PI));
    float circumference = 2*averageRadius*PI;
    float numberOfTexTiles = branch->length/circumference;

    // ------- apply the claculates offests so that the branches do not intersect -----
    Vector3f startPos = branch->startPosition + branch->direction*branch->startOffset;
    Vector3f endPos = branch->endPosition - branch->direction*branch->endOffset;

    // ----------------------- Create all of the loops -------------------------
    float vTexCoordinate = 0;
    for( int i = 0; i < noOfSegments+1; i++)
    {
        ratio = (float)i/((float)noOfSegments);
        radius = branch->startRadius + (branch->endRadius - branch->startRadius )*ratio;

        offset =   startPos + (endPos -  startPos)*ratio;
        vTexCoordinate = vOffset + (numberOfTexTiles)*ratio;

        CreateLoop( sides, radius, branch->rotation,offset, vTexCoordinate, model, vertexLoops, texLoops );
    }
    vOffset += (numberOfTexTiles)*ratio;

    // ---------------------- create all of the faces between the loops ------------------------
    GenerateFaces( model,vertexLoops, texLoops, branch );

    // ----------------- store the two vertext oops at each end  -----------------
    vector< Vertex*>& startVertices = branch->startVertices;
    vector< Vertex*>&  endVertices = branch->endVertices;

    vector< Vertex*>  start = *vertexLoops[0];
    startVertices.clear();
    for( unsigned int i = 0; i < start.size(); i++)
        startVertices.push_back(start[i]);

    vector< Vertex*>  end =  *vertexLoops[vertexLoops.size() - 1];
    endVertices.clear();
    for( unsigned int i = 0; i < end.size(); i++)
        endVertices.push_back(end[i]);

    vector< Vertex*>& SecondFromStart = branch->Vertices2ndFromStart;
    vector< Vertex*>&  SecondFromEnd = branch->Vertices2ndFromEnd;

    vector< Vertex*>  secondStart = *vertexLoops[1];
    SecondFromStart.clear();
    for( unsigned int i = 0; i < secondStart.size(); i++)
        SecondFromStart.push_back(secondStart[i]);

    vector< Vertex*>  secondEnd =  *vertexLoops[vertexLoops.size() - 2];
    SecondFromEnd.clear();
    for( unsigned int i = 0; i < secondEnd.size(); i++)
        SecondFromEnd.push_back(secondEnd[i]);

}


Vertex* mergeAwithB2( Vertex* A, Vertex* B, vector< Face* > incomingBranchFace )
{
    // ------- Add A to B ---------

    B->position += A->position;
    B->normal += A->normal;

    // ------ Normalize B, which now represents the merged point-----
    B->position /=2;
    B->normal /= 2;


    // ----- update the faces that were pointing at A to point to B ----
    for( uint j = 0; j < incomingBranchFace.size(); j++)
    {
        Face* face = incomingBranchFace[j];
        for( uint k = 0; k < 4; k++)
        {
            if( face->vertices[k] == A)
                face->vertices[k] = B;
        }

    }

    return B;
}



Vector3f TrimIncomingBranches( vector< vector< Vertex* >* >& boundaries, vector< Vector3f >& loopNormals, vector< float >& offsets, Vector3f center )
{

    int length = loopNormals.size();

    // --------------- find the furthest offset ------------

    float maxOffset = 0;

    for (int a = 0; a < length; a++)
        maxOffset = max(maxOffset,offsets[a]);

    maxOffset *= 1.55f; // make it a little larger fo luck


    // --------------- for each branch ------------
    for (int a = 0; a < length; a++)
    {
        // --------------off set everything-----------

        vector< Vertex* >& loop = *boundaries[a];

        for (uint i = 0; i < loop.size(); i++)
        {
            Vertex* vertex = loop[i];
            vertex->finalPosition = vertex->position;
        }
    }
    // ----------- loops centers -----------------
    vector<Vector3f> loopCenters;
    for (unsigned int i = 0; i < boundaries.size(); i++)
    {
        Vector3f loopCenter;
        vector< Vertex* >& loop = *boundaries[i];
        for (unsigned int j = 0; j < loop.size(); j++)
            loopCenter += loop[j]->position;

        loopCenter /= loop.size();
        loopCenters.push_back(loopCenter);
    }

    // ----------- rotate loops so that they are orthoganal to the geo center -------------------
    for (unsigned int i = 0; i < boundaries.size(); i++)
    {

        vector< Vertex* >& loop = *boundaries[i];

        //  Vector3f translation = direction* offsets[a]*1.01f;
        Vector3f maxTranslation = loopNormals[i]* maxOffset*1.01;
        maxTranslation -= ( loopCenters[i] -center );

        //   AddLine(geometricCenter+ maxTranslation, geometricCenter, BLACK );
        for (unsigned int j = 0; j < loop.size(); j++)
        {
            Vertex* A = loop[j];
            A->position += maxTranslation;

        }
    }

    return center;

}

extern int faces, Count;
void MergeTwoBondaries(vector<Vertex*>& loopA, vector<Vertex*>& loopB, std::vector<Face *> incomingBranchFaces )
{

    // make copies
    vector< Vertex* > loop1(loopA);
    vector< Vertex* > loop2(loopB);
    vector< Vertex* > newLoop;
    // --- this assumes that both loops have the same number of vertices ---
    int iterations= loop1.size();
    for ( int i = 0; i < iterations; i++)
    {
        int closestVertexPair[2] = { -1, -1 };
        float closestSqrDistance = 10000000;
        for ( unsigned int a = 0; a < loop1.size(); a++)
        {
            Vertex* A = loop1[a];
            for ( unsigned int b = 0; b < loop2.size(); b++)
            {
                Vertex* B = loop2[b];

                float sqrDist = (A->position -B->position).lengthSq();
                if( sqrDist < closestSqrDistance )
                {
                    closestSqrDistance = sqrDist;
                    closestVertexPair[0] = a;
                    closestVertexPair[1] = b;
                }
            }
        }

        // ------------------- escape case-------------------------
        if(closestVertexPair[0] == -1 )
        {
            cout << "No closest vert found" << endl;
            continue;
        }

        // -- merge vertices ---
        int indexA = closestVertexPair[0];
        int indexB = closestVertexPair[1];
        Vertex* A = loop1[indexA ];
        Vertex* B = loop2[indexB ];

        loop1.erase( loop1.begin() + indexA);
        loop2.erase( loop2.begin() + indexB);

        //AddLine(A->position, B->position, RED);

        B = mergeAwithB2(A, B, incomingBranchFaces);
        newLoop.push_back(B);



    }


}


Mesh* generateMesh( vector<BranchNode*>& branches,  QProgressDialog* progressBar)
{


    // --------------------- debug stuff ------------------------
    cout << "beginning mesh generation" << endl;
    countdown = Count;
    DebugClear();



    if(branches.size() == 0  )
        return NULL;

    // ------------------ create new model ----------------------
    Mesh* model = new Mesh();

    // ------------- generate all the branch sections -----------
    float vTexStartOffset = 0;                                              // This offset ensures that the testure is tiled down the length of the brancg
    for(int i = 0; i < branches.size(); i++)
    {
        BranchNode* branch = branches[i];
        if(branch->parent != NULL && branch->parent->children.size() > 1)   // if the segment is starts at a joint, start tiling again from 0
            vTexStartOffset = 0;

        GenerateBranch(model,  branches[i], vTexStartOffset );
    }





    // ------------------ Create the joints of the mesh ------------------

    int noOfBranches = branches.size();
    int branchesProcessed =0;
    int jointsConstructed = 0;

    BranchNode* current;
    stack<BranchNode* > branchesToProcess;
    branchesToProcess.push( branches[0] );

    while( branchesToProcess.size() > 0 )
    {
        current = branchesToProcess.top();
        branchesToProcess.pop();

        // ---------------------- Update progress bar ---------------------
        branchesProcessed++;
        if(progressBar != NULL)
            progressBar->setValue(( branchesProcessed*100)/noOfBranches );


        // ------ generate a mesh for each child and add it to the stack -------

        for (unsigned int i = 0; i < current->children.size(); i++)
        {
            branchesToProcess.push(current->children[i]);
        }


        // --------------- create the joint ---------------------------------
        if( current->children.size() != 0)
        {
            // ----------- prepare the neccesary data for trimming ---------------

            vector< Vector3f > loopNormals;
            vector< float > offsets;
            vector< vector< Vertex* >* > boundaries;
            vector< Face* > incomingBranchFaces;
            vector< Vertex* > otherVertices;


            boundaries.push_back(&current->endVertices);
            loopNormals.push_back(-current->direction);
            offsets.push_back(current->endOffset );
            for (unsigned int i = 0; i < current->endFaces.size(); i++)
                incomingBranchFaces.push_back( current->endFaces[i] );
            for (unsigned int i = 0; i < current->Vertices2ndFromEnd.size(); i++)
                otherVertices.push_back(  current->Vertices2ndFromEnd[i] );



            for (unsigned int i = 0; i < current->children.size(); i++)
            {
                boundaries.push_back( &current->children[i]->startVertices);
                loopNormals.push_back(current->children[i]->direction);
                offsets.push_back(current->children[i]->startOffset );
                for (unsigned int f = 0; f < current->children[i]->startFaces.size(); f++)
                    incomingBranchFaces.push_back( current->children[i]->startFaces[f] );
                for (unsigned int f = 0; f < current->children[i]->Vertices2ndFromStart.size(); f++)
                    otherVertices.push_back(  current->children[i]->Vertices2ndFromStart[f] );

            }


            // --------- Trim the branches so that the dont intersect --------


            Vector3f geometricCenter = TrimIncomingBranches(boundaries, loopNormals, offsets, current->endPosition);

            // --- If there is only one child, then simply merge the closest vertices ---
            if( current->children.size() == 1)
            {
                MergeTwoBondaries( *boundaries[0], *boundaries[1], incomingBranchFaces );

            }

            if( current->children.size() > 1)
            {


                // --------- Finally construct the joint --------
                jointsConstructed++;
                Mesh* jointMesh = GenerateJoint(boundaries, incomingBranchFaces, loopNormals);

                // --------- Triangles from joint into main model --------
                for (unsigned int i = 0; i < jointMesh->triangles.size(); i++)
                {
                    bool newFace = true;
                    for (unsigned int f = 0; f < incomingBranchFaces.size(); f++)
                    {
                        if ( incomingBranchFaces[f] == jointMesh->triangles[i] )
                        {
                            newFace = false;
                            break;
                        }

                    }
                    if(newFace)
                        model->triangles.push_back( jointMesh->triangles[i] );

                }


                // ------------- Must clear these arrays before deleting the models as they exist in the main model ------

                jointMesh->vertices.clear();
                jointMesh->triangles.clear();
                jointMesh->quads.clear();
                delete jointMesh;
            }
        }
    }

    // ------ Store the mesh state for subdivision purposes -------
    model->ClearNeighourAndEdgeData();

    model->ReconstructMeshDataStructure();

    model->CalculateNormals();

    cout << "mesh generation complete" << endl;

    // ---- This needs to run twice ----
    InterpolateTexCoordsAccrosRemainingFaces( model );
    InterpolateTexCoordsAccrosRemainingFaces( model );

    cout << "paramertization finished" << endl;

    model->StoreMeshState();

    return model;
}


