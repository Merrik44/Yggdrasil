#include "segmentedbranch.h"

SegmentedBranch::SegmentedBranch()
{
}

void SegmentedBranch::addSegmentId(Branch* tr, long iden)
{
    setTree(tr);
    branchIdentifiers.append(iden);
}

void SegmentedBranch::setLengthBranchPercentages(Branch *tr)
{
    setTree(tr);
    lengthCalculateLengths();

    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
    {
        findBranch(tree, branchIdentifiers.at(i));
        float currBranchLength = neededBranch->getLengthVariationMax();
        lengthBranchPercentages.append(currBranchLength/lengthMax);
    }

    lengthCalculatePoints();
}

void SegmentedBranch::findBranch(Branch* tr, long iden)
{
    if (tr->getIdentifier() == iden)
        neededBranch = tr;
    else
        for (int i = 0 ; i < tr->numChildren() ; i++)
            findBranch(tr->childAt(i), iden);
}

void SegmentedBranch::initialiseWidthVariationPoints()
{
    findBranch(tree, branchIdentifiers.at(0));
    Branch* tempBranch = neededBranch;
    widthStartMinPoint = tempBranch->getWidthVariationPoint("smin");
    widthStartMaxPoint = tempBranch->getWidthVariationPoint("smax");
    widthStartMinLength = tempBranch->getWidthVariation("smin");
    widthStartMaxLength = tempBranch->getWidthVariation("smax");

    findBranch(tree, branchIdentifiers.last());
    tempBranch = neededBranch;
    widthEndMinPoint = tempBranch->getWidthVariationPoint("emin");
    widthEndMaxPoint = tempBranch->getWidthVariationPoint("emax");
    widthEndMinLength = tempBranch->getWidthVariation("emin");
    widthEndMaxLength = tempBranch->getWidthVariation("emax");
}

QPointF SegmentedBranch::findPoint(double angle, QLineF branchLine, float length)
{
    QPointF newPoint = QPointF();
    newPoint.setX(branchLine.p1().x() + (int)(length * cos(angle)));
    newPoint.setY(branchLine.p1().y() + (int)(length * sin(angle)));
    return newPoint;
}

QPointF SegmentedBranch::calculateOpposingPoint(QPointF point1, QLineF branchLine)
{
    // Find closest point on line
    QPointF tempMidPoint = closestPointOnLine(branchLine.p1(), branchLine.p2(), point1);
    // Use closest point as midpoint to find opposing point
    QPointF adjacentPoint = findAdjacentPoint(tempMidPoint, point1);
    // Return point
    return adjacentPoint;
}

QPointF SegmentedBranch::closestPointOnLine(QPointF a, QPointF b, QPointF c)
{
    QPointF ac = c-a;
    QPointF ab = b-a;
    float ab2 = (ab.x() * ab.x()) + (ab.y() * ab.y());
    float ac_ab = (ac.x() * ab.x()) + (ac.y() * ab.y());
    float t = ac_ab / ab2;
    QPointF closest = a + (ab * t);
    return closest;
}

QPointF SegmentedBranch::closestPointOnLineSegment(QPointF a, QPointF b, QPointF c)
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

QPointF SegmentedBranch::findAdjacentPoint(QPointF mid, QPointF point)
{
    QPointF newPoint;
    newPoint.setX(midpointCalculation(mid.x(), point.x()));
    newPoint.setY(midpointCalculation(mid.y(), point.y()));
    return newPoint;
}

float SegmentedBranch::midpointCalculation(float mid, float point)
{
    float newFloat = 2 * mid - point;
    return newFloat;
}

float SegmentedBranch::findDistance(QPointF a, QPointF b)
{
    int x = (a.x() - b.x()) * (a.x() - b.x());
    int y = (a.y() - b.y()) * (a.y() - b.y());
    float dist = sqrt(x + y);
    return dist;
}


QPointF SegmentedBranch::calcVarExtremePoint(QLineF norm, QPointF point, float len)
{
    norm.setLength(len);
    QPointF newPoint = norm.p2();
    QPointF tempPoint = norm.p1() - point;
    newPoint = newPoint - tempPoint;
    return newPoint;
}

bool SegmentedBranch::hasMember(long iden)
{
    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
        if (branchIdentifiers.at(i) == iden)
            return true;

    return false;
}

/*void SegmentedBranch::drawBranch(Branch *tr, QPainter *painter, bool isSelected, QPointF panDiff)
{
    QPen pen = QPen();
    pen.setWidth(2);
    float trans;
    if (isSelected)
        trans = 255;
    else
        trans = 95;

    setTree(tr);
    lengthCalculateLengths();
    lengthCalculatePoints();

    bool pastMinLength = false;
    float accumulator = 0.0f;

    QPointF bottomMaxRect1, bottomMaxRect2;
    QPointF bottomMinRect1, bottomMinRect2;

    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
    {
        findBranch(tree, branchIdentifiers.at(i));
        QPointF* branchPoints = neededBranch->getMaxRect();

        if (pastMinLength)
        {
            // Draw in light green
            pen.setColor(QColor(0, 130, 0, trans));
            painter->setPen(pen);
            painter->setBrush(QBrush(QColor(0, 130, 0, trans), Qt::SolidPattern));

            QPointF maxRect[4];
            if (i == 0)
            {
                maxRect[0] = branchPoints[0] + panDiff;
                maxRect[1] = branchPoints[1] + panDiff;
            }
            else
            {
                maxRect[0] = bottomMaxRect1;
                maxRect[1] = bottomMaxRect2;
            }
            maxRect[2] = branchPoints[2] + panDiff;
            maxRect[3] = branchPoints[3] + panDiff;

            painter->drawPolygon(maxRect, 4);

            bottomMaxRect1 = maxRect[3];
            bottomMaxRect2 = maxRect[2];
        }
        else
        {
            float currBranchLength = findDistance(neededBranch->getBranchLine().p1(), neededBranch->getLengthMaxPoint());

            QPointF midPoint1;
            midPoint1.setX((branchPoints[0].x() + branchPoints[1].x())/2);
            midPoint1.setY((branchPoints[0].y() + branchPoints[1].y())/2);
            QPointF midPoint2;
            midPoint2.setX((branchPoints[2].x() + branchPoints[3].x())/2);
            midPoint2.setY((branchPoints[2].y() + branchPoints[3].y())/2);

            // Draw light green
            pen.setColor(QColor(0, 130, 0, trans));
            painter->setPen(pen);
            painter->setBrush(QBrush(QColor(0, 130, 0, trans), Qt::SolidPattern));

            QPointF maxRect[4];
            if (i == 0)
            {
                maxRect[0] = branchPoints[0] + panDiff;
                maxRect[1] = branchPoints[1] + panDiff;
            }
            else
            {
                maxRect[0] = bottomMaxRect1;
                maxRect[1] = bottomMaxRect2;
            }
            maxRect[2] = branchPoints[2] + panDiff;
            maxRect[3] = branchPoints[3] + panDiff;

            painter->drawPolygon(maxRect, 4);

            bottomMaxRect1 = maxRect[3];
            bottomMaxRect2 = maxRect[2];

            QPointF minPoints[4];
            QLineF tempLine = QLineF(midPoint1, branchPoints[0]);

            if (i == 0)
            {
                tempLine.setLength(neededBranch->getThicknessVariationStartMin());
                minPoints[0] = tempLine.p2() + panDiff;
                tempLine = QLineF(midPoint1, branchPoints[1]);
                tempLine.setLength(neededBranch->getThicknessVariationStartMin());
                minPoints[1] = tempLine.p2() + panDiff;
            }
            else
            {
                minPoints[0] = bottomMinRect1;
                minPoints[1] = bottomMinRect2;
            }

            // Segment with minimum running all the way through
            if (currBranchLength + accumulator < lengthMin)
            {
                tempLine = QLineF(midPoint2, branchPoints[2]);
                tempLine.setLength(neededBranch->getThicknessVariationEndMin());
                minPoints[2] = tempLine.p2() + panDiff;
                tempLine = QLineF(midPoint2, branchPoints[3]);
                tempLine.setLength(neededBranch->getThicknessVariationEndMin());
                minPoints[3] = tempLine.p2() + panDiff;

                pen.setColor(QColor(0, 75, 0, trans));
                painter->setPen(pen);
                painter->setBrush(QBrush(QColor(0, 75, 0, trans), Qt::SolidPattern));
                painter->drawPolygon(minPoints, 4);
            }
            else // Segment with minimum finishing in the middle
            {
                QPointF diff = lengthMinPoint - midPoint1;

                QLineF tempLine = QLineF(midPoint1, branchPoints[1]);
                tempLine.setLength(neededBranch->getThicknessVariationEndMin());
                tempLine.translate(diff);
                minPoints[2] = tempLine.p2() + panDiff;

                tempLine = QLineF(midPoint1, branchPoints[0]);
                tempLine.setLength(neededBranch->getThicknessVariationEndMin());
                tempLine.translate(diff);
                minPoints[3] = tempLine.p2() + panDiff;

                pen.setColor(QColor(0, 75, 0, trans));
                painter->setPen(pen);
                painter->setBrush(QBrush(QColor(0, 75, 0, trans), Qt::SolidPattern));
                painter->drawPolygon(minPoints, 4);
                pastMinLength = true;
            }

            bottomMinRect1 = minPoints[3];
            bottomMinRect2 = minPoints[2];
            accumulator += currBranchLength;
        }
    }
}*/

/* Original*/
/*void SegmentedBranch::drawBranch(Branch *tr, QPainter *painter, bool isSelected, QPointF panDiff)
{
    QPen pen = QPen();
    pen.setWidth(2);
    float trans;
    if (isSelected)
        trans = 255;
    else
        trans = 95;

    setTree(tr);
    lengthCalculateLengths();
    lengthCalculatePoints();

    bool pastMinLength = false;
    float accumulator = 0.0f;

    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
    {
        findBranch(tree, branchIdentifiers.at(i));
        QPointF* branchPoints = neededBranch->getMaxRect();

        if (pastMinLength)
        {
            // Draw in light green
            pen.setColor(QColor(0, 130, 0, trans));
            painter->setPen(pen);
            painter->setBrush(QBrush(QColor(0, 130, 0, trans), Qt::SolidPattern));

            QPointF tempPoints[4];
            tempPoints[0] = branchPoints[0] + panDiff;
            tempPoints[1] = branchPoints[1] + panDiff;
            tempPoints[2] = branchPoints[2] + panDiff;
            tempPoints[3] = branchPoints[3] + panDiff;

            painter->drawPolygon(tempPoints, 4);
        }
        else
        {
            float currBranchLength = findDistance(neededBranch->getBranchLine().p1(), neededBranch->getLengthMaxPoint());

            QPointF midPoint1;
            midPoint1.setX((branchPoints[0].x() + branchPoints[1].x())/2);
            midPoint1.setY((branchPoints[0].y() + branchPoints[1].y())/2);
            QPointF midPoint2;
            midPoint2.setX((branchPoints[2].x() + branchPoints[3].x())/2);
            midPoint2.setY((branchPoints[2].y() + branchPoints[3].y())/2);

            // Draw dark green
            pen.setColor(QColor(0, 130, 0, trans));
            painter->setPen(pen);
            painter->setBrush(QBrush(QColor(0, 130, 0, trans), Qt::SolidPattern));

            QPointF tempPoints[4];
            tempPoints[0] = branchPoints[0] + panDiff;
            tempPoints[1] = branchPoints[1] + panDiff;
            tempPoints[2] = branchPoints[2] + panDiff;
            tempPoints[3] = branchPoints[3] + panDiff;

            painter->drawPolygon(tempPoints, 4);

            QPointF minPoints[4];

            QLineF tempLine = QLineF(midPoint1, branchPoints[0]);
            tempLine.setLength(neededBranch->getThicknessVariationStartMin());
            minPoints[0] = tempLine.p2() + panDiff;
            tempLine = QLineF(midPoint1, branchPoints[1]);
            tempLine.setLength(neededBranch->getThicknessVariationStartMin());
            minPoints[1] = tempLine.p2() + panDiff;

            if (currBranchLength + accumulator < lengthMin)
            {
                tempLine = QLineF(midPoint2, branchPoints[2]);
                tempLine.setLength(neededBranch->getThicknessVariationEndMin());
                minPoints[2] = tempLine.p2() + panDiff;
                tempLine = QLineF(midPoint2, branchPoints[3]);
                tempLine.setLength(neededBranch->getThicknessVariationEndMin());
                minPoints[3] = tempLine.p2() + panDiff;

                pen.setColor(QColor(0, 75, 0, trans));
                painter->setPen(pen);
                painter->setBrush(QBrush(QColor(0, 75, 0, trans), Qt::SolidPattern));
                painter->drawPolygon(minPoints, 4);
            }
            else
            {
                QPointF diff = lengthMinPoint - midPoint1;

                QLineF tempLine = QLineF(midPoint1, branchPoints[1]);
                tempLine.setLength(neededBranch->getThicknessVariationEndMin());
                tempLine.translate(diff);
                minPoints[2] = tempLine.p2() + panDiff;

                tempLine = QLineF(midPoint1, branchPoints[0]);
                tempLine.setLength(neededBranch->getThicknessVariationEndMin());
                tempLine.translate(diff);
                minPoints[3] = tempLine.p2() + panDiff;

                pen.setColor(QColor(0, 75, 0, trans));
                painter->setPen(pen);
                painter->setBrush(QBrush(QColor(0, 75, 0, trans), Qt::SolidPattern));
                painter->drawPolygon(minPoints, 4);
                pastMinLength = true;
            }
            accumulator += currBranchLength;
        }
    }
}*/

void SegmentedBranch::drawBranch(Branch *tr, QPainter *painter, bool isSelected, QPointF panDiff)
{
    QPen pen = QPen();
    pen.setWidth(2);
    float trans;
    if (isSelected)
        trans = 255;
    else
        trans = 95;

    setTree(tr);
    lengthCalculateLengths();
    lengthCalculatePoints();

    float smallestLength;
    if (lengthMin < lengthMax)
        smallestLength = lengthMin;
    else
        smallestLength = lengthMax;

    bool pastMinLength = false;
    float accumulator = 0.0f;

    QPointF bottomMaxRect1, bottomMaxRect2;
    QPointF bottomMinRect1, bottomMinRect2;

    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
    {
        findBranch(tree, branchIdentifiers.at(i));
        QPointF* branchPoints = neededBranch->getMaxRect();

        // Draw light green
        pen.setColor(QColor(0, 130, 0, trans));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(0, 130, 0, trans), Qt::SolidPattern));

        QPointF maxRect[4];
        maxRect[0] = branchPoints[0] + panDiff;
        maxRect[1] = branchPoints[1] + panDiff;
        maxRect[2] = branchPoints[2] + panDiff;
        maxRect[3] = branchPoints[3] + panDiff;
        painter->drawPolygon(maxRect, 4);

        if(i != 0)
        {
            QPointF maxFillerRect[4];
            maxFillerRect[0] = bottomMaxRect1;
            maxFillerRect[1] = bottomMaxRect2;
            maxFillerRect[2] = maxRect[1];
            maxFillerRect[3] = maxRect[0];
            painter->drawPolygon(maxFillerRect, 4);
        }

        bottomMaxRect1 = maxRect[3];
        bottomMaxRect2 = maxRect[2];

        if (!pastMinLength)
        {
            float currBranchLength = findDistance(neededBranch->getBranchLine().p1(), neededBranch->getLengthMaxPoint());

            QPointF midPoint1;
            midPoint1.setX((branchPoints[0].x() + branchPoints[1].x())/2);
            midPoint1.setY((branchPoints[0].y() + branchPoints[1].y())/2);
            QPointF midPoint2;
            midPoint2.setX((branchPoints[2].x() + branchPoints[3].x())/2);
            midPoint2.setY((branchPoints[2].y() + branchPoints[3].y())/2);

            QPointF minRect[4];

            QLineF tempLine = QLineF(midPoint1, branchPoints[0]);
            tempLine.setLength(neededBranch->getThicknessVariationStartMin());
            minRect[0] = tempLine.p2() + panDiff;
            tempLine = QLineF(midPoint1, branchPoints[1]);
            tempLine.setLength(neededBranch->getThicknessVariationStartMin());
            minRect[1] = tempLine.p2() + panDiff;

            if (currBranchLength + accumulator < smallestLength)
            {
                tempLine = QLineF(midPoint2, branchPoints[2]);
                tempLine.setLength(neededBranch->getThicknessVariationEndMin());
                minRect[2] = tempLine.p2() + panDiff;
                tempLine = QLineF(midPoint2, branchPoints[3]);
                tempLine.setLength(neededBranch->getThicknessVariationEndMin());
                minRect[3] = tempLine.p2() + panDiff;
            }
            else
            {
                QPointF diff;
                if (lengthMin < lengthMax)
                    diff = lengthMinPoint - midPoint1;
                else
                    diff = lengthMaxPoint - midPoint1;

                QLineF tempLine = QLineF(midPoint1, branchPoints[1]);
                tempLine.setLength(neededBranch->getThicknessVariationEndMin());
                tempLine.translate(diff);
                minRect[2] = tempLine.p2() + panDiff;

                tempLine = QLineF(midPoint1, branchPoints[0]);
                tempLine.setLength(neededBranch->getThicknessVariationEndMin());
                tempLine.translate(diff);
                minRect[3] = tempLine.p2() + panDiff;

                pastMinLength = true;
            }

            // Draw rectangle
            pen.setColor(QColor(0, 75, 0, trans));
            painter->setPen(pen);
            painter->setBrush(QBrush(QColor(0, 75, 0, trans), Qt::SolidPattern));
            painter->drawPolygon(minRect, 4);

            // Draw Filler rectangle
            if (i != 0)
            {
                QPointF minFillerRect[4];
                minFillerRect[0] = bottomMinRect1;
                minFillerRect[1] = bottomMinRect2;
                minFillerRect[2] = minRect[1];
                minFillerRect[3] = minRect[0];
                painter->drawPolygon(minFillerRect, 4);
            }

            bottomMinRect1 = minRect[3];
            bottomMinRect2 = minRect[2];

            accumulator += currBranchLength;
        }
    }
}

QPointF SegmentedBranch::calculateLineIntersection(QLineF line1, QLineF line2)
{
    float d = (line1.p1().x() - line1.p2().x()) * (line2.p1().y() - line2.p2().y()) - (line1.p1().y() - line1.p2().y()) * (line2.p1().x() - line2.p2().x());

    float pre = (line1.p1().x() * line1.p2().y() - line1.p1().y() * line1.p2().x());
    float post = (line2.p1().x() * line2.p2().y() - line2.p1().y() * line2.p2().x());
    QPointF intersectionPoint;
    intersectionPoint.setX((pre * (line2.p1().x() - line2.p2().x()) - post * (line1.p1().x() - line1.p2().x())) / d);
    intersectionPoint.setY((pre * (line2.p1().y() - line2.p2().y()) - post * (line1.p1().y() - line1.p2().y())) / d);

    return intersectionPoint;
}

void SegmentedBranch::drawVariation(Branch* tr, QPainter *painter, QPointF panDiff)
{
    setTree(tr);
    drawAngleVariation(painter, panDiff);
    drawLengthVariation(painter, panDiff);
    drawWidthVariation(painter, panDiff);
}

void SegmentedBranch::drawAngleVariation(QPainter *painter, QPointF panDiff)
{
    findBranch(tree, branchIdentifiers.at(0));
    neededBranch->drawAngleVariation(painter, panDiff);
}

void SegmentedBranch::drawLengthVariation(QPainter *painter, QPointF panDiff)
{
    lengthCalculateLengths();
    lengthCalculatePoints();

    painter->setPen(QPen(Qt::red));

    painter->drawEllipse(lengthMinPoint + panDiff, 3, 3);
    painter->drawEllipse(lengthMaxPoint + panDiff, 3, 3);
}

void SegmentedBranch::drawWidthVariation(QPainter *painter, QPointF panDiff)
{
    QPen pen = QPen();
    pen.setWidthF(2.5);
    painter->setPen(pen);

    initialiseWidthVariationPoints();

    findBranch(tree, branchIdentifiers.at(0));
    QLineF branchNormal = QLineF(neededBranch->getBranchLine().p1(), neededBranch->getLengthMaxPoint()).normalVector();

    QPointF temp = calcVarExtremePoint(branchNormal, widthStartMinPoint, widthStartMinLength);
    QPointF temp2 = findAdjacentPoint(widthStartMinPoint, temp);
    drawBar(painter, temp, temp2, panDiff);
    temp = calcVarExtremePoint(branchNormal, widthStartMaxPoint, widthStartMaxLength);
    temp2 = findAdjacentPoint(widthStartMaxPoint, temp);
    drawBar(painter, temp, temp2, panDiff);

    findBranch(tree, branchIdentifiers.last());
    branchNormal = QLineF(neededBranch->getBranchLine().p1(), neededBranch->getLengthMaxPoint()).normalVector();

    temp = calcVarExtremePoint(branchNormal, widthEndMinPoint, widthEndMinLength);
    temp2 = findAdjacentPoint(widthEndMinPoint, temp);
    drawBar(painter, temp, temp2, panDiff);
    temp = calcVarExtremePoint(branchNormal, widthEndMaxPoint, widthEndMaxLength);
    temp2 = findAdjacentPoint(widthEndMaxPoint, temp);
    drawBar(painter, temp, temp2, panDiff);
}

void SegmentedBranch::drawBar(QPainter* painter, QPointF point1, QPointF point2, QPointF panDiff)
{
    painter->drawLine(point1 + panDiff, point2 + panDiff);
    painter->drawEllipse(point1 + panDiff, 1.5, 1.5);
    painter->drawEllipse(point2 + panDiff, 1.5, 1.5);
}

QString SegmentedBranch::angleCheckOnLine(Branch* tr, QPointF mousePoint)
{
    setTree(tr);

    findBranch(tree, branchIdentifiers.at(0));
    if (neededBranch->checkOnLineA(mousePoint).compare("NULL") != 0)
    {
        distToAngleWidget = neededBranch->getDistanceToAngleWidget();
    }
    return neededBranch->checkOnLineA(mousePoint);
}

QString SegmentedBranch::widthCheckOnPoint(Branch* tr, QPointF mousePoint)
{
    setTree(tr);
    initialiseWidthVariationPoints();

    findBranch(tree, branchIdentifiers.at(0));
    QLineF branchNormal = QLineF(neededBranch->getBranchLine().p1(), neededBranch->getLengthMaxPoint()).normalVector();

    QPointF temp1 = calcVarExtremePoint(branchNormal, widthStartMinPoint, widthStartMinLength);
    QPointF temp2 = findAdjacentPoint(widthStartMinPoint, temp1);
    float disPoint1 = findDistance(temp1, mousePoint);
    float disPoint2 = findDistance(temp2, mousePoint);
    if (disPoint1 < 5.0f || disPoint2 < 5.0f)
    {
        if (disPoint1 < disPoint2)
            distToThicknessWidget = disPoint1;
        else
            distToThicknessWidget = disPoint2;
        return "smin";
    }
    temp1 = calcVarExtremePoint(branchNormal, widthStartMaxPoint, widthStartMaxLength);
    temp2 = findAdjacentPoint(widthStartMaxPoint, temp1);
    disPoint1 = findDistance(temp1, mousePoint);
    disPoint2 = findDistance(temp2, mousePoint);
    if (disPoint1 < 5.0f || disPoint2 < 5.0f)
    {
        if (disPoint1 < disPoint2)
            distToThicknessWidget = disPoint1;
        else
            distToThicknessWidget = disPoint2;
        return "smax";
    }

    findBranch(tree, branchIdentifiers.last());
    branchNormal = QLineF(neededBranch->getBranchLine().p1(), neededBranch->getLengthMaxPoint()).normalVector();

    temp1 = calcVarExtremePoint(branchNormal, widthEndMinPoint, widthEndMinLength);
    temp2 = findAdjacentPoint(widthEndMinPoint, temp1);
    disPoint1 = findDistance(temp1, mousePoint);
    disPoint2 = findDistance(temp2, mousePoint);
    if (disPoint1 < 5.0f || disPoint2 < 5.0f)
    {
        if (disPoint1 < disPoint2)
            distToThicknessWidget = disPoint1;
        else
            distToThicknessWidget = disPoint2;
        return "emin";
    }
    temp1 = calcVarExtremePoint(branchNormal, widthEndMaxPoint, widthEndMaxLength);
    temp2 = findAdjacentPoint(widthEndMaxPoint, temp1);
    disPoint1 = findDistance(temp1, mousePoint);
    disPoint2 = findDistance(temp2, mousePoint);
    if (disPoint1 < 5.0f || disPoint2 < 5.0f)
    {
        if (disPoint1 < disPoint2)
            distToThicknessWidget = disPoint1;
        else
            distToThicknessWidget = disPoint2;
        return "emax";
    }
    return "NULL";
}

QString SegmentedBranch::lengthCheckOnPoint(Branch* tr, QPointF mousePoint)
{
    setTree(tr);
    lengthCalculateLengths();
    lengthCalculatePoints();

    if (findDistance(lengthMinPoint, mousePoint) < 5.0f)
    {
        distToLengthWidget = findDistance(lengthMinPoint, mousePoint);
        return "lmin";
    }
    else if (findDistance(lengthMaxPoint, mousePoint) < 5.0f)
    {
        distToLengthWidget = findDistance(lengthMaxPoint, mousePoint);
        return "lmax";
    }
    else
        return "NULL";
}

float SegmentedBranch::getDistanceToAngleWidget()
{
    return distToAngleWidget;
}

float SegmentedBranch::getDistanceToLengthWidget()
{
    return distToLengthWidget;
}

float SegmentedBranch::getDistanceToThicknessWidget()
{
    return distToThicknessWidget;
}

void SegmentedBranch::modifyAngleVariation(Branch* tr, QPointF mousePoint)
{
    setTree(tr);

    findBranch(tree, branchIdentifiers.at(0));

    neededBranch->modifyAngleVariation(mousePoint);
}

void SegmentedBranch::modifyWidthVariation(Branch* tr, QPointF mousePoint, QPointF prevPoint, QString varType)
{
    //setTree(tr);
    tree = tr;

    initialiseWidthVariationPoints();

    findBranch(tree, branchIdentifiers.at(0));
    QLineF branchNormal = QLineF(neededBranch->getBranchLine().p1(), neededBranch->getLengthMaxPoint()).normalVector();
    findBranch(tree, branchIdentifiers.last());
    QLineF branchNormal2 = QLineF(neededBranch->getBranchLine().p1(), neededBranch->getLengthMaxPoint()).normalVector();

    QPointF temp, temp2;
    if (varType.compare("smin") == 0)
    {
        temp = calcVarExtremePoint(branchNormal, widthStartMinPoint, widthStartMinLength);
        temp2 = findAdjacentPoint(widthStartMinPoint, temp);
    }
    else if (varType.compare("smax") == 0)
    {
        temp = calcVarExtremePoint(branchNormal, widthStartMaxPoint, widthStartMaxLength);
        temp2 = findAdjacentPoint(widthStartMaxPoint, temp);
    }
    else if (varType.compare("emin") == 0)
    {
        temp = calcVarExtremePoint(branchNormal2, widthEndMinPoint, widthEndMinLength);
        temp2 = findAdjacentPoint(widthEndMinPoint, temp);
    }
    else
    {
        temp = calcVarExtremePoint(branchNormal2, widthEndMaxPoint, widthEndMaxLength);
        temp2 = findAdjacentPoint(widthEndMaxPoint, temp);
    }

    QPointF point1 = closestPointOnLine(temp, temp2, mousePoint);
    QPointF point2 = closestPointOnLine(temp, temp2, prevPoint);

    float newDist;

    float disPoint1, disPoint2;
    if (varType.compare("smin") == 0)
    {
        disPoint1 = findDistance(point1, widthStartMinPoint);
        disPoint2 = findDistance(point2, widthStartMinPoint);
        newDist = disPoint1 - disPoint2;
        widthStartMinLength = widthStartMinLength + newDist;
        if (widthStartMinLength < 1)
            widthStartMinLength = 1;
        checkMaxEndWidthLimits();
    }
    if (varType.compare("smax") == 0)
    {
        disPoint1 = findDistance(point1, widthStartMaxPoint);
        disPoint2 = findDistance(point2, widthStartMaxPoint);
        newDist = disPoint1 - disPoint2;
        widthStartMaxLength = widthStartMaxLength + newDist;
        if (widthStartMaxLength < 1)
            widthStartMaxLength = 1;
        checkMaxEndWidthLimits();
    }
    if (varType.compare("emin") == 0)
    {
        disPoint1 = findDistance(point1, widthEndMinPoint);
        disPoint2 = findDistance(point2, widthEndMinPoint);
        newDist = disPoint1 - disPoint2;
        widthEndMinLength = widthEndMinLength + newDist;
        if (widthEndMinLength < 1)
            widthEndMinLength = 1;
        checkMaxStartWidthLimits();
    }
    if (varType.compare("emax") == 0)
    {
        disPoint1 = findDistance(point1, widthEndMaxPoint);
        disPoint2 = findDistance(point2, widthEndMaxPoint);
        newDist = disPoint1 - disPoint2;
        widthEndMaxLength = widthEndMaxLength + newDist;
        if (widthEndMaxLength < 1)
            widthEndMaxLength = 1;
        checkMaxStartWidthLimits();
    }

    resizeBranchWidths();
}

void SegmentedBranch::modifyLengthVariation(Branch *tr, QPointF mousePoint, QString vt)
{
    setTree(tr);
    lengthCalculateLengths();
    lengthCalculatePoints();

    lengthClosestDistance = INT_MAX;

    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
    {
        findBranch(tree, branchIdentifiers.at(i));
        QPointF tempClosestPoint = closestPointOnLineSegment(neededBranch->getBranchLine().p1(), neededBranch->getLengthMaxPoint(), mousePoint);
        float tempDist = findDistance(tempClosestPoint, mousePoint);
        if (tempDist < lengthClosestDistance)
        {
            lengthClosestBranchIdentifier = neededBranch->getIdentifier();
            lengthClosestDistance = tempDist;
            lengthClosestPointOnLine = tempClosestPoint;
        }
    }

    if (findDistance(lengthClosestPointOnLine, neededBranch->getLengthMaxPoint()) == 0)
    {
        QPointF tempClosestPoint = closestPointOnLine(neededBranch->getBranchLine().p1(), neededBranch->getLengthMaxPoint(), mousePoint);
        float tempDist = findDistance(tempClosestPoint, mousePoint);
        lengthClosestDistance = tempDist;
        lengthClosestPointOnLine = tempClosestPoint;
    }

    float accumulator = 0.0f;
    bool completed = false;

    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
    {
        if (!completed)
        {
            findBranch(tree, branchIdentifiers.at(i));
            if (branchIdentifiers.at(i) != lengthClosestBranchIdentifier)
            {
                accumulator += neededBranch->getLengthVariationMax();
            }
            else
            {
                accumulator += findDistance(neededBranch->getBranchLine().p1(), lengthClosestPointOnLine);
                completed = true;
            }
        }
    }

    if (accumulator < 30)
        accumulator = 30;

    if (vt.compare("lmin") == 0)
        resizeBranchLengths(accumulator, lengthMax, vt);
    else
        resizeBranchLengths(lengthMin, accumulator, vt);
}

void SegmentedBranch::setWidthVariation(Branch* tr, float newLength, QString varType)
{
    setTree(tr);
    initialiseWidthVariationPoints();

    if (varType.compare("smin") == 0)
    {
        widthStartMinLength = newLength;
        checkMaxEndWidthLimits();
    }
    else if (varType.compare("smax") == 0)
    {
        widthStartMaxLength = newLength;
        checkMaxEndWidthLimits();
    }
    else if (varType.compare("emin") == 0)
    {
        widthEndMinLength = newLength;
        checkMaxStartWidthLimits();
    }
    else
    {
        widthEndMaxLength = newLength;
        checkMaxStartWidthLimits();
    }

    resizeBranchWidths();
}

float SegmentedBranch::getWidthVariation(Branch* tr, QString varType)
{
    setTree(tr);

    initialiseWidthVariationPoints();

    if (varType.compare("smin") == 0)
        return widthStartMinLength;
    else if (varType.compare("smax") == 0)
        return widthStartMaxLength;
    else if (varType.compare("emin") == 0)
        return widthEndMinLength;
    else
        return widthEndMaxLength;
}

void SegmentedBranch::setLengthVariation(Branch *tr, float newLength, QString varType)
{
    setTree(tr);

    lengthCalculateLengths();

    if (varType.compare("lmin") == 0)
        resizeBranchLengths(newLength, lengthMax, varType);
    else
        resizeBranchLengths(lengthMin, newLength, varType);
}

float SegmentedBranch::getLengthVariation(Branch *tr, QString varType)
{
    setTree(tr);

    lengthCalculateLengths();

    if (varType.compare("lmin") == 0)
        return lengthMin;
    else
        return lengthMax;
}

void SegmentedBranch::resizeBranchWidths()
{
    float tempMin = widthStartMinLength;
    float tempMax = widthStartMaxLength;
    float totalLength = calculateBranchLength("lmax");
    float minDiff = widthStartMinLength - widthEndMinLength;
    float maxDiff = widthStartMaxLength - widthEndMaxLength;
    if (minDiff < 0)
        minDiff *= -1;
    if (maxDiff < 0)
        maxDiff *= -1;

    float accumulator = 0;
    float posAlongBranch;
    float tempMinThickness;
    float tempMaxThickness;

    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
    {
        setWidthValues(tree, branchIdentifiers.at(i), tempMin, tempMax, 0, 0);
        findBranch(tree, branchIdentifiers.at(i));
        accumulator += findDistance(neededBranch->getStartPos(), neededBranch->getLengthMaxPoint());
        posAlongBranch = accumulator / totalLength;
        tempMinThickness = minDiff * posAlongBranch;
        tempMaxThickness = maxDiff * posAlongBranch;
        if (widthStartMinLength > widthEndMinLength)
            tempMin = widthStartMinLength - tempMinThickness;
        else
            tempMin = widthStartMinLength + tempMinThickness;

        if (widthStartMaxLength > widthEndMaxLength)
            tempMax = widthStartMaxLength - tempMaxThickness;
        else
            tempMax = widthStartMaxLength + tempMaxThickness;
        setWidthValues(tree, branchIdentifiers.at(i), 0, 0, tempMin, tempMax);
    }
}

/*void SegmentedBranch::resizeBranchLengths(float newMinLength, float newMaxLength, QString vt)
{
    lengthCalculateLengths();

    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
    {
        findBranch(tree, branchIdentifiers.at(i));
        if (vt.compare("lmin") == 0)
        {
            float tempCurrBranchLength = neededBranch->getLengthVariationMax();
            float percentageMin = newMinLength/lengthMax;
            float newLengthMin = percentageMin * tempCurrBranchLength;
            if (newLengthMin < 3)
                newLengthMin = 3;
            neededBranch->setLengthVariation(newLengthMin, "lmin");
        }
        else
        {
            float tempCurrBranchLengthMax = neededBranch->getLengthVariationMax();
            float percentageMax = tempCurrBranchLengthMax/lengthMax;
            float newLengthMax = percentageMax * newMaxLength;
            neededBranch->setLengthVariation(newLengthMax, "lmax");
        }
    }
    lengthCalculateLengths();
    lengthCalculatePoints();
}*/

void SegmentedBranch::resizeBranchLengths(float newMinLength, float newMaxLength, QString vt)
{
    lengthCalculateLengths();

    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
    {
        findBranch(tree, branchIdentifiers.at(i));
        if (vt.compare("lmin") == 0)
            neededBranch->setLengthVariation(newMinLength * lengthBranchPercentages.at(i), "lmin");
        else
            neededBranch->setLengthVariation(newMaxLength * lengthBranchPercentages.at(i), "lmax");
    }

    lengthCalculateLengths();
    lengthCalculatePoints();
}

void SegmentedBranch::setWidthValues(Branch* tr, long iden, float startMin, float startMax, float endMin, float endMax)
{
    if (tr->getIdentifier() == iden)
    {
        if (startMin != 0)
            tr->setCurveWidthVariation(startMin, "smin");
        if (startMax != 0)
            tr->setCurveWidthVariation(startMax, "smax");
        if (endMin != 0)
            tr->setCurveWidthVariation(endMin, "emin");
        if (endMax != 0)
            tr->setCurveWidthVariation(endMax, "emax");
    }
    else
        for (int i = 0 ; i < tr->numChildren() ; i++)
            setWidthValues(tr->childAt(i), iden, startMin, startMax, endMin, endMax);
}

float SegmentedBranch::calculateBranchLength(QString varType)
{
    float length = 0;
    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
    {
        findBranch(tree, branchIdentifiers.at(i));
        if (varType.compare("lmax") == 0)
            length += findDistance(neededBranch->getStartPos(), neededBranch->getLengthMaxPoint());
        else
            length += findDistance(neededBranch->getStartPos(), neededBranch->getLengthMinPoint());
    }
    return length;
}

QVector<long> SegmentedBranch::getAllIdentifiers()
{
    return branchIdentifiers;
}

void SegmentedBranch::setTree(Branch *tr)
{
    Branch* tempTree = tr;
    tree = tempTree;
}

Branch* SegmentedBranch::getTree()
{
    Branch* tempTree = tree;
    return tempTree;
}

void SegmentedBranch::lengthCalculateLengths()
{
    float minLength = 0;
    float maxLength = 0;
    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
    {
        findBranch(tree, branchIdentifiers.at(i));

        /*minLength += neededBranch->getLengthVariationMin();
        maxLength += neededBranch->getLengthVariationMax();*/

        minLength += neededBranch->getLengthVariation("lmin");
        maxLength += neededBranch->getLengthVariation("lmax");
    }

    lengthMin = minLength;
    lengthMax = maxLength;
}

void SegmentedBranch::lengthCalculatePoints()
{
    float accumulatorMin = 0;
    float accumulatorMax = 0;
    bool foundMin = false;
    bool foundMax = false;

    for (int i = 0 ; i < branchIdentifiers.size() ; i++)
    {
        findBranch(tree, branchIdentifiers.at(i));
        if (!foundMin)
        {            
            if (accumulatorMin + neededBranch->getLengthVariationMax() < lengthMin)
            {
                accumulatorMin += neededBranch->getLengthVariationMax();
            }
            else
            {
                QLineF tempLine = QLineF(neededBranch->getBranchLine().p1(), neededBranch->getLengthMaxPoint());
                tempLine.setLength(lengthMin - accumulatorMin);
                lengthMinPoint = tempLine.p2();
                foundMin = true;
            }
        }
        if (!foundMax)
        {
            if (accumulatorMax + neededBranch->getLengthVariationMax() < lengthMax)
            {
                accumulatorMax += neededBranch->getLengthVariationMax();
            }
            else
            {
                QLineF tempLine = QLineF(neededBranch->getBranchLine().p1(), neededBranch->getLengthMaxPoint());
                tempLine.setLength(lengthMax - accumulatorMax);
                lengthMaxPoint = tempLine.p2();
                foundMax = true;
            }
        }
    }
}

void SegmentedBranch::checkMaxEndWidthLimits()
{
    if (widthEndMinLength >= widthEndMaxLength)
    {
        if (widthStartMinLength < widthStartMaxLength)
        {
            if (widthEndMinLength > widthStartMaxLength)
                widthEndMinLength = widthStartMaxLength;
        }
        else
            if (widthEndMinLength > widthStartMinLength)
                widthEndMinLength = widthStartMinLength;
    }
    if (widthEndMaxLength >= widthEndMinLength)
    {
        if (widthStartMinLength < widthStartMaxLength)
        {
            if (widthEndMaxLength > widthStartMaxLength)
                widthEndMaxLength = widthStartMaxLength;
        }
        else
            if (widthEndMaxLength > widthStartMinLength)
                widthEndMaxLength = widthStartMinLength;
    }
}

void SegmentedBranch::checkMaxStartWidthLimits()
{
    if (widthEndMinLength >= widthEndMaxLength)
    {
        if (widthStartMinLength < widthStartMaxLength)
        {
            if (widthEndMinLength > widthStartMaxLength)
                widthStartMaxLength = widthEndMinLength;
        }
        else
            if (widthEndMinLength > widthStartMinLength)
                widthStartMinLength = widthEndMinLength;
    }
    if (widthEndMaxLength >= widthEndMinLength)
    {
        if (widthStartMinLength < widthStartMaxLength)
        {
            if (widthEndMaxLength > widthStartMaxLength)
                widthStartMaxLength = widthEndMaxLength;
        }
        else
            if (widthEndMaxLength > widthStartMinLength)
                widthStartMinLength = widthEndMaxLength;
    }
}
