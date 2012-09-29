#ifndef CHANGEADDSEGMENTEDBRANCH_H
#define CHANGEADDSEGMENTEDBRANCH_H
#include "change.h"
#include "newbranch.h"

class ChangeAddSegmentedBranch : public Change
{
public:
    ChangeAddSegmentedBranch(QVector<QLineF> lineSegs, Branch* tree, bool treeEmpty, long identifierIncrementer, float defaultThickness);
    void apply();
    void reverse();
    Branch* getTree();
    void setTree(Branch* tr);
    QString getType();
    bool getIsTrunk();

private:
    void findBranchToDelete(Branch* tree);
    void closestPoint(QPointF nb, Branch* tr, bool isEnd);
    QPointF findPoint(QPointF a, QPointF b, QPointF c);
    float findDistance(QPointF a, QPointF b);
    QVector<QLineF> invertAllPoints(QVector<QLineF> lineSegs);
    QVector<QLineF> lineSegs;
    Branch* tree;
    bool treeEmpty;
    long branchIdentifier;
    NewBranch nb;
    bool isTrunk;    
    float smallDist;
    bool endCloser;
    float defaultThickness;
};

#endif // CHANGEADDSEGMENTEDBRANCH_H
