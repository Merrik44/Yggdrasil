#include "treefilexml.h"

TreeFileXML::TreeFileXML()
{
}

void TreeFileXML::createFile(Branch* tree, QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {

    }
    else
    {
        QXmlStreamWriter* xmlWriter = new QXmlStreamWriter();

        xmlWriter->setDevice(&file);
        xmlWriter->setAutoFormatting(true);

        xmlWriter->writeStartDocument();
        // Recursive call to write branches to XML file
        createXMLBranch(xmlWriter, tree, true);

        xmlWriter->writeEndDocument();
        delete xmlWriter;
    }
}

void TreeFileXML::createXMLBranch(QXmlStreamWriter* xmlWriter, Branch* tree, bool parentRot)
{
    xmlWriter->writeStartElement("branch");

    // Write curve parameters to XML file
    xmlWriter->writeEmptyElement("curve");

    // Write position on parent to XML file
    xmlWriter->writeStartElement("positionOnParent");
    xmlWriter->writeCharacters(floatToString(tree->getPosOnParent()));
    xmlWriter->writeEndElement();

    // Write whether part of curved branch
    xmlWriter->writeStartElement("isSubSegment");
    if (tree->getIsSegment() && !tree->getIsRootSegment())
        xmlWriter->writeCharacters("true");
    else
        xmlWriter->writeCharacters("false");
    xmlWriter->writeEndElement();

    // Write angle to parent including max and min for inclination and azimuth
    float angleToParentMin = calculateAngleToParent(tree, true);
    float angleToParentMax = calculateAngleToParent(tree, false);

    if (angleToParentMin < angleToParentMax)
    {
        float tempAngle = angleToParentMin;
        angleToParentMin = angleToParentMax;
        angleToParentMax = tempAngle;
    }

    float inclinationMin = calculateInclination(angleToParentMin);
    float azimuthMin = calculateAzimuth(angleToParentMin, parentRot);


    float inclinationMax = calculateInclination(angleToParentMax);
    float azimuthMax = calculateAzimuth(angleToParentMax, parentRot);


    if (parentRot && angleToParentMin <= M_PI)
        parentRot = false;
    if (!parentRot && angleToParentMin > M_PI)
        parentRot = true;

    xmlWriter->writeStartElement("angleToParent");
    xmlWriter->writeStartElement("inclination");
    xmlWriter->writeStartElement("min");
    xmlWriter->writeCharacters(floatToString(inclinationMin));
    xmlWriter->writeEndElement();
    xmlWriter->writeStartElement("max");
    xmlWriter->writeCharacters(floatToString(inclinationMax));
    xmlWriter->writeEndElement();
    xmlWriter->writeEndElement();
    xmlWriter->writeStartElement("azimuth");
    xmlWriter->writeStartElement("min");
    xmlWriter->writeCharacters(floatToString(azimuthMin));
    xmlWriter->writeEndElement();
    xmlWriter->writeStartElement("max");
    xmlWriter->writeCharacters(floatToString(azimuthMax));
    xmlWriter->writeEndElement();
    xmlWriter->writeEndElement();
    xmlWriter->writeEndElement();

    // Write Branch Thickness including start and end min and max to XML file
    xmlWriter->writeStartElement("thickness");
    xmlWriter->writeStartElement("start");
    xmlWriter->writeStartElement("min");
    xmlWriter->writeCharacters(floatToString(tree->getThicknessVariationStartMin()));
    xmlWriter->writeEndElement();
    xmlWriter->writeStartElement("max");
    xmlWriter->writeCharacters(floatToString(tree->getThicknessVariationStartMax()));
    xmlWriter->writeEndElement();
    xmlWriter->writeEndElement();
    xmlWriter->writeStartElement("end");
    xmlWriter->writeStartElement("min");
    xmlWriter->writeCharacters(floatToString(tree->getThicknessVariationEndMin()));
    xmlWriter->writeEndElement();
    xmlWriter->writeStartElement("max");
    xmlWriter->writeCharacters(floatToString(tree->getThicknessVariationEndMax()));
    xmlWriter->writeEndElement();
    xmlWriter->writeEndElement();
    xmlWriter->writeEndElement();

    // Write Length including min and max to XML file
    xmlWriter->writeStartElement("length");
    xmlWriter->writeStartElement("min");
    xmlWriter->writeCharacters(floatToString(tree->getLengthVariationMin()));
    xmlWriter->writeEndElement();
    xmlWriter->writeStartElement("max");
    xmlWriter->writeCharacters(floatToString(tree->getLengthVariationMax()));
    xmlWriter->writeEndElement();
    xmlWriter->writeEndElement();

    // Write Children (recursive call) to XML file
    xmlWriter->writeStartElement("children");
    for (int i = 0 ; i < tree->numChildren() ; i++)
    {
        createXMLBranch(xmlWriter, tree->childAt(i), parentRot);
    }

    xmlWriter->writeEndElement();
    xmlWriter->writeEndElement();
}

float TreeFileXML::calculateAngleToParent(Branch* tree, bool min)
{
    float angleDegrees;
    if (!tree->getIsTrunk())
    {
        if (min)
            angleDegrees = tree->getAngleLineMin().angleTo(tree->getParent()->getBranchLine());
        else
            angleDegrees = tree->getAngleLineMax().angleTo(tree->getParent()->getBranchLine());
    }
    else
    {
        if (min)
            angleDegrees = 360 - tree->getAngleLineMin().angle();
        else
            angleDegrees = 360 - tree->getAngleLineMax().angle();
    }
    return (angleDegrees * M_PI / 180);
}

QString TreeFileXML::floatToString(float fl)
{
    QString str = QString::number(fl);
    return str;
}

float TreeFileXML::calculateInclination(float angleToParent)
{
    if (angleToParent > M_PI) return -angleToParent + 2 * M_PI;
    return -angleToParent;

    /*float inclination;
    if (angleToParent <= M_PI)
        inclination = angleToParent;
    else
        inclination = 2 * M_PI - angleToParent;
    return inclination;*/
}

float TreeFileXML::calculateAzimuth(float angleToParent, bool parentRot)
{
    return 0;

    /*float azimuth;
    if (parentRot)
    {
        if (angleToParent <= M_PI)
            azimuth = M_PI;
        else
            azimuth = 0;
    }
    else
    {
        if (angleToParent <= M_PI)
            azimuth = 0;
        else
            azimuth = M_PI;
    }
    return azimuth;*/
}
