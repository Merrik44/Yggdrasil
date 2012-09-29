#include "lengthvariation.h"

#define DEFAULT_VARIATION 1

LengthVariation::LengthVariation()
{
}

LengthVariation::LengthVariation(QLineF bl)
{
    calcPoints(bl, DEFAULT_VARIATION);
    lineWidth = 25;
}

void LengthVariation::updatePoints(QLineF bl)
{
    float distToMin = findDistance(branchLine.p1(), minPoint);
    float distToMax = findDistance(branchLine.p1(), maxPoint);

    branchLine = bl;
    QLineF tempLine = branchLine;
    tempLine.setLength(distToMin);
    minPoint = tempLine.p2();
    tempLine.setLength(distToMax);
    maxPoint = tempLine.p2();
}

void LengthVariation::calcPoints(QLineF bl, float variation)
{
    branchLine = bl;
    normalLine = bl.normalVector();
    QLineF tempLine = bl;
    tempLine.setLength(tempLine.length());
    minPoint = tempLine.p2();
    tempLine = bl;
    tempLine.setLength(tempLine.length() + (tempLine.length() * variation/100));
    maxPoint = tempLine.p2();

    minLength = findDistance(branchLine.p1(), minPoint);
    maxLength = findDistance(branchLine.p1(), maxPoint);
}

void LengthVariation::updateLengths()
{
    QLineF tempLine = branchLine;
    tempLine.setLength(minLength);
    minPoint = tempLine.p2();
    tempLine.setLength(maxLength);
    maxPoint = tempLine.p2();
}

void LengthVariation::drawLines(QPainter* painter, QPointF panDiff)
{
    QPen pen = QPen();
    pen.setColor(Qt::red);
    pen.setWidth(2);
    painter->setBrush(QBrush(Qt::red, Qt::SolidPattern));
    painter->setPen(pen);

    painter->drawEllipse(getMaxPoint() + panDiff, 3, 3);

    painter->drawEllipse(getMinPoint() + panDiff, 3, 3);
}

void LengthVariation::modifyVariation(QPointF mousePoint, QString varType)
{
    float origLength, finalLength;
    if (varType.compare("lmin") == 0)
        origLength = findDistance(branchLine.p1(), minPoint);
    else
        origLength = findDistance(branchLine.p1(), maxPoint);
    QPointF closePoint = closestPointOnLine(branchLine.p1(), branchLine.p2(), mousePoint);
    if (varType.compare("lmin") == 0)
    {
        if (findDistance(closePoint, branchLine.p2()) >= findDistance(branchLine.p1(), branchLine.p2()) && findDistance(closePoint, branchLine.p1()) < findDistance(closePoint, branchLine.p2()))
        {
            QLineF tempLine = QLineF(branchLine.p1(), branchLine.p2());
            tempLine.setLength(30);
            minPoint = tempLine.p2();
        }
        else
            minPoint = closePoint;
    }
    else
    {
        if (findDistance(closePoint, branchLine.p2()) >= findDistance(branchLine.p1(), branchLine.p2()) && findDistance(closePoint, branchLine.p1()) < findDistance(closePoint, branchLine.p2()))
        {
            QLineF tempLine = QLineF(branchLine.p1(), branchLine.p2());
            tempLine.setLength(30);
            maxPoint = tempLine.p2();
        }
        else
            maxPoint = closePoint;
    }

    if (varType.compare("lmin") == 0)
        finalLength = findDistance(branchLine.p1(), minPoint);
    else
        finalLength = findDistance(branchLine.p1(), maxPoint);

    minLength = findDistance(branchLine.p1(), minPoint);
    maxLength = findDistance(branchLine.p1(), maxPoint);

    if (minLength < 30)
    {
        QLineF tempLine = QLineF(branchLine.p1(), minPoint);
        tempLine.setLength(30);
        minPoint = tempLine.p2();
        minLength = findDistance(branchLine.p1(), minPoint);
    }
    if (maxLength < 30)
    {
        QLineF tempLine = QLineF(branchLine.p1(), maxPoint);
        tempLine.setLength(30);
        maxPoint = tempLine.p2();
        maxLength = findDistance(branchLine.p1(), maxPoint);
    }
}

QPointF LengthVariation::closestPointOnLine(QPointF a, QPointF b, QPointF c)
{
    QPointF ac = c-a;
    QPointF ab = b-a;
    float ab2 = (ab.x() * ab.x()) + (ab.y() * ab.y());
    float ac_ab = (ac.x() * ab.x()) + (ac.y() * ab.y());
    float t = ac_ab / ab2;
    QPointF closest = a + (ab * t);
    return closest;
}

QPointF LengthVariation::calcVarExtremePoint(QLineF norm, QPointF point, float len)
{
    norm.setLength(len);
    QPointF newPoint = norm.p2();
    QPointF tempPoint = norm.p1() - point;
    newPoint = newPoint - tempPoint;
    return newPoint;
}

QPointF LengthVariation::findAdjacentPoint(QPointF mid, QPointF point)
{
    QPointF newPoint;
    newPoint.setX(midpointCalculation(mid.x(), point.x()));
    newPoint.setY(midpointCalculation(mid.y(), point.y()));
    return newPoint;
}

float LengthVariation::midpointCalculation(float mid, float point)
{
    float newFloat = 2 * mid - point;
    return newFloat;
}

QString LengthVariation::checkOnLine(QPointF mousePoint)
{
    QPointF temp1 = calcVarExtremePoint(normalLine, minPoint, lineWidth);
    QPointF temp2 = findAdjacentPoint(minPoint, temp1);
    QLineF minLine = QLineF(temp1, temp2);

    temp1 = calcVarExtremePoint(normalLine, maxPoint, lineWidth);
    temp2 = findAdjacentPoint(maxPoint, temp1);
    QLineF maxLine = QLineF(temp1, temp2);

    QPointF closePointMin = findPoint(minLine.p1(), minLine.p2(), mousePoint);
    float disToMin = findDistance(closePointMin, mousePoint);

    QPointF closePointMax = findPoint(maxLine.p1(), maxLine.p2(), mousePoint);
    float disToMax = findDistance(closePointMax, mousePoint);

    if (disToMin < 5.0f)
    {
        distToWidget = disToMin;
        return "lmin";
    }
    else if (disToMax < 5.0f)
    {
        distToWidget = disToMax;
        return "lmax";
    }
    else
        return "NULL";
}

float LengthVariation::getDistanceToWidget()
{
    return distToWidget;
}

QPointF LengthVariation::findPoint(QPointF a, QPointF b, QPointF c)
{
    QPointF ac = c-a;
    QPointF ab = b-a;
    float ab2 = (ab.x() * ab.x()) + (ab.y() * ab.y());
    float ac_ab = (ac.x() * ab.x()) + (ac.y() * ab.y());
    float t = ac_ab / ab2;
    if (t < 0.0f)
        t = 0.0f;
    else if (t > 1.0f)
        t = 1.0f;
    QPointF closest = a + (ab * t);
    return closest;
}

float LengthVariation::findDistance(QPointF a, QPointF b)
{
    int x = (a.x() - b.x()) * (a.x() - b.x());
    int y = (a.y() - b.y()) * (a.y() - b.y());
    float dist = sqrt(x + y);
    return dist;
}

void LengthVariation::setLengthVariation(float length, QString varType)
{
    QLineF tempLine = branchLine;
    if (length > 1)
        tempLine.setLength(length);
    else
        tempLine.setLength(1);

    if (varType.compare("lmin") == 0)
    {
        minPoint = tempLine.p2();
        minLength = findDistance(tempLine.p1(), minPoint);
    }
    else
    {
        maxPoint = tempLine.p2();
        maxLength = findDistance(tempLine.p1(), maxPoint);
    }
}

float LengthVariation::getLengthVariation(QString vt)
{
    float dist;
    if (vt.compare("lmin") == 0)
    {
        dist = findDistance(branchLine.p1(), minPoint);
    }
    else
    {
        dist = findDistance(branchLine.p1(), maxPoint);
    }
    return dist;
}

float LengthVariation::getMinLength()
{
    if (findDistance(branchLine.p1(), minPoint) < findDistance(branchLine.p1(), maxPoint))
        return findDistance(branchLine.p1(), minPoint);
    else
        return findDistance(branchLine.p1(), maxPoint);
}

float LengthVariation::getMaxLength()
{
    if (findDistance(branchLine.p1(), minPoint) > findDistance(branchLine.p1(), maxPoint))
        return findDistance(branchLine.p1(), minPoint);
    else
        return findDistance(branchLine.p1(), maxPoint);

}

QPointF LengthVariation::getMinPoint()
{
    if (findDistance(branchLine.p1(), minPoint) < findDistance(branchLine.p1(), maxPoint))
        return minPoint;
    else
        return maxPoint;
}

QPointF LengthVariation::getMaxPoint()
{
    if (findDistance(branchLine.p1(), minPoint) < findDistance(branchLine.p1(), maxPoint))
        return maxPoint;
    else
        return minPoint;
}
