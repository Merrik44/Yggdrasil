#include "changeanglevariation.h"

ChangeAngleVariation::ChangeAngleVariation()
{
}

ChangeAngleVariation::ChangeAngleVariation(QPointF op, QPointF fp, QVector<long> branchBeingModifiedIdentifiers, QVector<QPointF> origVariationPoints, QVector<QPointF> finalVariationPoints, QString vt, long id, Branch* tr)
{
    origPoint = op;
    finalPoint = fp;
    branchIdentifiers = branchBeingModifiedIdentifiers;
    origPoints = origVariationPoints;
    finalPoints = finalVariationPoints;
    varType = vt;
    branchIdentifier = id;
    setTree(tr);
}

void ChangeAngleVariation::apply()
{
    setAngleVariation(tree, branchIdentifier, finalPoint);
    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
        setAngleVariation(tree, branchIdentifiers.at(i), finalPoints.at(i));
}

void ChangeAngleVariation::reverse()
{
    setAngleVariation(tree, branchIdentifier, origPoint);
    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
        setAngleVariation(tree, branchIdentifiers.at(i), origPoints.at(i));
}

void ChangeAngleVariation::setAngleVariation(Branch *tr, long identifier, QPointF point)
{
    if (tr->getIdentifier() == identifier)
        tr->setAngleVariation(point, varType);
    else
        for (int i = 0 ; i < tr->numChildren() ; i++)
            setAngleVariation(tr->childAt(i), identifier, point);
}

void ChangeAngleVariation::setTree(Branch *tr)
{
    Branch* tempTree = tr;
    tree = tempTree;
}

Branch* ChangeAngleVariation::getTree()
{
    Branch* tempTree = tree;
    return tempTree;
}

QString ChangeAngleVariation::getType()
{
    return "angle";
}
