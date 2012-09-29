#include "changelengthvariation.h"

ChangeLengthVariation::ChangeLengthVariation()
{
}

ChangeLengthVariation::ChangeLengthVariation(float ol, float fl, QVector<long> branchesBeingModifiedIden, QVector<float> origVariationLengths,
                                             QVector<float> finalVariationLengths, QString pt, long id, Branch* tr)
{
    origLength = ol;
    finalLength = fl;
    branchIdentifiers = branchesBeingModifiedIden;
    origLengths = origVariationLengths;
    finalLengths = finalVariationLengths;
    pointType = pt;
    branchIdentifier = id;
    setTree(tr);
}

void ChangeLengthVariation::apply()
{
    Branch* tempTree = tree;
    setLengthVariation(tempTree, pointType, branchIdentifier, finalLength);
    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
        setLengthVariation(tempTree, pointType, branchIdentifiers.at(i), finalLengths.at(i));
}

void ChangeLengthVariation::reverse()
{
    Branch* tempTree = tree;
    setLengthVariation(tempTree, pointType, branchIdentifier, origLength);
    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
        setLengthVariation(tempTree, pointType, branchIdentifiers.at(i), origLengths.at(i));
}

void ChangeLengthVariation::setLengthVariation(Branch* tr, QString pType, long id, float len)
{
    if (tr->getIdentifier() == id)
        tr->setLengthVariation(len, pType);
    else
        for (int i = 0 ; i < tr->numChildren() ; i++)
            setLengthVariation(tr->childAt(i), pType, id, len);
}

Branch* ChangeLengthVariation::getTree()
{
    Branch* tempTree = tree;
    return tempTree;
}

void ChangeLengthVariation::setTree(Branch* tr)
{
    Branch* tempTree = tr;
    tree = tempTree;
}

QString ChangeLengthVariation::getType()
{
    return "length";
}
