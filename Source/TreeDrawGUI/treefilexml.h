#ifndef TREEFILEXML_H
#define TREEFILEXML_H

#include <QXmlStreamWriter>
#include <QFile>
#include "branch.h"

class TreeFileXML
{
public:
    TreeFileXML();
    void createFile(Branch* tree, QString path);

private:
    Branch* tree;
    QString floatToString(float fl);
    void createXMLBranch(QXmlStreamWriter* xmlWriter, Branch* tree, bool parentRot);
    float calculateAngleToParent(Branch* tree, bool min);
    float calculateInclination(float angleToParent);
    float calculateAzimuth(float angleToParent, bool parentRot);
};

#endif // TREEFILEXML_H
