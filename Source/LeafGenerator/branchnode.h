// Author: Richard Pieterse
// Date: October 2012

#ifndef BRANCH_H
#define BRANCH_H

#include "vmath.h"
#include "vector"
#include <QtCore>
//#include "mesh.h"

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
    }
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
