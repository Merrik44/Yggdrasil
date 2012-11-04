// Author: Richard Pieterse
// Date: October 2012

#ifndef BRANCH_H
#define BRANCH_H

#include "vmath.h"
#include "vector"
#include "mesh.h"
#include <QString>
#include <QtCore>

class BranchNode
{
public:

    Vector3f startPosition;
    Vector3f endPosition;
    float startRadius;
    float endRadius;
    float length;
    std::vector<BranchNode*> children;
    BranchNode* parent;
    Matrix3f rotation;
    Vector3f direction;

    std::vector< Vertex*> startVertices;
    std::vector< Vertex*>  endVertices;
    std::vector< Vertex*> Vertices2ndFromStart;
    std::vector< Vertex*>  Vertices2ndFromEnd;


    std::vector< Face*> startFaces;
    std::vector< Face*>  endFaces;

    float startOffset;
    float endOffset;
    float tightStartOffset;
    float tightEndOffset;
    bool split;



    BranchNode( Vector3f StartPosition,  Matrix3f Rotation , float StartRadius, float EndRadius, float Length, BranchNode* Parent)
    {
        startPosition = StartPosition;
        startRadius = StartRadius;
        endRadius = EndRadius;
        length = Length;
        parent = Parent;
        rotation =Rotation;
        if( parent != NULL )
            parent->children.push_back(this);

        // calculate end pos
        direction = rotation*Vector3f( 0, 1, 0 );;
        endPosition = direction*length + startPosition;

        startOffset = endOffset = 0;

        // --- recalulate rotation relative to the up vector ---
        Vector3f axis = direction.crossProduct(Vector3f(0,1, 0));
        if( direction == Vector3f(0,1, 0) )
            axis = Vector3f(1, 0, 0);
        else if( direction == Vector3f(0,-1, 0) )
                axis = Vector3f(1, 0, 0);

        float dot  =direction.dotProduct(Vector3f(0,1, 0));
        float angle = acos(dot);
        angle *=  57.2957795;

        axis *= angle;
      //  Quatf quat;
        //quat.fromAxisRot(axis, angle );
       // rotation = quat.rotMatrix();


    }


//    BranchNode( Vector3f StartPosition, Vector3f EndPosition, float StartRadius, float EndRadius, BranchNode* Parent)
//    {
//        startPosition = StartPosition;
//        startRadius = StartRadius;
//        endRadius = EndRadius;
//        endPosition = EndPosition;
//        length = (startPosition - EndPosition).normalize();
//        parent = Parent;
//        rotation =Rotation;

//        if( parent != NULL )
//            parent->children.push_back(this);

//        // calculate end pos
//        direction = rotation*Vector3f( 0, 1, 0 );;
//        endPosition = direction*length + startPosition;

//        startOffset = endOffset = 0;
//    }
};

class BranchSection
{
public:
    std::vector< BranchNode*> branchNodes;
};

class BranchPoint
{
    public:
        Vector3f poistion;
        float radius;
};


class LSTLeafNode
{
    public:
    LSTLeafNode()
    {

    }
    float size;
    float scale;
    float attachment;
    BranchNode* attachmentPoint;

    QString name;
    QString path;
    QString tex;
    int number;
    Vector3f p1;
    Vector3f p2;
    Vector3f p3;
    Vector3f p4;

    Matrix3f rot;
};


#endif // BRANCH_H
