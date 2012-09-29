#ifndef CHANGEANGLEVARIATION_H
#define CHANGEANGLEVARIATION_H

#include "change.h"

class ChangeAngleVariation : public Change
{
public:
    ChangeAngleVariation();
    ChangeAngleVariation(QPointF origPoint, QPointF finalPoint, QVector<long> branchesBeingModifiedIdentifiers, QVector<QPointF> origVariationPoints, QVector<QPointF> finalVariationPoints, QString varType, long id, Branch* tr);
    void apply();
    void reverse();
    void setTree(Branch *tree);
    Branch* getTree();
    QString getType();

private:
    void setAngleVariation(Branch* tr, long identifier, QPointF point);
    QPointF origPoint;
    QPointF finalPoint;
    QString varType;
    long branchIdentifier;
    QVector<long> branchIdentifiers;
    QVector<QPointF> origPoints;
    QVector<QPointF> finalPoints;
    Branch* tree;
};

#endif // CHANGEANGLEVARIATION_H
