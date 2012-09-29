#ifndef SELECTBOX_H
#define SELECTBOX_H
#include "branch.h"
#include "segmentedbranch.h"

class SelectBox
{
public:
    SelectBox();
    Branch* useSelectBox(Branch *tr, QRectF selectRect, QVector<SegmentedBranch> segBranches);
private:
    void checkBranchSegments(QVector<long> branchIdentifiers, QRectF selectRect);
    void checkBranch(Branch* tree, QRectF selectRect);
    QPointF findMidpoint(QPointF p1, QPointF p2);
    void findBranch(Branch* tr, long iden);
    Branch* tree;
    Branch* currBranch;
};

#endif // SELECTBOX_H
