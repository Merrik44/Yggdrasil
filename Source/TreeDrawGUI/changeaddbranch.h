#ifndef CHANGEADDBRANCH_H
#define CHANGEADDBRANCH_H

#include "change.h"
#include "newbranch.h"
#include <QWidget>

class ChangeAddBranch : public Change
{
public:
    ChangeAddBranch(QPointF startPos, QPointF endPos, Branch* tree, bool treeEmpty, int id, float defaultThickness);
    void apply();
    void reverse();
    Branch* getTree();
    void setTree(Branch* tr);
    QString getType();
    bool getIsTrunk();
    bool getIsSelected();

private:
    void checkBranchSelected(Branch* tr, long id);
    void closestPoint(QPointF nb, Branch* tr, bool isEnd);
    QPointF findPoint(QPointF a, QPointF b, QPointF c);
    float findDistance(QPointF a, QPointF b);
    QPointF startPos;
    QPointF endPos;
    Branch* tree;
    bool treeEmpty;
    NewBranch nb;
    long branchIdentifier;
    void findBranchToDelete(Branch* tree);
    bool isTrunk;
    bool endCloser;
    float smallDist;
    bool isBranchSelected;
    float defaultThickness;
};

#endif // CHANGEADDBRANCH_H
