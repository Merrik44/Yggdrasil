#include "newbranch.h"

NewBranch::NewBranch()
{
}

void NewBranch::FindParentBranch(Branch *nb, Branch *tr, bool seg, bool firstSeg, float defaultThickness)
{
    smallDist = INT_MAX;
    newBranch = nb;
    tree = tr;
    // CalculateParent for the start and end pos
    if (!seg || firstSeg)
    {
        CalculateParent(newBranch->getStartPos(), tree);
        QPointF diff = closestPoint - newBranch->getStartPos();
        newBranch->setPos(closestPoint, diff + newBranch->getEndPos());
        newBranch->setOrigPosOnParent(closestPoint);
        newBranch->setIsTrunk(false);
        newBranch->setIsSelected(true);
        if (firstSeg)
        {
            newBranch->setIsSegment(true);
            newBranch->setIsRootSegment(true);
        }
        else
            newBranch->setIsSegment(false);
        FindPosOnParent();
        if (newBranch->getPosOnParent() < 0.05 && closestBranch->getPosOnParent() > 0.95)
        {
            if (!closestBranch->getIsTrunk())
            {
                closestBranch = closestBranch->getParent();
                QPointF pointDiff = closestBranch->getLengthMaxPoint() - newBranch->getStartPos();
                newBranch->setPos(closestBranch->getLengthMaxPoint(), pointDiff + newBranch->getEndPos());
                newBranch->setOrigPosOnParent(newBranch->getStartPos());
                newBranch->setPosOnParent(1);
            }
        }
        else if ((newBranch->getPosOnParent() < 0.05) || (closestBranch->getIsRootSegment() && newBranch->getPosOnParent() < 0.25))
        {
            if (!closestBranch->getIsTrunk())
            {
                QPointF pointDiff = closestBranch->getOrigPosOnParent() - newBranch->getStartPos();
                newBranch->setPos(closestBranch->getOrigPosOnParent(), pointDiff + newBranch->getEndPos());
                newBranch->setOrigPosOnParent(newBranch->getStartPos());
                newBranch->setPosOnParent(closestBranch->getPosOnParent());
                closestBranch = closestBranch->getParent();
            }
        }
        AddChildBranch();
        newBranch->setBranchLine(newBranch->getParent()->moveBranchToEdgeOfRect(newBranch->getStartPos(), newBranch->getBranchLine(), newBranch->getPosOnParent()));
        newBranch->setDefaultThickness(defaultThickness);
        newBranch->updateLengthRect();
    }
    else
    {
        calculateSegParent(newBranch->getIdentifier() - 1, newBranch->getStartPos(), tree);
        QPointF diff = closestPoint - newBranch->getStartPos();
        newBranch->setPos(closestPoint, diff + newBranch->getEndPos());
        newBranch->setOrigPosOnParent(closestPoint);
        newBranch->setIsTrunk(false);
        newBranch->setIsSelected(true);
        newBranch->setIsSegment(true);
        newBranch->setIsRootSegment(false);
        newBranch->setPosOnParent(1.0f);
        AddChildBranch();
        newBranch->setBranchLine(newBranch->getParent()->moveBranchToEdgeOfRect(newBranch->getStartPos(), newBranch->getBranchLine(), newBranch->getPosOnParent()));
        newBranch->setDefaultThickness(defaultThickness);
        newBranch->updateLengthRect();
    }
}

void NewBranch::CalculateParent(QPointF nb, Branch* tr)
{
    QPointF* maxRect = tr->getMaxRect();
    QPointF closePoint1 = FindPoint(maxRect[0], maxRect[3], nb);
    QPointF closePoint2 = FindPoint(maxRect[1], maxRect[2], nb);
    QPointF closePoint3 = FindPoint(maxRect[2], maxRect[3], nb);
    float dist1 = FindDistance(closePoint1, nb);
    float dist2 = FindDistance(closePoint2, nb);
    float dist3 = FindDistance(closePoint3, nb);

    QPointF closePoint = FindPoint(tr->getStartPos(), tr->getLengthMaxPoint(), nb);
    float distance = FindDistance(closePoint, nb);
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
    }
    for (int i = 0 ; i < tr->numChildren() ; i++)
        CalculateParent(nb, tr->childAt(i));
}

void NewBranch::calculateSegParent(float iden, QPointF nb, Branch* tr)
{
    if (tr->getIdentifier() == iden)
    {
        closestPoint = tr->getLengthMaxPoint();
        smallDist = FindDistance(closestPoint, nb);
        closestBranch = tr;
    }
    else
    {
        for (int i = 0 ; i < tr->numChildren() ; i++)
            calculateSegParent(iden, nb, tr->childAt(i));
    }
}

QPointF NewBranch::FindPoint(QPointF a, QPointF b, QPointF c)
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

float NewBranch::FindDistance(QPointF a, QPointF b)
{
    int x = (a.x() - b.x()) * (a.x() - b.x());
    int y = (a.y() - b.y()) * (a.y() - b.y());
    float dist = sqrt(x + y);
    return dist;
}

void NewBranch::FindPosOnParent()
{
    float fullDistance = FindDistance(closestBranch->getStartPos(), closestBranch->getLengthMaxPoint());
    float segDistance = FindDistance(closestBranch->getStartPos(), closestPoint);
    float posOnParent = segDistance / fullDistance;
    newBranch->setPosOnParent(posOnParent);
}

void NewBranch::AddChildBranch()
{
    closestBranch->addChild(newBranch);
    newBranch->setParent(closestBranch);
    newBranch->setIsSelected(true);
    newBranch->setIsTrunk(false);

    // Check if branch has siblings, if it does, set min and max length variation to that of sibling
    /*if (closestBranch->numChildren() > 1)
    {
        newBranch->setLengthVariation(closestBranch->childAt(0)->getLengthVariation("lmin"), "lmin");
        newBranch->setLengthVariation(closestBranch->childAt(0)->getLengthVariation("lmax"), "lmax");
    }*/
}

Branch* NewBranch::returnBranch()
{
    return closestBranch;
}

QPointF NewBranch::returnPoint()
{
    return closestPoint;
}
