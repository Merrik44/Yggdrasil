#include "selectbox.h"

SelectBox::SelectBox()
{
}

Branch* SelectBox::useSelectBox(Branch *tr, QRectF selectRect, QVector<SegmentedBranch> segBranches)
{
    Branch* tempTree = tr;
    tree = tempTree;
    for (int i = 0 ; i < segBranches.size() ; i++)
        checkBranchSegments(segBranches.value(i).getAllIdentifiers(), selectRect);
    checkBranch(tree, selectRect);

    return tree;
}

void SelectBox::checkBranchSegments(QVector<long> branchIdentifiers, QRectF selectRect)
{
    int counter = 0;
    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
    {
        findBranch(this->tree, branchIdentifiers.at(i));
        QPointF p1 = currBranch->getBranchLine().p1();
        QPointF p2 = currBranch->getLengthMaxPoint();
        QPointF p3 = findMidpoint(p1, p2);

        if ((selectRect.contains(p1) && selectRect.contains(p3)) || (selectRect.contains(p2) && selectRect.contains(p3)))
            counter++;
    }

    if (counter >= (branchIdentifiers.size() / 2.0))
    {
        for (int i = 0 ; i < branchIdentifiers.size() ; i++)
        {
            findBranch(this->tree, branchIdentifiers.at(i));
            currBranch->setIsSelected(true);
        }
    }
}

void SelectBox::checkBranch(Branch *tr, QRectF selectRect)
{
    if (!tr->getIsSegment())
    {
        QPointF p1 = tr->getBranchLine().p1();
        QPointF p2 = tr->getLengthMaxPoint();
        QPointF p3 = findMidpoint(p1, p2);

        if ((selectRect.contains(p1) && selectRect.contains(p3)) || (selectRect.contains(p2) && selectRect.contains(p3)))
            tr->setIsSelected(true);        
    }

    for (int i = 0 ; i < tr->numChildren() ; i++)
        checkBranch(tr->childAt(i), selectRect);
}

QPointF SelectBox::findMidpoint(QPointF p1, QPointF p2)
{
    QPointF p3 = QPointF();
    p3.setX((p1.x() + p2.x())/2);
    p3.setY((p1.y() + p2.y())/2);
    return p3;
}

void SelectBox::findBranch(Branch* tr, long iden)
{
    if (tr->getIdentifier() == iden)
        currBranch = tr;
    else
        for (int i = 0 ; i < tr->numChildren() ; i++)
            findBranch(tr->childAt(i), iden);
}
