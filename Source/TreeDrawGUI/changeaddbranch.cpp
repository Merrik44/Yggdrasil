#include "changeaddbranch.h"

ChangeAddBranch::ChangeAddBranch(QPointF sp, QPointF ep, Branch* tr, bool te, int id, float dt)
{
    startPos = sp;
    endPos = ep;
    tree = tr;
    treeEmpty = te;
    if (treeEmpty && startPos.y() < endPos.y())
    {
        QPointF tempPoint = startPos;
        startPos = endPos;
        endPos = tempPoint;
    }
    nb = NewBranch();
    branchIdentifier = id;
    defaultThickness = dt;
    isTrunk = false;
    smallDist = INT_MAX;
    endCloser = false;
    if (!treeEmpty)
    {
        Branch* tempTree = tree;
        closestPoint(startPos, tempTree, false);
        closestPoint(endPos, tempTree, true);
    }
    if (endCloser)
    {
        QPointF temp = startPos;
        startPos = endPos;
        endPos = temp;
    }
}

void ChangeAddBranch::apply()
{
    Branch* childBranch = new Branch(startPos, endPos);
    childBranch->setPosOnParent(0);
    childBranch->setIsTrunk(true);
    childBranch->setIsSelected(false);
    childBranch->setIdentifier(branchIdentifier);
    childBranch->setIsSegment(false);
    childBranch->setIsRootSegment(false);

    if (treeEmpty)
    {
        tree = childBranch;
        tree->setDefaultThickness(defaultThickness);
        tree->updateLengthRect();
    }
    else
        nb.FindParentBranch(childBranch, tree, false, false, defaultThickness);
}

void ChangeAddBranch::reverse()
{
    findBranchToDelete(tree);
}

void ChangeAddBranch::findBranchToDelete(Branch* tr)
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

void ChangeAddBranch::setTree(Branch* tr)
{
    Branch* tempTree = tr;
    tree = tempTree;
}

Branch* ChangeAddBranch::getTree()
{
    Branch* tempTree = tree;
    return tempTree;
}

QString ChangeAddBranch::getType()
{
    return "add";
}

bool ChangeAddBranch::getIsTrunk()
{
    return isTrunk;
}

bool ChangeAddBranch::getIsSelected()
{
    isBranchSelected = false;
    checkBranchSelected(tree, branchIdentifier);
    return isBranchSelected;
}

void ChangeAddBranch::checkBranchSelected(Branch* tr, long id)
{
   if (tr->getIdentifier() == id)
       isBranchSelected = tr->getIsSelected();
   else
   {
       for (int i = 0 ; i < tr->numChildren() ; i++)
           checkBranchSelected(tr->childAt(i), id);
   }
}

void ChangeAddBranch::closestPoint(QPointF nb, Branch* tr, bool isEnd)
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

QPointF ChangeAddBranch::findPoint(QPointF a, QPointF b, QPointF c)
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

float ChangeAddBranch::findDistance(QPointF a, QPointF b)
{
    int x = (a.x() - b.x()) * (a.x() - b.x());
    int y = (a.y() - b.y()) * (a.y() - b.y());
    float dist = sqrt(x + y);
    return dist;
}
