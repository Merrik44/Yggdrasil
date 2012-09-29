#ifndef CHANGEREMOVEBRANCH_H
#define CHANGEREMOVEBRANCH_H

#include "change.h"

class ChangeRemoveBranch : public Change
{
public:
    ChangeRemoveBranch(Branch* tr);
    void apply();
    void reverse();
    Branch* getTree();
    void setTree(Branch* tr);
    bool getIsTrunk();
    QString getType();

private:
    bool areAncestorsSelected(Branch* tr);
    void deselectBranches(Branch* tree);
    Branch* tree;
    Branch* deletedBranch;
    QVector<Branch*> deletedBranches;
    void findSelectedBranches(Branch* tr);
    void findSelectedBranch(Branch* tr);
    void deleteBranch(Branch* tr, long identifier);
    QVector<long> deleteBranchIdentifiers;
    long deleteBranchIdentifier;
    void readdBranch(Branch* tr, long identifier, int index);
    bool isTrunk;
};

#endif // CHANGEREMOVEBRANCH_H
