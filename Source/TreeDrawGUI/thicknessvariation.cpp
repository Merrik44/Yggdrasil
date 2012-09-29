#include "thicknessvariation.h"

ThicknessVariation::ThicknessVariation()
{
}

ThicknessVariation::ThicknessVariation(QLineF bl)
{
    startMinLength = 1;
    startMaxLength = 1;
    endMinLength = 1;
    endMaxLength = 1;
    calculatePoints(bl);
}

void ThicknessVariation::calculatePoints(QLineF bl)
{
    branchLine = bl;
    branchNormal = bl.normalVector();
    QLineF tempLine = bl;
    tempLine.setLength(tempLine.length() + 12);
    endMinPoint = tempLine.p2();
    tempLine.setLength(tempLine.length() + 8);
    endMaxPoint = tempLine.p2();
    tempLine = QLineF(bl.p2(), bl.p1());
    tempLine.setLength(tempLine.length() + 15);
    startMinPoint = tempLine.p2();
    tempLine.setLength(tempLine.length() + 8);
    startMaxPoint = tempLine.p2();
}

void ThicknessVariation::updatePoints(QLineF bl)
{
    calculatePoints(bl);
}

void ThicknessVariation::drawLines(QPainter* painter, QPointF panDiff)
{
    QPen pen = QPen();
    pen.setWidthF(2.5);
    painter->setPen(pen);

    QPointF temp = calcVarExtremePoint(branchNormal, startMinPoint, startMinLength);
    QPointF temp2 = findAdjacentPoint(startMinPoint, temp);
    drawBar(painter, temp, temp2, panDiff);
    temp = calcVarExtremePoint(branchNormal, startMaxPoint, startMaxLength);
    temp2 = findAdjacentPoint(startMaxPoint, temp);
    drawBar(painter, temp, temp2, panDiff);
    temp = calcVarExtremePoint(branchNormal, endMinPoint, endMinLength);
    temp2 = findAdjacentPoint(endMinPoint, temp);
    drawBar(painter, temp, temp2, panDiff);
    temp = calcVarExtremePoint(branchNormal, endMaxPoint, endMaxLength);
    temp2 = findAdjacentPoint(endMaxPoint, temp);
    drawBar(painter, temp, temp2, panDiff);
}

void ThicknessVariation::drawBar(QPainter* painter, QPointF point1, QPointF point2, QPointF panDiff)
{
    painter->drawLine(point1 + panDiff, point2 + panDiff);
    painter->drawEllipse(point1 + panDiff, 1.5, 1.5);
    painter->drawEllipse(point2 + panDiff, 1.5, 1.5);
}

QPointF ThicknessVariation::calcVarExtremePoint(QLineF norm, QPointF point, float len)
{
    norm.setLength(len);
    QPointF newPoint = norm.p2();
    QPointF tempPoint = norm.p1() - point;
    newPoint = newPoint - tempPoint;
    return newPoint;
}

QPointF ThicknessVariation::findAdjacentPoint(QPointF mid, QPointF point)
{
    QPointF newPoint;
    newPoint.setX(midpointCalculation(mid.x(), point.x()));
    newPoint.setY(midpointCalculation(mid.y(), point.y()));
    return newPoint;
}

float ThicknessVariation::midpointCalculation(float mid, float point)
{
    float newFloat = 2 * mid - point;
    return newFloat;
}

void ThicknessVariation::modifyVariation(QPointF mousePoint, QPointF prevPoint, QString varType)
{
    QPointF temp, temp2;
    if (varType.compare("smin") == 0)
    {
        temp = calcVarExtremePoint(branchNormal, startMinPoint, startMinLength);
        temp2 = findAdjacentPoint(startMinPoint, temp);
    }
    else if (varType.compare("smax") == 0)
    {
        temp = calcVarExtremePoint(branchNormal, startMaxPoint, startMaxLength);
        temp2 = findAdjacentPoint(startMaxPoint, temp);
    }
    else if (varType.compare("emin") == 0)
    {
        temp = calcVarExtremePoint(branchNormal, endMinPoint, endMinLength);
        temp2 = findAdjacentPoint(endMinPoint, temp);
    }
    else
    {
        temp = calcVarExtremePoint(branchNormal, endMaxPoint, endMaxLength);
        temp2 = findAdjacentPoint(endMaxPoint, temp);
    }

    QPointF point1 = closestPointOnLine(temp, temp2, mousePoint);
    QPointF point2 = closestPointOnLine(temp, temp2, prevPoint);

    float newDist;

    float disPoint1, disPoint2;
    if (varType.compare("smin") == 0)
    {
        disPoint1 = distanceBetween(point1, startMinPoint);
        disPoint2 = distanceBetween(point2, startMinPoint);
        newDist = disPoint1 - disPoint2;
        startMinLength = startMinLength + newDist;
        if (startMinLength < 1)
            startMinLength = 1;
        checkMaxEndWidthLimits();
    }
    if (varType.compare("smax") == 0)
    {
        disPoint1 = distanceBetween(point1, startMaxPoint);
        disPoint2 = distanceBetween(point2, startMaxPoint);
        newDist = disPoint1 - disPoint2;
        startMaxLength = startMaxLength + newDist;
        if (startMaxLength < 1)
            startMaxLength = 1;
        checkMaxEndWidthLimits();
    }
    if (varType.compare("emin") == 0)
    {
        disPoint1 = distanceBetween(point1, endMinPoint);
        disPoint2 = distanceBetween(point2, endMinPoint);
        newDist = disPoint1 - disPoint2;
        endMinLength = endMinLength + newDist;
        if (endMinLength < 1)
            endMinLength = 1;
        checkMaxStartWidthLimits();
    }
    if (varType.compare("emax") == 0)
    {
        disPoint1 = distanceBetween(point1, endMaxPoint);
        disPoint2 = distanceBetween(point2, endMaxPoint);
        newDist = disPoint1 - disPoint2;
        endMaxLength = endMaxLength + newDist;
        if (endMaxLength < 1)
            endMaxLength = 1;
        checkMaxStartWidthLimits();
    }
}

float ThicknessVariation::getVariationLength(QString varType)
{
    if (varType.compare("smin") == 0)
    {
        return startMinLength;
    }
    else if (varType.compare("smax") == 0)
    {
        return startMaxLength;
    }
    else if (varType.compare("emin") == 0)
    {
        return endMinLength;
    }
    else
    {
        return endMaxLength;
    }
}

QPointF ThicknessVariation::closestPointOnLine(QPointF a, QPointF b, QPointF c)
{
    QPointF ac = c-a;
    QPointF ab = b-a;
    float ab2 = (ab.x() * ab.x()) + (ab.y() * ab.y());
    float ac_ab = (ac.x() * ab.x()) + (ac.y() * ab.y());
    float t = ac_ab / ab2;
    QPointF closest = a + (ab * t);
    return closest;
}

float ThicknessVariation::distanceBetween(QPointF a, QPointF b)
{
    int x = (a.x() - b.x()) * (a.x() - b.x());
    int y = (a.y() - b.y()) * (a.y() - b.y());
    float dist = sqrt(x + y);
    return dist;
}

QString ThicknessVariation::checkOnPoint(QPointF mousePoint)
{
    QPointF temp1 = calcVarExtremePoint(branchNormal, startMinPoint, startMinLength);
    QPointF temp2 = findAdjacentPoint(startMinPoint, temp1);
    float disPoint1 = distanceBetween(temp1, mousePoint);
    float disPoint2 = distanceBetween(temp2, mousePoint);
    if (disPoint1 < 5.0f || disPoint2 < 5.0f)
    {
        if (disPoint1 < disPoint2)
            distToWidget = disPoint1;
        else
            distToWidget = disPoint2;
        return "smin";
    }
    temp1 = calcVarExtremePoint(branchNormal, startMaxPoint, startMaxLength);
    temp2 = findAdjacentPoint(startMaxPoint, temp1);
    disPoint1 = distanceBetween(temp1, mousePoint);
    disPoint2 = distanceBetween(temp2, mousePoint);
    if (disPoint1 < 5.0f || disPoint2 < 5.0f)
    {
        if (disPoint1 < disPoint2)
            distToWidget = disPoint1;
        else
            distToWidget = disPoint2;
        return "smax";
    }
    temp1 = calcVarExtremePoint(branchNormal, endMinPoint, endMinLength);
    temp2 = findAdjacentPoint(endMinPoint, temp1);
    disPoint1 = distanceBetween(temp1, mousePoint);
    disPoint2 = distanceBetween(temp2, mousePoint);
    if (disPoint1 < 5.0f || disPoint2 < 5.0f)
    {
        if (disPoint1 < disPoint2)
            distToWidget = disPoint1;
        else
            distToWidget = disPoint2;
        return "emin";
    }
    temp1 = calcVarExtremePoint(branchNormal, endMaxPoint, endMaxLength);
    temp2 = findAdjacentPoint(endMaxPoint, temp1);
    disPoint1 = distanceBetween(temp1, mousePoint);
    disPoint2 = distanceBetween(temp2, mousePoint);
    if (disPoint1 < 5.0f || disPoint2 < 5.0f)
    {
        if (disPoint1 < disPoint2)
            distToWidget = disPoint1;
        else
            distToWidget = disPoint2;
        return "emax";
    }
    else
        return "NULL";
}

float ThicknessVariation::getDistanceToWidget()
{
    return distToWidget;
}

void ThicknessVariation::setVariationLength(float len, QString varType)
{
    if (varType.compare("smin") == 0)
    {
        startMinLength = len;
        checkMaxEndWidthLimits();
    }
    else if (varType.compare("smax") == 0)
    {
        startMaxLength = len;
        checkMaxEndWidthLimits();
    }
    else if (varType.compare("emin") == 0)
    {
        endMinLength = len;
        checkMaxStartWidthLimits();
    }
    else
    {
        endMaxLength = len;
        checkMaxStartWidthLimits();
    }
}

void ThicknessVariation::setCurveVariationLength(float len, QString varType)
{
    if (varType.compare("smin") == 0)
        startMinLength = len;
    else if (varType.compare("smax") == 0)
        startMaxLength = len;
    else if (varType.compare("emin") == 0)
        endMinLength = len;
    else
        endMaxLength = len;
}

QPointF ThicknessVariation::getVariationPoint(QString varType)
{
    if (varType.compare("smin") == 0)
        return startMinPoint;
    else if (varType.compare("smax") == 0)
        return startMaxPoint;
    else if (varType.compare("emin") == 0)
        return endMinPoint;
    else
        return endMaxPoint;
}

float ThicknessVariation::getStartMin()
{
    if (startMinLength < startMaxLength)
        return startMinLength;
    else
        return startMaxLength;
}

float ThicknessVariation::getStartMax()
{
    if (startMinLength < startMaxLength)
        return startMaxLength;
    else
        return startMinLength;
}

float ThicknessVariation::getEndMin()
{
    if (endMinLength < endMaxLength)
        return endMinLength;
    else
        return endMaxLength;
}

float ThicknessVariation::getEndMax()
{
    if (endMinLength < endMaxLength)
        return endMaxLength;
    else
        return endMinLength;
}

void ThicknessVariation::checkMaxEndWidthLimits()
{
    if (endMinLength >= endMaxLength)
    {
        if (startMinLength < startMaxLength)
        {
            if (endMinLength > startMaxLength)
                endMinLength = startMaxLength;
        }
        else
            if (endMinLength > startMinLength)
                endMinLength = startMinLength;
    }
    if (endMaxLength >= endMinLength)
    {
        if (startMinLength < startMaxLength)
        {
            if (endMaxLength > startMaxLength)
                endMaxLength = startMaxLength;
        }
        else
            if (endMaxLength > startMinLength)
                endMaxLength = startMinLength;
    }
}

void ThicknessVariation::checkMaxStartWidthLimits()
{
    if (endMinLength >= endMaxLength)
    {
        if (startMinLength < startMaxLength)
        {
            if (endMinLength > startMaxLength)
                startMaxLength = endMinLength;
        }
        else
            if (endMinLength > startMinLength)
                startMinLength = endMinLength;
    }
    if (endMaxLength >= endMinLength)
    {
        if (startMinLength < startMaxLength)
        {
            if (endMaxLength > startMaxLength)
                startMaxLength = endMaxLength;
        }
        else
            if (endMaxLength > startMinLength)
                startMinLength = endMaxLength;
    }
}

void ThicknessVariation::setDefaultThickness(float width)
{
    startMinLength = width;
    startMaxLength = width;
    endMinLength = width;
    endMaxLength = width;
    updatePoints(branchLine);
}
