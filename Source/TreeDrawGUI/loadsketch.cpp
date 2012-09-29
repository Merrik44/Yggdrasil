#include "loadsketch.h"
#include <algorithm>

//qDebug() << newData.angleMax << newData.angleMin << newData.hasSubSegment << newData.isSubSegment << newData.lengthMax << newData.lengthMin << newData.posOnParent << newData.widthEndMax << newData.widthEndMin << newData.widthStartMax << newData.widthStartMin;

struct LoadSketch::DataInfo
{
    bool isSubSegment;
    bool hasSubSegment;
    double angleMin;
    double angleMax;
    double lengthMin;
    double lengthMax;
    double widthStartMin;
    double widthStartMax;
    double widthEndMin;
    double widthEndMax;
    double posOnParent;
};

LoadSketch::LoadSketch(QString filename)
{
    readSketch(filename);
}

void LoadSketch::readSketch(QString filename)
{
    bool doneTrunk = false;
    branchIdentifiers = 1;
    QFile file(filename);

    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QXmlStreamReader xmlReader;
    xmlReader.setDevice(&file);

    Branch* tempPointer = tree;

    while (!xmlReader.atEnd() && !xmlReader.hasError())
    {
        LoadSketch::DataInfo trunkData;
        xmlReader.readNext();
        if (xmlReader.isStartElement())
        {
            if (xmlReader.name().toString() == "branch")
            {
                trunkData = DataInfo();
            }
            // Add Root Branch
            QString name = xmlReader.name().toString();
            if (name == "positionOnParent")
                trunkData.posOnParent = xmlReader.readElementText().toDouble();
            if (name == "isSubSegment")
            {
                trunkData.hasSubSegment = true;
                if (xmlReader.readElementText() == "false")
                    trunkData.isSubSegment = false;
                else
                    trunkData.isSubSegment = true;

            }
            if (name == "angleToParent")
                trunkData = parseAngleToParent(xmlReader, trunkData);
            if (name == "thickness")
                trunkData = parseThickness(xmlReader, trunkData);
            if (name == "length")
            {
                trunkData = parseLength(xmlReader, trunkData);

                if (!doneTrunk)
                {
                    Branch* tempNewBranch = new Branch();
                    tempPointer = createBranch(trunkData, tempNewBranch, true, branchIdentifiers);
                    doneTrunk = true;
                }
                else
                {
                    tempPointer = createBranch(trunkData, tempPointer, false, branchIdentifiers);
                }
                branchIdentifiers++;
                xmlReader.readNextStartElement();
            }
            if (name == "children")
            {
            }
        }
        else if (xmlReader.isEndElement() && xmlReader.name().toString() == "children")
        {
            if (!tempPointer->getIsTrunk())
                tempPointer = tempPointer->getParent();
        }
    }
    if (xmlReader.hasError())
    {
        qDebug() << "XML error: " << xmlReader.errorString() << endl;
    }
    else if (xmlReader.atEnd())
    {
        qDebug() << "Reached end, done" << endl;
    }

    tree = tempPointer;
}

Branch* LoadSketch::createBranch(DataInfo data, Branch* parent, bool trunk, long branchId)
{
    Branch* newBranch = new Branch();
    if (trunk)
    {
        QLineF branchLine = QLineF();
        branchLine.setP1(QPointF(300, 500));
        branchLine.setP2(QPointF(1, 1));
        branchLine.setAngle(360-((data.angleMax + data.angleMin) / 2));
        branchLine.setLength(data.lengthMax);
        newBranch->setBranchLine(branchLine);
        newBranch->setIsTrunk(true);
        newBranch->setIsSegment(false);
        newBranch->setIsRootSegment(false);
        newBranch->setPosOnParent(0);
        newBranch->setLengthVariation(data.lengthMin, "lmin");
        newBranch->setLengthVariation(data.lengthMax, "lmax");
        newBranch->setWidthVariation(data.widthStartMin, "smin");
        newBranch->setWidthVariation(data.widthStartMax, "smax");
        newBranch->setWidthVariation(data.widthEndMin, "emin");
        newBranch->setWidthVariation(data.widthEndMax, "emax");

        newBranch->postModificationIntegrity();

        newBranch->updateLengthRect();

        QLineF tempLine = newBranch->getBranchLine();
        tempLine.setAngle(360 - data.angleMin);
        tempLine.setLength(40.0f);
        newBranch->setAngleVariation(tempLine.p2(), "amax");

        newBranch->updateLengthRect();
        newBranch->setIdentifier(branchIdentifiers);

        return newBranch;
    }
    else
    {
        QLineF parentBranchLine = parent->getBranchLine();
        double lengthAlongParent = parentBranchLine.length() * data.posOnParent;
        parentBranchLine.setLength(lengthAlongParent);
        QPointF origPosOnParent = parentBranchLine.p2();
        QLineF branchLine = QLineF();
        branchLine.setP1(origPosOnParent);
        branchLine.setP2(QPointF(0, 0));

        branchLine = parentBranchLine;
        branchLine.setAngle((360 -((data.angleMin + data.angleMax)/2)) + parentBranchLine.angle());
        branchLine.setLength(data.lengthMax);
        newBranch->setBranchLine(branchLine);

        newBranch->setParent(parent);
        parent->addChild(newBranch);

        newBranch->postModificationIntegrity();

        newBranch->setIsTrunk(false);
        newBranch->setIsSegment(false);
        newBranch->setIsRootSegment(false);
        newBranch->setPosOnParent(data.posOnParent);
        newBranch->setOrigPosOnParent(origPosOnParent);

        newBranch->setBranchLine(newBranch->getParent()->moveBranchToEdgeOfRect(origPosOnParent, newBranch->getBranchLine(), newBranch->getPosOnParent()));

        newBranch->setLengthVariation(data.lengthMin, "lmin");
        newBranch->setLengthVariation(data.lengthMax, "lmax");

        newBranch->setWidthVariation(data.widthStartMin, "smin");
        newBranch->setWidthVariation(data.widthStartMax, "smax");
        newBranch->setWidthVariation(data.widthEndMin, "emin");
        newBranch->setWidthVariation(data.widthEndMax, "emax");

        newBranch->updateLengthRect();

        QLineF tempLine = newBranch->getBranchLine();
        //tempLine.setAngle(data.angleMin);
        tempLine.setAngle(tempLine.angle() + 0.05);
        tempLine.setLength(40.0f);
        newBranch->setAngleVariation(tempLine.p2(), "amax");

        newBranch->updateLengthRect();
        newBranch->setIdentifier(branchIdentifiers);

        // Check for segmented branch and if yes, add to segmented branches QVector
        if (data.hasSubSegment)
        {
            if (data.isSubSegment)
            {
                bool branchFound = false;
                // Find current segmented branch and add to it
                for (int i = 0 ; i < curvedBranches.size() ; i++)
                {
                    if (curvedBranches.value(i).hasMember(branchIdentifiers-1))
                    {
                        SegmentedBranch tempSegBranch = curvedBranches.value(i);
                        tempSegBranch.addSegmentId(tree, branchIdentifiers);
                        branchFound = true;
                        curvedBranches.replace(i, tempSegBranch);
                        newBranch->setIsSegment(true);
                        break;
                    }
                }

                // If isn't one, add previous and current identifiers to new segmented branch
                if (!branchFound)
                {
                    SegmentedBranch tempSegBranch = SegmentedBranch();
                    tempSegBranch.addSegmentId(tree, branchIdentifiers-1);
                    tempSegBranch.addSegmentId(tree, branchIdentifiers);
                    newBranch->setIsSegment(true);
                    newBranch->getParent()->setIsRootSegment(true);
                    newBranch->getParent()->setIsSegment(true);
                    curvedBranches.append(tempSegBranch);
                }
            }
        }

        //return parent;
        return newBranch;
    }
}

LoadSketch::DataInfo LoadSketch::parseAngleToParent(QXmlStreamReader& xmlReader, DataInfo data)
{
    xmlReader.readNextStartElement(); // read inclination
    xmlReader.readNextStartElement(); // read min
    QString minInclination = xmlReader.readElementText();
    xmlReader.readNextStartElement(); // read max
    QString maxInclination = xmlReader.readElementText();
    xmlReader.readNextStartElement(); // read azimuth
    xmlReader.readNextStartElement();
    xmlReader.readNextStartElement(); // read min
    QString minAzimuth = xmlReader.readElementText();
    xmlReader.readNextStartElement(); // read max
    QString maxAzimuth = xmlReader.readElementText();

    double tempMinInclination = 2*M_PI - minInclination.toDouble();
    double tempMaxInclination = 2*M_PI - maxInclination.toDouble();

    /*if (tempMinInclination > tempMaxInclination)
       std::swap(tempMinInclination, tempMaxInclination);*/

    /*if (tempMinInclination > 0)
        tempMinInclination = -1 * (tempMinInclination - 2 * M_PI);
    else
        tempMinInclination = -tempMinInclination;

    if (tempMaxInclination > 0)
        tempMaxInclination = -1 * (tempMaxInclination - 2 * M_PI);
    else
        tempMaxInclination = -tempMaxInclination;*/


    data.angleMin = (tempMinInclination * 180.0 / M_PI);
    data.angleMax = (tempMaxInclination * 180.0 / M_PI);

    /*data.angleMin = (minInclination.toDouble() * 180.0 / M_PI);
    data.angleMax = (maxInclination.toDouble() * 180.0 / M_PI);*/
    return data;
}

LoadSketch::DataInfo LoadSketch::parseThickness(QXmlStreamReader& xmlReader, DataInfo data)
{
    xmlReader.readNextStartElement(); // read start
    xmlReader.readNextStartElement(); // read min
    QString minStart = xmlReader.readElementText();
    xmlReader.readNextStartElement(); // read max
    QString maxStart = xmlReader.readElementText();
    xmlReader.readNextStartElement();
    xmlReader.readNextStartElement();
    xmlReader.readNextStartElement(); // read min
    QString minEnd = xmlReader.readElementText();
    xmlReader.readNextStartElement(); // read max
    QString maxEnd = xmlReader.readElementText();

    data.widthStartMin = minStart.toDouble();
    data.widthStartMax = maxStart.toDouble();
    data.widthEndMin = minEnd.toDouble();
    data.widthEndMax = maxEnd.toDouble();
    return data;
}

LoadSketch::DataInfo LoadSketch::parseLength(QXmlStreamReader& xmlReader, DataInfo data)
{
    xmlReader.readNextStartElement(); // read min
    QString minLength = xmlReader.readElementText();
    xmlReader.readNextStartElement(); // read max
    QString maxLength = xmlReader.readElementText();

    data.lengthMin = minLength.toDouble();
    data.lengthMax = maxLength.toDouble();
    return data;
}

void LoadSketch::parseNewChild(QXmlStreamReader& xmlReader, Branch* parent)
{
    Branch* currentTree = parent;
    while (!xmlReader.atEnd() && !xmlReader.hasError())
    {
        LoadSketch::DataInfo trunkData;
        xmlReader.readNext();
        if (xmlReader.isStartElement())
        {
            if (xmlReader.name().toString() == "branch")
            {
                trunkData = DataInfo();
            }
            // Add Root Branch
            QString name = xmlReader.name().toString();
            if (name == "positionOnParent")
                trunkData.posOnParent = xmlReader.readElementText().toDouble();
            if (name == "isSubSegment")
            {
                if (xmlReader.readElementText() == "false")
                    trunkData.isSubSegment = false;
                else
                    trunkData.isSubSegment = true;
            }
            if (name == "angleToParent")
                trunkData = parseAngleToParent(xmlReader, trunkData);
            if (name == "thickness")
                trunkData = parseThickness(xmlReader, trunkData);
            if (name == "length")
            {
                trunkData = parseLength(xmlReader, trunkData);

                createBranch(trunkData, currentTree, false, branchIdentifiers);
                branchIdentifiers++;
            }
            if (name == "children")
            {
                xmlReader.readNextStartElement();
                if (xmlReader.name().toString() == "branch")
                {
                    parseNewChild(xmlReader, currentTree);
                }
            }
        }
    }
    if (xmlReader.hasError())
    {
        qDebug() << "XML error: " << xmlReader.errorString() << endl;
    }
    else if (xmlReader.atEnd())
    {
        qDebug() << "Reached end, done" << endl;
    }
}

Branch* LoadSketch::getTree()
{
    Branch* tempTree = findTrunk(tree);
    //printTree(tempTree);
    return tempTree;
}

QVector<SegmentedBranch> LoadSketch::getCurvedBranches()
{
    return curvedBranches;
}

long LoadSketch::getIdentifierIncrementer()
{
    return branchIdentifiers;
}

void LoadSketch::printTree(Branch* tr)
{
    qDebug() << tr->getIdentifier();
    qDebug() << tr->getBranchLine();

    for (int i = 0 ; i < tr->numChildren() ; i++)
        printTree(tr->childAt(i));
}

Branch* LoadSketch::findTrunk(Branch* tr)
{
    if (tr->getIsTrunk())
        return tr;
    else
        return findTrunk(tr->getParent());
}
