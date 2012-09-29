#ifndef LENGTHVARIATION_H
#define LENGTHVARIATION_H
#include <QtGui>

class LengthVariation
{
public:
    LengthVariation();
    LengthVariation(QLineF bl);
    void calcPoints(QLineF bl, float variation);
    void drawLines(QPainter* painter, QPointF panDiff);
    void modifyVariation(QPointF mousePoint, QString varType);
    QString checkOnLine(QPointF mousePoint);
    void setLengthVariation(float length, QString varType);
    float getLengthVariation(QString vt);
    float getMinLength();
    float getMaxLength();
    QPointF getMinPoint();
    QPointF getMaxPoint();
    void updatePoints(QLineF bl);
    void updateLengths();
    float getDistanceToWidget();
private:
    QPointF closestPointOnLine(QPointF a, QPointF b, QPointF c);
    QPointF calcVarExtremePoint(QLineF norm, QPointF point, float len);
    QPointF findAdjacentPoint(QPointF mid, QPointF point);
    float midpointCalculation(float mid, float point);
    QPointF findPoint(QPointF a, QPointF b, QPointF c);
    float findDistance(QPointF a, QPointF b);
    int lineWidth;
    QLineF branchLine;
    QLineF normalLine;
    QPointF minPoint;
    QPointF maxPoint;
    float minLength;
    float maxLength;
    float distToWidget;
};

#endif // LENGTHVARIATION_H
