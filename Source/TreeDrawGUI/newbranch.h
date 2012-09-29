#ifndef NEWBRANCH_H
#define NEWBRANCH_H

#include "branch.h"

class NewBranch
{
public:
    NewBranch();
    void FindParentBranch(Branch* newBranch, Branch* tree, bool segment, bool firstSegment, float defaultThickness);
    Branch* returnBranch();
    QPointF returnPoint();

private:
    void CalculateParent(QPointF point, Branch* tree);
    void calculateSegParent(float iden, QPointF nb, Branch* tr);
    void FindBranch();
    QPointF FindPoint(QPointF a, QPointF b, QPointF c);
    float FindDistance(QPointF a, QPointF b);
    void FindPosOnParent();
    void AddChildBranch();
    Branch* newBranch;
    Branch* tree;
    float smallDist;
    QPointF closestPoint;
    Branch* closestBranch;
};

#endif // NEWBRANCH_H
