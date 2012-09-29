#ifndef CHANGEWIDTHVARIATION_H
#define CHANGEWIDTHVARIATION_H

#include "change.h"
#include "widthchanges.h"

class ChangeWidthVariation : public Change
{
public:
    //ChangeWidthVariation(float origLength, float finalLength, QVector<long> branchesBeingModifiedIden, QVector<float> origVariationLengths, QVector<float> finalVariationLengths, QString pointType, long identifier, Branch* tree);
    ChangeWidthVariation(WidthChanges currBranchOrig, WidthChanges currBranchFinal, QVector<long> branchesBeingModifiedIden, QVector<WidthChanges> origWidthChanges, QVector<WidthChanges> finalWidthChanges, long id, Branch* tr);
    void apply();
    void reverse();
    Branch* getTree();
    void setTree(Branch* tr);
    void setBranchWidths(Branch* tr, long id, QString vt, float len);
    void setWidths(Branch* tr, long id, WidthChanges wc);
    void setFinalLength(float length);
    QString getType();

private:
    Branch* tree;
    long branchIdentifier;
    QVector<long> branchIdentifiers;
    WidthChanges currBranchOrig;
    WidthChanges currBranchFinal;
    QVector<WidthChanges> otherBranchesOrig;
    QVector<WidthChanges> otherBranchesFinal;
    /*float origLength;
    float finalLength;
    QVector<float> origLengths;
    QVector<float> finalLengths;
    QString varType;*/
};

#endif // CHANGEWIDTHVARIATION_H
