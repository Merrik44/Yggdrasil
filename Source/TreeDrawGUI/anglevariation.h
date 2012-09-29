#ifndef ANGLEVARIATION_H
#define ANGLEVARIATION_H

#include <QtGui>

class AngleVariation
{
public:
    AngleVariation();
    AngleVariation(QLineF branchLine);
    void drawLines(QPainter* painter, QPointF panDiff);
    void modifyVariation(QPointF mousePoint);
    QString checkOnLineA(QPointF mousePoint);
    void setAngleVariation(QPointF point, QString varType);
    QPointF getAngleVariation(QString varType);
    QLineF getMinLine();
    QLineF getMaxLine();
    void updatePoints(QLineF bl);
    void checkParentBranchIntersection(QPointF* parentBounds);
    float getDistanceToWidget();

private:
    void initialise();
    QPointF findPoint(double angle);
    QPointF calculateOpposingPoint(QPointF point1);
    QPointF closestPointOnLine(QPointF a, QPointF b, QPointF c);
    QPointF closestPointOnLineSegment(QPointF a, QPointF b, QPointF c);
    QPointF calcVarExtremePoint(QLineF norm, QPointF point, float len);
    QPointF findAdjacentPoint(QPointF mid, QPointF point);
    float midpointCalculation(float mid, float point);
    float findDistance(QPointF a, QPointF b);
    QLineF branchLine;
    QPointF origPoint;
    QPointF variationPoint;
    int length;
    double branchAngle;
    double variationAngle;
    float distToWidget;
};

#endif // ANGLEVARIATION_H
