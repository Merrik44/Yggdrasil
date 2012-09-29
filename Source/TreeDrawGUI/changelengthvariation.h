#ifndef CHANGELENGTHVARIATION_H
#define CHANGELENGTHVARIATION_H

#include "change.h"

class ChangeLengthVariation : public Change
{
public:
    ChangeLengthVariation();
    ChangeLengthVariation(float origLength, float finalLength, QVector<long> branchesBeingModifiedIden, QVector<float> origVariationLengths, QVector<float> finalVariationLengths, QString pointType, long identifier, Branch* tree);
    void apply();
    void reverse();
    void setTree(Branch* tree);
    Branch* getTree();
    QString getType();
private:
    void setLengthVariation(Branch* tree, QString pointType, long identifier, float length);
    float origLength;
    float finalLength;
    QString pointType;
    long branchIdentifier;
    Branch* tree;
    QVector<long> branchIdentifiers;
    QVector<float> origLengths;
    QVector<float> finalLengths;

};

#endif // CHANGELENGTHVARIATION_H
