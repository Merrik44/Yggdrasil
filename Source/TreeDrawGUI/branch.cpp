#include "branch.h"

Branch::Branch()
{
    setThickness();
    setIsTrunk(false);
    setIsSelected(false);
}

Branch::Branch(QPointF sp, QPointF ep)
{
    setPos(sp, ep);
    setThickness();
}

Branch::Branch(QPointF sp, QPointF ep, Branch* par, double iden)
{
    setPos(sp, ep);
    setParent(par);
    setThickness();
    setIdentifier(iden);
}

void Branch::setPos(QPointF sp, QPointF ep)
{
    startPos = sp;
    endPos = ep;
    branchLine = QLineF(sp, ep);
    setThickness();
    setIsTrunk(false);
    setIsSelected(false);

    minRect = new QPointF[4];
    maxRect = new QPointF[4];
    updateLengthRect();
}

void Branch::drawLine(QPainter* painter, QPointF panDiff)
{
    /*drawLineWidth(painter);
    drawLineLength(painter);*/
    drawBranches(painter, panDiff);
}

void Branch::drawLineWidth(QPainter* painter)
{
    QPen pen = QPen();
    pen.setWidth(2);
    pen.setColor(QColor(0, 130, 0, 255));
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor(0, 140, 0, 255), Qt::SolidPattern));

    QLineF normal = (new QLineF(startPos, endPos))->normalVector();

    QPointF pointsMin[4], pointsMax[4];
    if (tv.getVariationLength("smin") > tv.getVariationLength("smax"))
    {
        pointsMin[0] = calcVarExtremePoint(normal, startPos, tv.getVariationLength("smax"));
        pointsMax[0] = calcVarExtremePoint(normal, startPos, tv.getVariationLength("smin"));
    }
    else
    {
        pointsMin[0] = calcVarExtremePoint(normal, startPos, tv.getVariationLength("smin"));
        pointsMax[0] = calcVarExtremePoint(normal, startPos, tv.getVariationLength("smax"));
    }

    pointsMin[1] = findAdjacentPoint(startPos, pointsMin[0]);
    pointsMax[1] = findAdjacentPoint(startPos, pointsMax[0]);

    if (tv.getVariationLength("emin") > tv.getVariationLength("emax"))
    {
        pointsMin[3] = calcVarExtremePoint(normal, endPos, tv.getVariationLength("emax"));
        pointsMax[3] = calcVarExtremePoint(normal, endPos, tv.getVariationLength("emin"));
    }
    else
    {
        pointsMin[3] = calcVarExtremePoint(normal, endPos, tv.getVariationLength("emin"));
        pointsMax[3] = calcVarExtremePoint(normal, endPos, tv.getVariationLength("emax"));
    }

    pointsMin[2] = findAdjacentPoint(endPos, pointsMin[3]);
    pointsMax[2] = findAdjacentPoint(endPos, pointsMax[3]);

    QLineF lineMin1 = QLineF(pointsMin[0], pointsMin[3]);
    QLineF lineMin2 = QLineF(pointsMin[1], pointsMin[2]);

    QLineF lineMax1 = QLineF(pointsMax[0], pointsMax[3]);
    QLineF lineMax2 = QLineF(pointsMax[1], pointsMax[2]);

    if (branchLine.length() > lv.getMinLength() && branchLine.length() < lv.getMaxLength())
    {
        painter->drawPolygon(pointsMax, 4);

        pen.setColor(QColor(0, 75, 0, 255));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(0, 75, 0, 255), Qt::SolidPattern));
        painter->drawPolygon(pointsMin, 4);
    }

    if (branchLine.length() > lv.getMaxLength())
    {
        QPointF point1 = calcVarExtremePoint(normal, lv.getMaxPoint(), 100);
        QPointF point2 = findAdjacentPoint(lv.getMaxPoint(), point1);
        QLineF maxLine = QLineF(point1, point2);

        QPointF newMaxPoints[2];
        newMaxPoints[0] = calculateLineIntersection(maxLine, lineMax1);
        newMaxPoints[1] = calculateLineIntersection(maxLine, lineMax2);

        QPointF newMinPoints[2];
        newMinPoints[0] = calculateLineIntersection(maxLine, lineMin1);
        newMinPoints[1] = calculateLineIntersection(maxLine, lineMin2);

        QPointF solidMin[4];
        QPointF solidMax[4];
        QPointF semiMin[4];
        QPointF semiMax[4];

        solidMin[0] = pointsMin[0];
        solidMin[1] = pointsMin[1];
        solidMin[2] = newMinPoints[1];
        solidMin[3] = newMinPoints[0];

        solidMax[0] = pointsMax[0];
        solidMax[1] = pointsMax[1];
        solidMax[2] = newMaxPoints[1];
        solidMax[3] = newMaxPoints[0];

        semiMin[0] = newMinPoints[0];
        semiMin[1] = newMinPoints[1];
        semiMin[2] = pointsMin[2];
        semiMin[3] = pointsMin[3];

        semiMax[0] = newMaxPoints[0];
        semiMax[1] = newMaxPoints[1];
        semiMax[2] = pointsMax[2];
        semiMax[3] = pointsMax[3];

        pen = QPen();
        pen.setWidth(2);
        pen.setColor(QColor(0, 130, 0, 255));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(0, 140, 0, 255), Qt::SolidPattern));
        painter->drawPolygon(solidMax, 4);

        pen.setColor(QColor(0, 75, 0, 255));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(0, 75, 0, 255), Qt::SolidPattern));
        painter->drawPolygon(solidMin, 4);

        pen = QPen();
        pen.setWidth(2);
        pen.setColor(QColor(0, 130, 0, 100));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(0, 140, 0, 50), Qt::SolidPattern));
        painter->drawPolygon(semiMax, 4);

        pen.setColor(QColor(0, 75, 0, 100));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(0, 75, 0, 50), Qt::SolidPattern));
        painter->drawPolygon(semiMin, 4);
    }
    else if (branchLine.length() < lv.getMinLength())
    {
        QPointF point1 = calcVarExtremePoint(normal, lv.getMinPoint(), 100);
        QPointF point2 = findAdjacentPoint(lv.getMinPoint(), point1);
        QLineF minLine = QLineF(point1, point2);

        QPointF newMaxPoints[2];
        newMaxPoints[0] = calculateLineIntersection(minLine, lineMax1);
        newMaxPoints[1] = calculateLineIntersection(minLine, lineMax2);

        QPointF newMinPoints[2];
        newMinPoints[0] = calculateLineIntersection(minLine, lineMin1);
        newMinPoints[1] = calculateLineIntersection(minLine, lineMin2);

        QPointF semiMin[4];
        QPointF semiMax[4];

        semiMin[0] = pointsMin[3];
        semiMin[1] = pointsMin[2];
        semiMin[2] = newMinPoints[1];
        semiMin[3] = newMinPoints[0];

        semiMax[0] = pointsMax[3];
        semiMax[1] = pointsMax[2];
        semiMax[2] = newMaxPoints[1];
        semiMax[3] = newMaxPoints[0];

        painter->drawPolygon(pointsMax, 4);

        pen.setColor(QColor(0, 75, 0, 255));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(0, 75, 0, 255), Qt::SolidPattern));
        painter->drawPolygon(pointsMin, 4);

        pen = QPen();
        pen.setWidth(2);
        pen.setColor(QColor(0, 130, 0, 100));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(0, 140, 0, 50), Qt::SolidPattern));
        painter->drawPolygon(semiMax, 4);

        pen.setColor(QColor(0, 75, 0, 100));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(0, 75, 0, 50), Qt::SolidPattern));
        painter->drawPolygon(semiMin, 4);
    }
}

void Branch::drawLineLength(QPainter* painter)
{

    QPen pen = QPen();
    pen.setWidth(1);
    pen.setColor(QColor(0, 130, 0, 255));
    painter->setPen(pen);
    //painter->setBrush(QBrush(QColor(0, 140, 0, 255), Qt::SolidPattern));
    painter->setBrush(QBrush());

    QPointF pointsMin[4], pointsMax[4];

    QLineF normal = (new QLineF(startPos, endPos))->normalVector();
    float rectangleWidth = calcLengthRect();

    pointsMin[0] = calcVarExtremePoint(normal, startPos, rectangleWidth);
    pointsMin[1] = findAdjacentPoint(startPos, pointsMin[0]);
    pointsMin[3] = calcVarExtremePoint(normal, lv.getMinPoint() , rectangleWidth);
    pointsMin[2] = findAdjacentPoint(lv.getMinPoint(), pointsMin[3]);

    rectangleWidth += 3;
    pointsMax[0] = calcVarExtremePoint(normal, startPos, rectangleWidth);
    pointsMax[1] = findAdjacentPoint(startPos, pointsMax[0]);
    pointsMax[3] = calcVarExtremePoint(normal, lv.getMaxPoint(), rectangleWidth);
    pointsMax[2] = findAdjacentPoint(lv.getMaxPoint(), pointsMax[3]);

    updateLengthRect();

    pen = QPen();
    pen.setWidth(2);
    pen.setColor(QColor(0, 130, 0, 255));
    painter->setPen(pen);

    painter->drawPolygon(minRect, 4);


    pen.setColor(QColor(0, 75, 0, 255));
    painter->setPen(pen);

    painter->drawPolygon(maxRect, 4);
}

void Branch::drawBranches(QPainter* painter, QPointF panDiff)
{
    QPen pen = QPen();
    pen.setWidth(2);

    QPointF maxRectTemp[4];
    maxRectTemp[0] = maxRect[0] + panDiff;
    maxRectTemp[1] = maxRect[1] + panDiff;
    maxRectTemp[2] = maxRect[2] + panDiff;
    maxRectTemp[3] = maxRect[3] + panDiff;

    QPointF minRectTemp[4];
    minRectTemp[0] = minRect[0] + panDiff;
    minRectTemp[1] = minRect[1] + panDiff;
    minRectTemp[2] = minRect[2] + panDiff;
    minRectTemp[3] = minRect[3] + panDiff;

    if (!getIsSelected())
    {
        pen.setColor(QColor(0, 130, 0, 95));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(0, 130, 0, 95), Qt::SolidPattern));
        painter->drawPolygon(maxRectTemp, 4);

        pen = QPen();
        pen.setWidth(2);
        pen.setColor(QColor(0, 75, 0, 95));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(0, 75, 0, 95), Qt::SolidPattern));
        painter->drawPolygon(minRectTemp, 4);
    }
    else
    {
        pen.setColor(QColor(0, 130, 0, 255));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(0, 130, 0, 255), Qt::SolidPattern));
        painter->drawPolygon(maxRectTemp, 4);

        pen = QPen();
        pen.setWidth(2);
        pen.setColor(QColor(0, 75, 0, 255));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(0, 75, 0, 255), Qt::SolidPattern));
        painter->drawPolygon(minRectTemp, 4);
    }
}

bool Branch::isChildSelected(Branch* tree)
{
    bool isSelected = tree->getIsSelected();
    bool childSelected = false;
    for (int i = 0 ; i < tree->numChildren() ; i++)
        if (isChildSelected(tree->childAt(i)))
            childSelected = true;
    if (isSelected || childSelected)
        return true;
    else
        return false;
}

void Branch::updateLengthRect()
{
    QLineF normal = (new QLineF(startPos, endPos))->normalVector();

    minRect[0] = calcVarExtremePoint(normal, startPos, tv.getStartMin());
    minRect[1] = findAdjacentPoint(startPos, minRect[0]);
    minRect[3] = calcVarExtremePoint(normal, lv.getMinPoint(), tv.getEndMin());
    minRect[2] = findAdjacentPoint(lv.getMinPoint(), minRect[3]);

    maxRect[0] = calcVarExtremePoint(normal, startPos, tv.getStartMax());
    maxRect[1] = findAdjacentPoint(startPos, maxRect[0]);
    maxRect[3] = calcVarExtremePoint(normal, lv.getMaxPoint(), tv.getEndMax());
    maxRect[2] = findAdjacentPoint(lv.getMaxPoint(), maxRect[3]);
}

QLineF Branch::moveBranchToEdgeOfRect(QPointF currPoint, QLineF newBranch, float posOnParent)
{
    QLineF tempLine = QLineF(branchLine.p1(), lv.getMaxPoint());
    tempLine.setLength(tempLine.length() * posOnParent);

    currPoint = tempLine.p2();

    QPointF differenceVec = newBranch.p1() - currPoint;
    newBranch.setP1(currPoint);
    newBranch.setP2(newBranch.p2() - differenceVec);

    QLineF tempNewBranch = newBranch;
    tempNewBranch.setLength(1000);

    QLineF sideLine1 = QLineF(maxRect[0], maxRect[3]);
    QLineF sideLine1Temp = sideLine1;
    sideLine1Temp.setLength(1000);
    sideLine1Temp = QLineF(sideLine1Temp.p2(), sideLine1Temp.p1());
    sideLine1Temp.setLength(1000);
    QLineF sideLine2 = QLineF(maxRect[1], maxRect[2]);
    QLineF topLine = QLineF(maxRect[2], maxRect[3]);

    QLineF finalLine;

    if (posOnParent < 0.95)
    {
        bool isSide1 = doLineSegmentsIntersect(sideLine1Temp, tempNewBranch);
        // find relative position of point on branchLine
        //float fullDistance = findDistance(branchLine.p1(), branchLine.p2());
        //float segDistance = findDistance(branchLine.p1(), currPoint);
        //float posOnParent = segDistance / fullDistance;

        // transpose to relative position on intersected rectangle line
        float outerRectLength;
        if (isSide1)
            outerRectLength = sideLine1.length();
        else
            outerRectLength = sideLine2.length();
        outerRectLength *= posOnParent;
        QPointF newStartPos;
        if (isSide1)
        {
            sideLine1.setLength(outerRectLength);
            newStartPos = sideLine1.p2();
        }
        else
        {
            sideLine2.setLength(outerRectLength);
            newStartPos = sideLine2.p2();
        }
        // find vector from point to newPoint
        QPointF changeVector = currPoint - newStartPos;
        // create newLine which moves newBranch by newPointVector
        finalLine.setP1(newStartPos);
        finalLine.setP2(newBranch.p2() - changeVector);
    }
    else
    {
        QPointF midPoint;
        midPoint.setX((topLine.p1().x() + topLine.p2().x())/2);
        midPoint.setY((topLine.p1().y() + topLine.p2().y())/2);

        QPointF diffVec = currPoint - midPoint;
        finalLine.setP1(midPoint);
        finalLine.setP2(newBranch.p2() - diffVec);
    }

    return finalLine;
}

float Branch::findDistance(QPointF a, QPointF b)
{
    int x = (a.x() - b.x()) * (a.x() - b.x());
    int y = (a.y() - b.y()) * (a.y() - b.y());
    float dist = sqrt(x + y);
    return dist;
}

bool Branch::isOnSegment(double xi, double yi, double xj, double yj, double xk, double yk)
{
    return (xi <= xk || xj <= xk) && (xk <= xi || xk <= xj) &&
            (yi <= yk || yj <= yk) && (yk <= yi || yk <= yj);
}

char Branch::computeDirection(double xi, double yi, double xj, double yj, double xk, double yk)
{
    double a = (xk - xi) * (yj - yi);
    double b = (xj - xi) * (yk - yi);
    return a < b ? -1 : a > b ? 1 : 0;
}

bool Branch::doLineSegmentsIntersect(QLineF line1, QLineF line2)
{
    char d1 = computeDirection(line2.p1().x(), line2.p1().y(), line2.p2().x(), line2.p2().y(), line1.p1().x(), line1.p1().y());
    char d2 = computeDirection(line2.p1().x(), line2.p1().y(), line2.p2().x(), line2.p2().y(), line1.p2().x(), line1.p2().y());
    char d3 = computeDirection(line1.p1().x(), line1.p1().y(), line1.p2().x(), line1.p2().y(), line2.p1().x(), line2.p1().y());
    char d4 = computeDirection(line1.p1().x(), line1.p1().y(), line1.p2().x(), line1.p2().y(), line2.p2().x(), line2.p2().y());
    return (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
            ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) ||
            (d1 == 0 && isOnSegment(line2.p1().x(), line2.p1().y(), line2.p2().x(), line2.p2().y(), line1.p1().x(), line1.p1().y())) ||
            (d2 == 0 && isOnSegment(line2.p1().x(), line2.p1().y(), line2.p2().x(), line2.p2().y(), line1.p2().x(), line1.p2().y())) ||
            (d3 == 0 && isOnSegment(line1.p1().x(), line1.p1().y(), line1.p2().x(), line1.p2().y(), line2.p1().x(), line2.p1().y())) ||
            (d4 == 0 && isOnSegment(line1.p1().x(), line1.p1().y(), line1.p2().x(), line1.p2().y(), line2.p2().x(), line2.p2().y()));
}

QPointF Branch::calculateLineIntersection(QLineF line1, QLineF line2)
{
    float d = (line1.p1().x() - line1.p2().x()) * (line2.p1().y() - line2.p2().y()) - (line1.p1().y() - line1.p2().y()) * (line2.p1().x() - line2.p2().x());

    float pre = (line1.p1().x() * line1.p2().y() - line1.p1().y() * line1.p2().x());
    float post = (line2.p1().x() * line2.p2().y() - line2.p1().y() * line2.p2().x());
    QPointF intersectionPoint;
    intersectionPoint.setX((pre * (line2.p1().x() - line2.p2().x()) - post * (line1.p1().x() - line1.p2().x())) / d);
    intersectionPoint.setY((pre * (line2.p1().y() - line2.p2().y()) - post * (line1.p1().y() - line1.p2().y())) / d);

    return intersectionPoint;
}

float Branch::calcLengthRect()
{
    float longestWidthVar = tv.getVariationLength("smin");

    if (tv.getVariationLength("smax") > longestWidthVar)
        longestWidthVar = tv.getVariationLength("smax");
    if (tv.getVariationLength("emin") > longestWidthVar)
        longestWidthVar = tv.getVariationLength("emin");
    if (tv.getVariationLength("emax") > longestWidthVar)
        longestWidthVar = tv.getVariationLength("emax");

    return longestWidthVar + 10;
}

QPointF Branch::calcVarExtremePoint(QLineF norm, QPointF point, float len)
{
    norm.setLength(len);
    QPointF newPoint = norm.p2();
    QPointF tempPoint = norm.p1() - point;
    newPoint = newPoint - tempPoint;
    return newPoint;
}

QPointF Branch::findAdjacentPoint(QPointF mid, QPointF point)
{
    QPointF newPoint;
    newPoint.setX(midpointCalculation(mid.x(), point.x()));
    newPoint.setY(midpointCalculation(mid.y(), point.y()));
    return newPoint;
}

float Branch::midpointCalculation(float mid, float point)
{
    float newFloat = 2 * mid - point;
    return newFloat;
}

void Branch::setParent(Branch* par)
{
    parent = par;
}

void Branch::setThickness()
{
    tv = ThicknessVariation(branchLine);
    lv = LengthVariation(branchLine);
    av = AngleVariation(branchLine);
}

void Branch::updateVariation()
{
    av.updatePoints(branchLine);
    lv.updatePoints(branchLine);

    QLineF newBranchLine = QLineF(branchLine.p1(), lv.getMaxPoint());
    tv.updatePoints(newBranchLine);
}

void Branch::applyCalculatePosOnRect()
{
    QLineF temp = parent->moveBranchToEdgeOfRect(origPosOnParent, branchLine, posOnParent);
    //setBranchLine(temp);
    branchLine = temp;
    startPos = branchLine.p1();
    endPos = branchLine.p2();

    updateVariation();
    updateLengthRect();

    lv.updateLengths();

    // apply to children
    for (int i = 0 ; i < numChildren() ; i++)
        children[i]->applyCalculatePosOnRect();
}

void Branch::addChild(Branch* child)
{
    children.append(child);
}

void Branch::setBranchLine(QLineF newBranch)
{
    branchLine = newBranch;
    setPos(branchLine.p1(), branchLine.p2());
    updateLengthRect();
}

QPointF Branch::getStartPos()
{
    return startPos;
}

QPointF Branch::getEndPos()
{
    return endPos;
}

int Branch::numChildren()
{
    return children.count();
}

Branch* Branch::childAt(int i)
{
    return children.at(i);
}

QLineF Branch::getBranchLine()
{
    return branchLine;
}

void Branch::drawVariation(QPainter *p, QPointF panDiff)
{
    tv.drawLines(p, panDiff);
    lv.drawLines(p, panDiff);
    av.drawLines(p, panDiff);
}

void Branch::drawAngleVariation(QPainter *p, QPointF panDiff)
{
    av.drawLines(p, panDiff);
}

void Branch::modifyWidthVariation(QPointF mousePoint, QPointF prevPoint, QString varType)
{
    tv.modifyVariation(mousePoint, prevPoint, varType);

    postModificationIntegrity();
}

void Branch::modifyLengthVariation(QPointF mousePoint, QString varType)
{
    lv.modifyVariation(mousePoint, varType);

    postModificationIntegrity();
}

void Branch::postModificationIntegrity()
{
    updateLengthRect();

    for (int i = 0 ; i < numChildren() ; i++)
        children[i]->applyCalculatePosOnRect();

    tv.updatePoints(QLineF(startPos, lv.getMaxPoint()));

    updateLengthRect();
}

void Branch::modifyAngleVariation(QPointF mousePoint)
{
    av.modifyVariation(mousePoint);
    if (!getIsTrunk())
        av.checkParentBranchIntersection(parent->getMaxRect());
}

QString Branch::checkOnPoint(QPointF mousePoint)
{
    return tv.checkOnPoint(mousePoint);
}

QString Branch::checkOnLine(QPointF mousePoint)
{
    return lv.checkOnLine(mousePoint);
}

QString Branch::checkOnLineA(QPointF mousePoint)
{
    return av.checkOnLineA(mousePoint);
}

float Branch::getThicknessVariationStartMin()
{
    return tv.getStartMin();
}

float Branch::getThicknessVariationStartMax()
{
    return tv.getStartMax();
}

float Branch::getThicknessVariationEndMin()
{
    return tv.getEndMin();
}

float Branch::getThicknessVariationEndMax()
{
    return tv.getEndMax();
}

float Branch::getPosOnParent()
{
    return posOnParent;
}

void Branch::setPosOnParent(float posOnPar)
{
    posOnParent = posOnPar;
}

void Branch::setOrigPosOnParent(QPointF origPos)
{
    origPosOnParent = origPos;
}

QPointF Branch::getOrigPosOnParent()
{
    return origPosOnParent;
}

Branch* Branch::getParent()
{
    return parent;
}

void Branch::setIsTrunk(bool isT)
{
    isTrunk = isT;
}

bool Branch::getIsTrunk()
{
    return isTrunk;
}

void Branch::setIsSelected(bool isS)
{
    isSelected = isS;
}

bool Branch::getIsSelected()
{
    return isSelected;
}

void Branch::removeChildAt(int ind)
{
    children.remove(ind);
}

void Branch::setIdentifier(long iden)
{
    identifier = iden;
}

long Branch::getIdentifier()
{
    return identifier;
}

void Branch::setWidthVariation(float len, QString vt)
{
    tv.setVariationLength(len, vt);

    postModificationIntegrity();
}

void Branch::setCurveWidthVariation(float len, QString vt)
{
    tv.setCurveVariationLength(len, vt);

    postModificationIntegrity();
}

float Branch::getWidthVariation(QString vt)
{
    return tv.getVariationLength(vt);
}

void Branch::setLengthVariation(float length, QString varType)
{
    lv.setLengthVariation(length, varType);

    postModificationIntegrity();
}

float Branch::getLengthVariation(QString vt)
{
    return lv.getLengthVariation(vt);
}

void Branch::setAngleVariation(QPointF point, QString varType)
{
    av.setAngleVariation(point, varType);

    if (!getIsTrunk())
        av.checkParentBranchIntersection(parent->getMaxRect());
}

QPointF Branch::getAngleVariation(QString varType)
{
    return av.getAngleVariation(varType);
}

float Branch::getLengthVariationMin()
{
    return lv.getMinLength();
}

float Branch::getLengthVariationMax()
{
    return lv.getMaxLength();
}

QLineF Branch::getAngleLineMin()
{
    return av.getMinLine();
}

QLineF Branch::getAngleLineMax()
{
    return av.getMaxLine();
}

QPointF* Branch::getMaxRect()
{
    QPointF* maxRectTemp = maxRect;
    return maxRectTemp;
}

QPointF Branch::getLengthMaxPoint()
{
    return lv.getMaxPoint();
}

QPointF Branch::getLengthMinPoint()
{
    return lv.getMinPoint();
}

Branch::~Branch()
{
}

void Branch::setIsSegment(bool seg)
{
    isSegment = seg;
}

bool Branch::getIsSegment()
{
    return isSegment;
}

void Branch::setIsRootSegment(bool rootSegment)
{
    isRootSegment = rootSegment;
}

bool Branch::getIsRootSegment()
{
    return isRootSegment;
}

void Branch::setSegmentId(int seg)
{
    segmentId = seg;
}

int Branch::getSegmentId()
{
    return segmentId;
}

QPointF Branch::getWidthVariationPoint(QString varType)
{
    return tv.getVariationPoint(varType);
}

float Branch::getDistanceToAngleWidget()
{
    return av.getDistanceToWidget();
}

float Branch::getDistanceToLengthWidget()
{
    return lv.getDistanceToWidget();
}

float Branch::getDistanceToThicknessWidget()
{
    return tv.getDistanceToWidget();
}

void Branch::setDefaultThickness(float width)
{
    tv.setDefaultThickness(width);
}
