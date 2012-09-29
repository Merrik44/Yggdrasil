#include "changewidthvariation.h"

ChangeWidthVariation::ChangeWidthVariation(WidthChanges cbo, WidthChanges cbf, QVector<long> branchesBeingModifiedIden, QVector<WidthChanges> obo, QVector<WidthChanges> obf, long id, Branch* tr)
{
    currBranchOrig = cbo;
    currBranchFinal = cbf;
    branchIdentifiers = branchesBeingModifiedIden;
    otherBranchesOrig = obo;
    otherBranchesFinal = obf;
    branchIdentifier = id;
    setTree(tr);
}

void ChangeWidthVariation::apply()
{
    Branch* tempTree = tree;
    setWidths(tempTree, branchIdentifier, currBranchFinal);
    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
    {
        setWidths(tempTree, branchIdentifiers.value(i), otherBranchesFinal.value(i));
    }
}

void ChangeWidthVariation::reverse()
{
    Branch* tempTree = tree;
    //setBranchLength(tempTree, branchIdentifier, varType, origLength);
    setWidths(tempTree, branchIdentifier, currBranchOrig);
    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
        setWidths(tempTree, branchIdentifiers.value(i), otherBranchesOrig.value(i));
    //setBranchWidths(tempTree, branchIdentifiers.at(i), varType, origLengths.at(i));
}

Branch* ChangeWidthVariation::getTree()
{
    Branch* tempTree = tree;
    return tempTree;
}

void ChangeWidthVariation::setTree(Branch* tr)
{
    Branch* tempTree = tr;
    tree = tempTree;
}

void ChangeWidthVariation::setWidths(Branch* tr, long id, WidthChanges wc)
{
    if (tr->getIdentifier() == id)
    {
        if (tr->getIsSegment())
        {
            tr->setCurveWidthVariation(wc.getVariation("smin"), "smin");
            tr->setCurveWidthVariation(wc.getVariation("smax"), "smax");
            tr->setCurveWidthVariation(wc.getVariation("emin"), "emin");
            tr->setCurveWidthVariation(wc.getVariation("emax"), "emax");
        }
        else
        {
            tr->setWidthVariation(wc.getVariation("smin"), "smin");
            tr->setWidthVariation(wc.getVariation("smax"), "smax");
            tr->setWidthVariation(wc.getVariation("emin"), "emin");
            tr->setWidthVariation(wc.getVariation("emax"), "emax");
        }
    }
    else
        for (int i = 0 ; i < tr->numChildren() ; i++)
            setWidths(tr->childAt(i), id, wc);
}

void ChangeWidthVariation::setBranchWidths(Branch* tr, long id, QString vt, float len)
{
    if (tr->getIdentifier() == id)
        tr->setWidthVariation(len, vt);
    else
        for (int i = 0 ; i < tr->numChildren() ; i++)
            setBranchWidths(tr->childAt(i), id, vt, len);
}

QString ChangeWidthVariation::getType()
{
    return "width";
}
