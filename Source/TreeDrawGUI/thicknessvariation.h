#ifndef THICKNESSVARIATION_H
#define THICKNESSVARIATION_H
#include <QtGui>

class ThicknessVariation
{
public:
    ThicknessVariation();
    ThicknessVariation(QLineF branchLine);
    void drawLines(QPainter *, QPointF panDiff);
    void modifyVariation(QPointF mousePoint, QPointF prevPoint, QString varType);
    float getVariationLength(QString type);
    QString checkOnPoint(QPointF mousePoint);
    void setVariationLength(float length, QString varType);
    void setCurveVariationLength(float len, QString varType);
    float getStartMin();
    float getStartMax();
    float getEndMin();
    float getEndMax();
    void updatePoints(QLineF bl);
    QPointF getVariationPoint(QString varType);
    float getDistanceToWidget();
    void setDefaultThickness(float width);

private:
    void calculatePoints(QLineF branchLine);
    QPointF calcVarExtremePoint(QLineF normal, QPointF point, float length);
    QPointF findAdjacentPoint(QPointF mid, QPointF point);
    float midpointCalculation(float mid, float point);
    QPointF closestPointOnLine(QPointF a, QPointF b, QPointF c);
    float distanceBetween(QPointF a, QPointF b);
    void drawBar(QPainter* painter, QPointF point1, QPointF point2, QPointF panDiff);
    void checkMaxEndWidthLimits();
    void checkMaxStartWidthLimits();
    QLineF branchLine;
    QLineF branchNormal;
    QPointF startMinPoint;
    QPointF startMaxPoint;
    float startMinLength;
    float startMaxLength;
    QPointF endMinPoint;
    QPointF endMaxPoint;
    float endMinLength;
    float endMaxLength;
    float distToWidget;
};

#endif // THICKNESSVARIATION_H
