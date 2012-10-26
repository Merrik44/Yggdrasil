// Author: Richard Pieterse
// Date: October 2012

#include "lstskeleton.h"
#include "stack"
#include "QtOpenGL"
#include "fstream"
#include "debug.h"
#include "math.h"
#include "stdlib.h"
#include <boost/algorithm/string.hpp>

#define radToDegree 57.2957795

using namespace boost;
using namespace std;
bool simplifyGraph = true;

LstSkeleton::LstSkeleton(string filepath)
{
    scale = 1;
    LoadLstFile( filepath );

}


LstSkeleton::LstSkeleton()
{
    scale = 1;

}

LstSkeleton::~LstSkeleton()
{
    cout << "LST file has been destroyed" << endl;
    for ( int i = 0; i < (int)branches.size(); i++ )
    {
        delete branches[i];

    }
}


void LstSkeleton::LoadLstFile( string path )
{
    cout << "Loading lst file: " << filepath << endl;
    filepath = path;
    ifstream myfile(filepath.c_str());

    if (myfile.is_open())
    {

        cout << "Loading lst file: Open " << filepath << endl;

        string command;

        stack<BranchNode*> branchStack;
        stack<Vector3f> positionStack;
        stack<Matrix3f> rotationStack;


        BranchNode* current = NULL;
        Vector3f currentPosition;
        Matrix3f currentRotation;
        root = NULL;

        while ( myfile.good() )
        {
            getline (myfile,command);

            vector< string > tokens;
            // boost library function
            erase_all(command, " ");
            trim(command);
            split( tokens, command, is_any_of(" (),") );
            if( tokens.size() > 0 )
            {


                if (tokens[0] == "Cylinder")
                {
                    float startRadius = (float)atof(tokens[1].c_str());
                    float endRadius = (float)atof(tokens[2].c_str());
                    float length = (float)atof(tokens[3].c_str());

                    current = new BranchNode( currentPosition, currentRotation, startRadius, endRadius, length, current);
                    branches.push_back(current);
                    if (root == NULL)
                        root = current;
                }
                else if (tokens[0] == "RotRel3f")
                {

                    float angle = -(float)atof(tokens[1].c_str());
                    Vector3f axisAngle;
                    axisAngle.x = (float)atof(tokens[2].c_str());
                    axisAngle.y = (float)atof(tokens[3].c_str());
                    axisAngle.z = (float)atof(tokens[4].c_str());
                    axisAngle *= angle*radToDegree;
                    Matrix3f newRot;
                    newRot.identity();
                    newRot = newRot.createRotationAroundAxis(axisAngle.x, axisAngle.y, axisAngle.z);
                    currentRotation = currentRotation*newRot;


                }
                else if (tokens[0] == "MovRel3f")
                {
                    Vector3f translation;
                    translation.x = atof(tokens[1].c_str());
                    translation.y = atof(tokens[2].c_str());
                    translation.z = atof(tokens[3].c_str());
                    translation = currentRotation*translation;
                    currentPosition +=  translation;

                }
                else if (tokens[0] == "SB")
                {
                    branchStack.push(current);
                    positionStack.push(currentPosition);
                    rotationStack.push(currentRotation);
                }
                else if (tokens[0] == "EB")
                {
                    current = branchStack.top();
                    branchStack.pop();
                    currentPosition = positionStack.top();
                    positionStack.pop();
                    currentRotation = rotationStack.top();
                    rotationStack.pop();

                }
            }
        }
    }

    // scale graph
    for ( int i = 0; i < (int)branches.size(); i++ )
    {
        BranchNode* branch = branches[i];
        branch->length *= scale;
        branch->startPosition*=scale;
        branch->endPosition*=scale;
        branch->endRadius*=scale;
        branch->startRadius*=scale;
    }


    // find the tips of all the branches
    for ( int i = 0; i < (int)branches.size(); i++ )
    {
        if (  branches[i]->children.size() == 0)
            branchTips.push_back(branches[i]);

    }

    // ----- Calculate offsets so that branches do not intersect -----
    for ( int i = 0; i < (int)branches.size(); i++ )
        CalculateBranchOffsets(*branches[i]);

    if(simplifyGraph)
        SimplifyGraph();

    BezierCurves(root);

}


void LstSkeleton::createRandomBifurcation(int numberOfBranches, float branchLength, float maxRadius, float minRadius )
{

    if( branches.size() > 0)
    {
        cout << "cannot generate bifurcation, graph already exists" << endl;
        return;
    }

    // --- create trunk ----
    float rad;
    Vector3f start;
    Matrix3f rotation;
    rad = minRadius +(maxRadius - minRadius)*(random()%1000)/1000.0f;
    root = new BranchNode( start, rotation, rad, rad, branchLength, NULL );
    branches.push_back(root);

    // --- create branches ---
    for( int i = 0; i < numberOfBranches; i++ )
    {
        start = root->endPosition;
        rad = minRadius +(maxRadius - minRadius)*(random()%1000)/1000.0f;
        rotation.identity();
        //  cout << rad << endl;
        rotation = rotation.createRotationAroundAxis(((random()%1000)/1000.0f)*360, ((random()%1000)/1000.0f)*360 , ((random()%1000)/1000.0f)*360  );
        // cout << rotation.toString() << endl;
        BranchNode* branch = new BranchNode( start, rotation, rad, rad, branchLength, root );
        branches.push_back(branch);


    }

    // scale graph
    for ( int i = 0; i < (int)branches.size(); i++ )
    {
        BranchNode* branch = branches[i];
        branch->length *= scale;
        branch->startPosition*=scale;
        branch->endPosition*=scale;
        branch->endRadius*=scale;
        branch->startRadius*=scale;
    }

    // find the tips of all the branches
    for ( int i = 0; i < (int)branches.size(); i++ )
    {
        if (  branches[i]->children.size() == 0)
            branchTips.push_back(branches[i]);

    }

    // ----- Calculate offsets so that branches do not intersect -----
    for ( int i = 0; i < (int)branches.size(); i++ )
        CalculateBranchOffsets(*branches[i]);

    if(simplifyGraph)
        SimplifyGraph();




}

void LstSkeleton::SimplifyGraph()
{




    for (int k = 0; k < 100; k++ )
    {
        bool stable = true;
        for ( int i = 0; i < (int)branches.size(); i++ )
        {
            BranchNode* branch = branches[i];

            if( TryCollapseBranch(branch) == true )
            {
                stable = false;
                delete branches[i];
                branches[i] = NULL;
            }
        }

        // ----- remove deleted branches -----
        for ( int i = (int)branches.size() - 1; i >= 0; i-- )
        {
            if( branches[i] == NULL  )
                branches.erase( branches.begin() + i );
        }


        for ( int i = 0; i < (int)branches.size(); i++ )
        {
            branches[i]->startOffset = 0;
            branches[i]->endOffset = 0;

        }
        // ----- recalculate offsets -----
        for ( int i = 0; i < (int)branches.size(); i++ )
        {
            CalculateBranchOffsets(*branches[i]);
        }

        if( stable )
            break;

        if( k == 80 )
            cout << "Graph simplification is taking way to long to stabalize, there may be something wrong" << endl;
    }




}

// takes in a section of branches, and outputs a smooth curving section
BranchSection* curvyAlgorithm( BranchSection* section)
{
    std::vector< BranchNode*>& branchNodes= section->branchNodes;
    std::vector< BranchPoint*> newNodes;


    for( int x = 0; x < 1; x++)
    {
         bool stable = false;

        for ( int j = 0; j < branchNodes.size(); j++ )
        {
            BranchNode* A = branchNodes[j];
            BranchNode* B = branchNodes[(j+1)% branchNodes.size()];
            Vector3f dirA = A->direction;
            Vector3f dirB = B->direction;

            dirA.normalize();
            dirB.normalize();

            float dot =dirB.dotProduct(-dirA);
            float tolerance = -0.8f;
            if(dot > tolerance ) // split
            {
                A->split = true;
                B->split = true;
            }
        }


        // split nodes


        newNodes.clear();
        for ( int j = 0; j < branchNodes.size(); j++ )
        {
            BranchNode* branchA = branchNodes[j];

            if( branchNodes[j]->split == false )
                continue;

            stable = false;



            BranchPoint* newPoint = new BranchPoint();
            newPoint->poistion = branchA->startPosition;;
            newPoint->radius = branchA->startRadius;;
             newNodes.push_back(newPoint);
            // ---- ignore start ---
            if(branchA->split)
            {

                BranchPoint* midPoint = new BranchPoint();
                midPoint->poistion =  branchA->endPosition -  branchA->startPosition;
                midPoint->radius =  branchA->endRadius - branchA->startRadius;
                midPoint->poistion /= 2;
                midPoint->radius /= 2;
                newNodes.push_back(midPoint);
            }

            if(j+1 != branchNodes.size())
            {
                BranchPoint* EndPoint = new BranchPoint();
                EndPoint->poistion =  branchA->endPosition;;
                EndPoint->radius = branchA->startRadius;;
                 newNodes.push_back(EndPoint);
            }



        }

        for ( int j = 0; j < newNodes.size()-1; j++ )
        {
            //BranchPoint A = newNodes[j];
           // BranchPoint B = newNodes[j+1];

          //  BranchNode* newBranch = new BranchNode(A.poistion, B.poistion,  )
        }

        if( stable)
            break;
    }


}

void LstSkeleton::BezierCurves( BranchNode* root)
{
    return;
    if( root == NULL)
        return;

    stack< BranchNode*> branchStack;
    branchStack.push(root);

    BranchSection* currentSection;

    // --- find continous branch sections ---
    // currentSection = new BranchSection();
    //   sections.push_back(currentSection);

    while( branchStack.size() > 0)
    {
        BranchNode* branch = branchStack.top();
        branchStack.pop();

        if(branch->parent == NULL || branch->parent->children.size() > 1)
        {
            currentSection = new BranchSection();
            sections.push_back(currentSection);
        }

        currentSection->branchNodes.push_back(branch);


        for( int i = 0; i <  branch->children.size(); i++)
        {
            branchStack.push(branch->children[i]);
        }


    }



    for ( int i = 0; i < (int)sections.size(); i++ )
    {
        curvyAlgorithm(sections[i]);
    }


}

bool LstSkeleton::TryCollapseBranch( BranchNode* branch )
{

    float finalSegmentlength =  branch->startOffset + branch->endOffset;
    float averageDiameter =  (branch->startRadius + branch->endOffset);

    // the 2 conditions for collapse are:
    // 1) the offsets overlap
    // or 2) the radius is less the than half the length
    //cout <<  branch->length << " > " << endl;
    // cout << finalSegmentlength << endl;
    if(  branch->length > finalSegmentlength + 0.5f*averageDiameter )
        return false;

    // ----- the branch needs to be collapsed ------

    BranchNode* parent = branch->parent;

    // ----- I dont really know what the bes action is for when the root branch mus collapse -----
    if ( parent == NULL )
        return false;

    // ------ kill pointers -----

    // ---- remove the branch from its parents children ----
    for( uint i = 0; i < parent->children.size(); i++)
    {
        if(parent->children[i] == branch)
        {
            parent->children.erase(parent->children.begin() + i);
            break;
        }
    }

    branch->parent = NULL;

    // ----- Add the branches children to the patent and update their start information ------
    for( uint i = 0; i < branch->children.size(); i++)
    {
        BranchNode* child = branch->children[i];

        child->parent = parent;
        child->startPosition = parent->endPosition;
        child->length = (child->startPosition - child->endPosition).length();

        Vector3f direction = (child->endPosition - child->startPosition);
        direction.normalize();
        child->direction = direction;

        // ---- update rotation matrix ----
        Vector3f axis = child->direction.crossProduct( Vector3f(0, 1, 0 ));
        axis.normalize();
        float dot = child->direction.dotProduct( Vector3f(0, 1, 0 ));
        float angle = acos(dot);

        if(dot == 0)
            axis = Vector3f(0, 1, 0);
        if(dot == 1)
            axis = Vector3f(0, -1, 0);


        angle*= radToDegree;

        Quatf rotQuat = Quatf::fromAxisRot(axis, -angle);
        child->rotation = rotQuat.rotMatrix();
        // child->rotation = newRot;
        //child->rotation = Matrix3f::(axis.x, axis.y, axis.z );

        parent->children.push_back(child);
    }


    return true;
}
struct Offsets
{
    float offsetA;
    float offsetB;
    Offsets( float a, float b)
    {
        offsetA = a;
        offsetB = b;
    }
};


Offsets GetOffsets( Vector3f directionA, Vector3f directionB, float radiusA, float radiusB )
{
    float offsetA = 0;
    float offsetB = 0;

    float dot = directionA.dotProduct( directionB);
    dot = max( dot, -1.0f);
    dot = min( dot, 1.0f);
    if (dot == 1)
    {
        // no solution
        offsetA =  0.0f;
        offsetB =  0.0f;
    }
    else if (dot == 0)
    {
        offsetA =  radiusB;
        offsetB =  radiusA;
    }
    else if (dot > 0)
    {

        float alpha = acos(dot);
        float os1 = radiusA/tan(alpha);
        float os2 = radiusB/sin(alpha);
        float A = os1 + os2;

        float hypotenuseSqrd = A * A + radiusA * radiusA;
        float B = sqrt(hypotenuseSqrd - radiusB * radiusB);

        offsetA =  A;
        offsetB = B;

    }
    else if (dot < 0)
    {

        float alpha = acos(dot);
        float beta = PI - alpha;
        float radius;

        //  use the shortest radius
        if( radiusB < radiusA)
            radius = radiusB;
        else
            radius = radiusA;

        float offset = radius * tan(beta/2.0f);


        offsetA = offset;
        offsetB =  offset;

    }


    return Offsets(offsetA*1.01f, offsetB*1.01f);
}

void LstSkeleton::CalculateBranchOffsets( BranchNode& branch)
{
    if( branch.children.size() == 0)
    {
        branch.endOffset = 0;
        return;
    }
    // --------- intersect parent with all children ----------
    for( unsigned int i = 0; i < branch.children.size(); i++ )
    {
        BranchNode& child = *branch.children[i];

        Vector3f directionA = -branch.direction;
        Vector3f directionB = child.direction;

        float radiusA = branch.endRadius;
        float radiusB = child.startRadius;

        Offsets offsets = GetOffsets(directionA, directionB,radiusA,radiusB  );


        branch.endOffset = max(offsets.offsetA, branch.endOffset);
        child.startOffset = max(offsets.offsetB, child.startOffset);

    }

    // --------- intersect children against each other ----------
    for( unsigned int i = 0; i < branch.children.size(); i++ )
    {
        BranchNode& childA = *branch.children[i];
        Vector3f directionA = childA.direction;
        float radiusA = childA.startRadius;
        for( unsigned int j = i + 1; j < branch.children.size(); j++ )
        {
            BranchNode& childB = *branch.children[j];
            Vector3f directionB = childB.direction;
            float radiusB = childB.startRadius;

            Offsets offsets = GetOffsets(directionA, directionB,radiusA,radiusB  );
            childA.startOffset = max(offsets.offsetA, childA.startOffset);
            childB.startOffset = max(offsets.offsetB, childB.startOffset);
        }

    }


}




extern bool displayGraph;
void LstSkeleton::Draw()
{
    //Model branchDiplay("../obj model/icosohedron.obj");
    if(!displayGraph)
        return;




    for ( int j = 0; j < (int)branches.size(); j++ )
    {
        Vector3f pos = branches[j]->startPosition;
        Vector3f posEnd = branches[j]->endPosition;
        BranchNode& branch = *branches[j];

        //  DrawPoint(pos);
        SetColour( CYAN );
        DrawLine( pos, posEnd);

        SetColour( RED );

        //         Vector3f dir(1, 1, 0);
        //         dir.normalize();
        //         dir = branch.rotation*dir;
        DrawRay( pos, branch.direction*0.1f);

        SetColour( RED );
        if(  branches[j]->children.size() > 0 )
            DrawPoint(branches[j]->endPosition);

        SetColour( BLUE );

        DrawPoint(branches[j]->endPosition - branch.direction*branch.endOffset);
        DrawPoint(branch.startPosition + branch.direction*branch.startOffset);


        //        SetColour( WHITE );

        //        DrawPoint(pos + branch.direction*branch.startOffset);

        //        SetColour( BLACK );
        //        DrawPoint(posEnd - branch.direction*branch.endOffset);
    }


    //    for ( int i = 0; i < (int)sections.size(); i++ )
    //    {
    //        for ( int j = 0; j < (int)sections[i]->branchNodes.size(); j++ )
    //        {
    //              BranchNode& branch = *sections[i]->branchNodes[j];
    //            Vector3f pos = branch.startPosition;
    //            Vector3f posEnd = branch.endPosition;


    //            //  DrawPoint(pos);
    //            SetColour( YELLOW );

    //            if( j == (int)sections[i]->branchNodes.size() -1)
    //                SetColour( RED );
    //            DrawLine( pos, posEnd);


    //        }
    //    }

}


