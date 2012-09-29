#ifndef BRANCHPLACEMENT_H
#define BRANCHPLACEMENT_H
#include "branch.h"

class BranchPlacement
{
public:
    BranchPlacement();
    QPointF drawClosePoint(Branch* tr, QPointF point1, QPointF point2);

private:
    void calculateClosestBranch(QPointF nb, Branch* tr, bool isPointOne);
    QPointF findPoint(QPointF a, QPointF b, QPointF c);
    float findDistance(QPointF a, QPointF b);
    float findPosOnParent();
    QLineF moveBranchToEdgeOfRect(QPointF currPoint, QLineF newBranch, float posOnParent);
    bool isOnSegment(double xi, double yi, double xj, double yj, double xk, double yk);
    char computeDirection(double xi, double yi, double xj, double yj, double xk, double yk);
    bool doLineSegmentsIntersect(QLineF line1, QLineF line2);
    Branch* tree;
    float smallDist;
    QPointF closestPoint;
    Branch* closestBranch;
    bool isPointOne;
};

#endif // BRANCHPLACEMENT_H
