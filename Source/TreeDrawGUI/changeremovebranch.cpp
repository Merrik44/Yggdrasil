#include "changeremovebranch.h"

ChangeRemoveBranch::ChangeRemoveBranch(Branch* tr)
{
    isTrunk = false;
    setTree(tr);
    findSelectedBranches(tr);
}

void ChangeRemoveBranch::findSelectedBranches(Branch* tr)
{
    bool completed = false;

    if (tr->getIsSelected())
    {
        if (tr->getIsTrunk())
        {
            completed = true;
            deleteBranchIdentifiers.append(tr->getIdentifier());
        }
        else if (!areAncestorsSelected(tr->getParent()))
            deleteBranchIdentifiers.append(tr->getIdentifier());
    }

    if (!completed)
        for (int i = 0 ; i < tr->numChildren() ; i++)
            findSelectedBranches(tr->childAt(i));
}

void ChangeRemoveBranch::apply()
{
    // deselect all branches
    deselectBranches(tree);

    for (int i = 0 ; i < deleteBranchIdentifiers.size() ; i++)
        deleteBranch(tree, deleteBranchIdentifiers.at(i));
}

void ChangeRemoveBranch::reverse()
{
    for (int i = 0 ; i < deleteBranchIdentifiers.size() ; i++)
    {
        if (!deletedBranches.at(i)->getIsTrunk())
        {
            long parentIdentifier = deletedBranches.at(i)->getParent()->getIdentifier();
            readdBranch(tree, parentIdentifier, i);
        }
        else
        {
            Branch* tempTree = deletedBranches.at(i);
            tree = tempTree;
        }
    }
}

void ChangeRemoveBranch::findSelectedBranch(Branch* tr)
{
    if (tr->getIsSelected())
    {
        tr->setIsSelected(false);
        deleteBranchIdentifier = tr->getIdentifier();
    }
    for (int i = 0 ; i < tr->numChildren() ; i++)
        findSelectedBranch(tr->childAt(i));
}

void ChangeRemoveBranch::deleteBranch(Branch* tr, long identifier)
{
    bool completed = false;

    if (tr->getIdentifier() == identifier && tr->getIsTrunk())
    {
        Branch* tempTree = tr;
        deletedBranch = tempTree;
        deletedBranches.append(tempTree);
        tr = new Branch();
        isTrunk = true;
        completed = true;
    }

    if (!completed)
        for (int i = 0 ; i < tr->numChildren() ; i++)
            if (tr->childAt(i)->getIdentifier() == identifier)
            {
                Branch* tempTree = tr->childAt(i);
                deletedBranch = tempTree;
                deletedBranches.append(tempTree);
                tr->removeChildAt(i);
                completed = true;
            }

    if (!completed)
        for (int i = 0 ; i < tr->numChildren() ; i++)
            deleteBranch(tr->childAt(i), identifier);
}

void ChangeRemoveBranch::readdBranch(Branch* tr, long identifier, int i)
{
    if (tr->getIdentifier() == identifier)
    {
        tr->addChild(deletedBranches.at(i));
    }
    else
        for (int j = 0 ; j < tr->numChildren() ; j++)
            readdBranch(tr->childAt(j), identifier, i);
}

Branch* ChangeRemoveBranch::getTree()
{
    Branch* treeTemp = tree;
    return treeTemp;
}

void ChangeRemoveBranch::setTree(Branch* tr)
{
    Branch* tempTree = tr;
    tree = tempTree;
}

bool ChangeRemoveBranch::getIsTrunk()
{
    return isTrunk;
}

QString ChangeRemoveBranch::getType()
{
    return "remove";
}

bool ChangeRemoveBranch::areAncestorsSelected(Branch* tr)
{
    bool isSel = tr->getIsSelected();
    bool isAncSel = false;
    if (!tr->getIsTrunk())
        isAncSel = areAncestorsSelected(tr->getParent());

    if (isSel || isAncSel)
        return true;
    else
        return false;
}

void ChangeRemoveBranch::deselectBranches(Branch* tr)
{
    if (tr->getIsSelected())
        tr->setIsSelected(false);
    for (int i = 0 ; i < tr->numChildren() ; i++)
        deselectBranches(tr->childAt(i));
}
