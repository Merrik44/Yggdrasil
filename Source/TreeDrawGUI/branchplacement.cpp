#include "branchplacement.h"

BranchPlacement::BranchPlacement()
{
}

QPointF BranchPlacement::drawClosePoint(Branch *tr, QPointF point1, QPointF point2)
{
    Branch* tempTree = tr;
    tree = tempTree;
    smallDist = INT_MAX;
    calculateClosestBranch(point1, tr, true);
    calculateClosestBranch(point2, tr, false);
    float posOnParent = findPosOnParent();
    QLineF tempLine;
    if (isPointOne)
        tempLine = QLineF(closestPoint, point1);
    else
        tempLine = QLineF(closestPoint, point2);
    QLineF finalLine = moveBranchToEdgeOfRect(closestPoint, tempLine, posOnParent);
    return finalLine.p1();
}

void BranchPlacement::calculateClosestBranch(QPointF nb, Branch *tr, bool isPoint1)
{
    QPointF* maxRect = tr->getMaxRect();
    QPointF closePoint1 = findPoint(maxRect[0], maxRect[3], nb);
    QPointF closePoint2 = findPoint(maxRect[1], maxRect[2], nb);
    QPointF closePoint3 = findPoint(maxRect[2], maxRect[3], nb);
    float dist1 = findDistance(closePoint1, nb);
    float dist2 = findDistance(closePoint2, nb);
    float dist3 = findDistance(closePoint3, nb);

    QPointF closePoint = findPoint(tr->getStartPos(), tr->getLengthMaxPoint(), nb);
    float distance = findDistance(closePoint, nb);
    if ((dist1 < smallDist) || (dist2 < smallDist) || (dist3 < smallDist) || (distance < smallDist))
    {
        if (dist1 < smallDist)
            smallDist = dist1;
        if (dist2 < smallDist)
            smallDist = dist2;
        if (dist3 < smallDist)
            smallDist = dist3;
        if (distance < smallDist)
            smallDist = distance;

        closestPoint = closePoint;
        closestBranch = tr;
        isPointOne = isPoint1;
    }
    for (int i = 0 ; i < tr->numChildren() ; i++)
        calculateClosestBranch(nb, tr->childAt(i), isPoint1);
}

QPointF BranchPlacement::findPoint(QPointF a, QPointF b, QPointF c)
{
    QPointF ac = c-a;
    QPointF ab = b-a;
    float ab2 = (ab.x() * ab.x()) + (ab.y() * ab.y());
    float ac_ab = (ac.x() * ab.x()) + (ac.y() * ab.y());
    float t = ac_ab / ab2;
    if (t < 0.0f)
        t = 0.0f;
    else if (t > 1.0f)
        t = 1.0f;
    QPointF closest = a + (ab * t);
    return closest;
}

float BranchPlacement::findPosOnParent()
{
    float fullDistance = findDistance(closestBranch->getStartPos(), closestBranch->getLengthMaxPoint());
    float segDistance = findDistance(closestBranch->getStartPos(), closestPoint);
    float posOnParent = segDistance / fullDistance;
    return posOnParent;
}

float BranchPlacement::findDistance(QPointF a, QPointF b)
{
    int x = (a.x() - b.x()) * (a.x() - b.x());
    int y = (a.y() - b.y()) * (a.y() - b.y());
    float dist = sqrt(x + y);
    return dist;
}

QLineF BranchPlacement::moveBranchToEdgeOfRect(QPointF currPoint, QLineF newBranch, float posOnParent)
{
    QLineF tempLine = QLineF(closestBranch->getBranchLine().p1(), closestBranch->getLengthMaxPoint());
    tempLine.setLength(tempLine.length() * posOnParent);

    currPoint = tempLine.p2();

    QPointF differenceVec = newBranch.p1() - currPoint;
    newBranch.setP1(currPoint);
    newBranch.setP2(newBranch.p2() - differenceVec);

    QLineF tempNewBranch = newBranch;
    tempNewBranch.setLength(1000);

    QPointF* maxRect = closestBranch->getMaxRect();
    QLineF sideLine1 = QLineF(maxRect[0], maxRect[3]);
    QLineF sideLine1Temp = sideLine1;
    sideLine1Temp.setLength(1000);
    sideLine1Temp = QLineF(sideLine1Temp.p2(), sideLine1Temp.p1());
    sideLine1Temp.setLength(1000);
    QLineF sideLine2 = QLineF(maxRect[1], maxRect[2]);
    QLineF topLine = QLineF(maxRect[2], maxRect[3]);

    QLineF finalLine;

    if (posOnParent < 0.95)
    {
        bool isSide1 = doLineSegmentsIntersect(sideLine1Temp, tempNewBranch);

        // transpose to relative position on intersected rectangle line
        float outerRectLength;
        if (isSide1)
            outerRectLength = sideLine1.length();
        else
            outerRectLength = sideLine2.length();
        outerRectLength *= posOnParent;
        QPointF newStartPos;
        if (isSide1)
        {
            sideLine1.setLength(outerRectLength);
            newStartPos = sideLine1.p2();
        }
        else
        {
            sideLine2.setLength(outerRectLength);
            newStartPos = sideLine2.p2();
        }
        // find vector from point to newPoint
        QPointF changeVector = currPoint - newStartPos;
        // create newLine which moves newBranch by newPointVector
        finalLine.setP1(newStartPos);
        finalLine.setP2(newBranch.p2() - changeVector);
    }
    else
    {
        QPointF midPoint;
        midPoint.setX((topLine.p1().x() + topLine.p2().x())/2);
        midPoint.setY((topLine.p1().y() + topLine.p2().y())/2);

        QPointF diffVec = currPoint - midPoint;
        finalLine.setP1(midPoint);
        finalLine.setP2(newBranch.p2() - diffVec);
    }

    return finalLine;
}

bool BranchPlacement::isOnSegment(double xi, double yi, double xj, double yj, double xk, double yk)
{
    return (xi <= xk || xj <= xk) && (xk <= xi || xk <= xj) &&
            (yi <= yk || yj <= yk) && (yk <= yi || yk <= yj);
}

char BranchPlacement::computeDirection(double xi, double yi, double xj, double yj, double xk, double yk)
{
    double a = (xk - xi) * (yj - yi);
    double b = (xj - xi) * (yk - yi);
    return a < b ? -1 : a > b ? 1 : 0;
}

bool BranchPlacement::doLineSegmentsIntersect(QLineF line1, QLineF line2)
{
    char d1 = computeDirection(line2.p1().x(), line2.p1().y(), line2.p2().x(), line2.p2().y(), line1.p1().x(), line1.p1().y());
    char d2 = computeDirection(line2.p1().x(), line2.p1().y(), line2.p2().x(), line2.p2().y(), line1.p2().x(), line1.p2().y());
    char d3 = computeDirection(line1.p1().x(), line1.p1().y(), line1.p2().x(), line1.p2().y(), line2.p1().x(), line2.p1().y());
    char d4 = computeDirection(line1.p1().x(), line1.p1().y(), line1.p2().x(), line1.p2().y(), line2.p2().x(), line2.p2().y());
    return (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
            ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) ||
            (d1 == 0 && isOnSegment(line2.p1().x(), line2.p1().y(), line2.p2().x(), line2.p2().y(), line1.p1().x(), line1.p1().y())) ||
            (d2 == 0 && isOnSegment(line2.p1().x(), line2.p1().y(), line2.p2().x(), line2.p2().y(), line1.p2().x(), line1.p2().y())) ||
            (d3 == 0 && isOnSegment(line1.p1().x(), line1.p1().y(), line1.p2().x(), line1.p2().y(), line2.p1().x(), line2.p1().y())) ||
            (d4 == 0 && isOnSegment(line1.p1().x(), line1.p1().y(), line1.p2().x(), line1.p2().y(), line2.p2().x(), line2.p2().y()));
}

