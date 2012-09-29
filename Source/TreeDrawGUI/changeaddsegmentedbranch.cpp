#include "changeaddsegmentedbranch.h"

ChangeAddSegmentedBranch::ChangeAddSegmentedBranch(QVector<QLineF> ls, Branch *tr, bool te, long id, float dt)
{
    lineSegs = ls;
    tree = tr;
    treeEmpty = te;
    branchIdentifier = id;
    defaultThickness = dt;
    nb = NewBranch();
    isTrunk = false;
    smallDist = INT_MAX;
    endCloser = false;
    if (treeEmpty && lineSegs.at(0).p1().y() < lineSegs.last().p2().y())
    {
        lineSegs = invertAllPoints(lineSegs);
    }
    if (!treeEmpty)
    {
        Branch* tempTree = tree;
        closestPoint(lineSegs.at(0).p1(), tempTree, false);
        closestPoint(lineSegs.last().p2(), tempTree, true);
    }
    if (endCloser)
        lineSegs = invertAllPoints(lineSegs);
}

void ChangeAddSegmentedBranch::apply()
{
    long tempIdentifier = branchIdentifier;
    bool tempTreeEmpty = treeEmpty;
    for (int i = 0 ; i < lineSegs.size() ; i++)
    {
        Branch* childBranch = new Branch(lineSegs.at(i).p1(), lineSegs.at(i).p2());
        childBranch->setPosOnParent(0);
        childBranch->setIsTrunk(true);
        childBranch->setIsSelected(false);
        childBranch->setIdentifier(tempIdentifier);

        if (tempTreeEmpty)
        {
            childBranch->setIsSegment(true);
            childBranch->setIsRootSegment(true);
            childBranch->setDefaultThickness(defaultThickness);
            childBranch->updateLengthRect();
            tree = childBranch;
            tempTreeEmpty = false;
        }
        else
        {
            if (i == 0)
                nb.FindParentBranch(childBranch, tree, true, true, defaultThickness);
            else
                nb.FindParentBranch(childBranch, tree, true, false, defaultThickness);
        }
        tempIdentifier++;
    }
}

void ChangeAddSegmentedBranch::reverse()
{
    findBranchToDelete(tree);
}

void ChangeAddSegmentedBranch::findBranchToDelete(Branch *tr)
{
    bool completed = false;

    if (tr->getIdentifier() == branchIdentifier && tr->getIsTrunk())
    {
        isTrunk = true;
        tr = new Branch();
        completed = true;
    }

    for (int i = 0 ; i < tr->numChildren() ; i++)
    {
        if (tr->childAt(i)->getIdentifier() == branchIdentifier)
        {
            tr->removeChildAt(i);
            completed = true;
        }
    }

    if (!completed)
        for (int i = 0 ; i < tr->numChildren() ; i++)
            findBranchToDelete(tr->childAt(i));
}

void ChangeAddSegmentedBranch::setTree(Branch *tr)
{
    Branch* tempTree = tr;
    tree = tempTree;
}

Branch* ChangeAddSegmentedBranch::getTree()
{
    Branch* tempTree = tree;
    return tempTree;
}

QString ChangeAddSegmentedBranch::getType()
{
    return "addseg";
}

bool ChangeAddSegmentedBranch::getIsTrunk()
{
    return isTrunk;
}

void ChangeAddSegmentedBranch::closestPoint(QPointF nb, Branch* tr, bool isEnd)
{
    QPointF closePoint = findPoint(tr->getStartPos(), tr->getLengthMaxPoint(), nb);
    float distance = findDistance(closePoint, nb);
    if (distance < smallDist)
    {
        smallDist = distance;
        endCloser = isEnd;
    }
    for (int i = 0 ; i < tr->numChildren() ; i++)
    {
        closestPoint(nb, tr->childAt(i), isEnd);
    }
}

QPointF ChangeAddSegmentedBranch::findPoint(QPointF a, QPointF b, QPointF c)
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

float ChangeAddSegmentedBranch::findDistance(QPointF a, QPointF b)
{
    int x = (a.x() - b.x()) * (a.x() - b.x());
    int y = (a.y() - b.y()) * (a.y() - b.y());
    float dist = sqrt(x + y);
    return dist;
}

QVector<QLineF> ChangeAddSegmentedBranch::invertAllPoints(QVector<QLineF> ls)
{
    QVector<QLineF> newLineSegs;
    while(!ls.empty())
    {
        QLineF tempLine = ls.last();
        ls.pop_back();
        QPointF tempPoint = tempLine.p1();
        tempLine.setP1(tempLine.p2());
        tempLine.setP2(tempPoint);
        newLineSegs.append(tempLine);
    }
    return newLineSegs;
}
