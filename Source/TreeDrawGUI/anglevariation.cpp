#include "anglevariation.h"

AngleVariation::AngleVariation()
{
}

AngleVariation::AngleVariation(QLineF bl)
{
    branchLine = bl;
    origPoint = bl.p1();
    length = 40;
    variationAngle = 0.01;
    initialise();
}

void AngleVariation::initialise()
{
    // Calculate points positions
    QPointF tempPoint = QPointF();
    tempPoint.setX(branchLine.p2().x() - branchLine.p1().x());
    tempPoint.setY(branchLine.p2().y() - branchLine.p1().y());
    branchAngle = atan2(tempPoint.y(), tempPoint.x());
    origPoint = findPoint(branchAngle - variationAngle);
    variationPoint = calculateOpposingPoint(origPoint);
}

void AngleVariation::updatePoints(QLineF bl)
{
    QPointF diffVec = branchLine.p1() - origPoint;
    branchLine = bl;
    origPoint = branchLine.p1() - diffVec;
    variationPoint = calculateOpposingPoint(origPoint);
}

QPointF AngleVariation::findPoint(double angle)
{
    QPointF newPoint = QPointF();
    newPoint.setX(branchLine.p1().x() + (int)(length * cos(angle)));
    newPoint.setY(branchLine.p1().y() + (int)(length * sin(angle)));
    return newPoint;
}

QPointF AngleVariation::calculateOpposingPoint(QPointF point1)
{
    // Find closest point on line
    QPointF tempMidPoint = closestPointOnLine(branchLine.p1(), branchLine.p2(), point1);
    // Use closest point as midpoint to find opposing point
    QPointF adjacentPoint = findAdjacentPoint(tempMidPoint, point1);
    // Return point
    return adjacentPoint;
}

QPointF AngleVariation::closestPointOnLine(QPointF a, QPointF b, QPointF c)
{
    QPointF ac = c-a;
    QPointF ab = b-a;
    float ab2 = (ab.x() * ab.x()) + (ab.y() * ab.y());
    float ac_ab = (ac.x() * ab.x()) + (ac.y() * ab.y());
    float t = ac_ab / ab2;
    QPointF closest = a + (ab * t);
    return closest;
}

QPointF AngleVariation::closestPointOnLineSegment(QPointF a, QPointF b, QPointF c)
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

QPointF AngleVariation::findAdjacentPoint(QPointF mid, QPointF point)
{
    QPointF newPoint;
    newPoint.setX(midpointCalculation(mid.x(), point.x()));
    newPoint.setY(midpointCalculation(mid.y(), point.y()));
    return newPoint;
}

float AngleVariation::midpointCalculation(float mid, float point)
{
    float newFloat = 2 * mid - point;
    return newFloat;
}

void AngleVariation::drawLines(QPainter* painter, QPointF panDiff)
{
    QPen pen = QPen();
    pen.setWidth(2);
    pen.setColor(QColor(255, 0, 0, 255));
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor(255, 0, 0, 255), Qt::SolidPattern));

    painter->drawLine(branchLine.p1() + panDiff, origPoint + panDiff);
    painter->drawLine(branchLine.p1() + panDiff, variationPoint + panDiff);
}

QString AngleVariation::checkOnLineA(QPointF mousePoint)
{
    QPointF closestPointToMin = closestPointOnLineSegment(branchLine.p1(), origPoint, mousePoint);
    float distToMin = findDistance(mousePoint, closestPointToMin);

    QPointF closestPointToMax = closestPointOnLineSegment(branchLine.p1(), variationPoint, mousePoint);
    float distToMax = findDistance(mousePoint, closestPointToMax);

    if (distToMin < 5.0f)
    {
        distToWidget = distToMin;
        return "amin";
    }
    else if (distToMax < 5.0f)
    {
        distToWidget = distToMax;
        return "amax";
    }
    else
        return "NULL";
}

float AngleVariation::getDistanceToWidget()
{
    return distToWidget;
}

float AngleVariation::findDistance(QPointF a, QPointF b)
{
    int x = (a.x() - b.x()) * (a.x() - b.x());
    int y = (a.y() - b.y()) * (a.y() - b.y());
    float dist = sqrt(x + y);
    return dist;
}

void AngleVariation::modifyVariation(QPointF mousePoint)
{
    // Find closest point on circle to mouse
    QPointF vec = QPointF(mousePoint.x() - branchLine.p1().x(), mousePoint.y() - branchLine.p1().y());
    double magVec = sqrt(pow(vec.x(), 2) + pow(vec.y(), 2));
    QPointF closestPoint = QPointF();
    closestPoint.setX(branchLine.p1().x() + (vec.x() / magVec * length));
    closestPoint.setY(branchLine.p1().y() + (vec.y() / magVec * length));

    origPoint = closestPoint;
    variationPoint = calculateOpposingPoint(origPoint);
}

void AngleVariation::checkParentBranchIntersection(QPointF* parentBounds)
{
    QLineF sideLine1 = QLineF(parentBounds[0], parentBounds[3]);
    QLineF sideLine2 = QLineF(parentBounds[1], parentBounds[2]);

    float tempSide1 = findDistance(branchLine.p1(), closestPointOnLine(sideLine1.p1(), sideLine1.p2(), branchLine.p1()));
    float tempSide2 = findDistance(branchLine.p1(), closestPointOnLine(sideLine2.p1(), sideLine2.p2(), branchLine.p1()));

    QLineF chosenLine;
    if (tempSide1 < tempSide2)
        chosenLine = sideLine1;
    else
        chosenLine = sideLine2;

    QPointF point1 = calcVarExtremePoint(chosenLine, branchLine.p1(), length);
    QPointF point2 = findAdjacentPoint(branchLine.p1(), point1);

    QPointF control = calcVarExtremePoint(branchLine, branchLine.p1(), length);

    if (findDistance(origPoint, control) >= findDistance(point1, control))
    {
        origPoint = point1;
        variationPoint = calculateOpposingPoint(origPoint);
    }
    if (findDistance(origPoint, control) >= findDistance(point2, control))
    {
        origPoint = point2;
        variationPoint = calculateOpposingPoint(origPoint);
    }
}

QPointF AngleVariation::calcVarExtremePoint(QLineF norm, QPointF point, float len)
{
    norm.setLength(len);
    QPointF newPoint = norm.p2();
    QPointF tempPoint = norm.p1() - point;
    newPoint = newPoint - tempPoint;
    return newPoint;
}

QPointF AngleVariation::getAngleVariation(QString varType)
{
    if (varType.compare("amin") == 0)
        return origPoint;
    else
        return variationPoint;
}

void AngleVariation::setAngleVariation(QPointF point, QString varType)
{
    if (varType.compare("amin") == 0)
    {
        origPoint = point;
        variationPoint = calculateOpposingPoint(origPoint);
    }
    else
    {
        variationPoint = point;
        origPoint = calculateOpposingPoint(variationPoint);
    }
}

QLineF AngleVariation::getMinLine()
{
    return QLineF(branchLine.p1(), origPoint);
}

QLineF AngleVariation::getMaxLine()
{
    return QLineF(branchLine.p1(), variationPoint);
}
