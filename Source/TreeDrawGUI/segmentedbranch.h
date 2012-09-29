#ifndef SEGMENTEDBRANCH_H
#define SEGMENTEDBRANCH_H
#include "branch.h"

class SegmentedBranch
{
public:
    SegmentedBranch();
    void addSegmentId(Branch* tr, long iden);
    void setLengthBranchPercentages(Branch* tr);
    bool hasMember(long iden);
    QVector<long> getAllIdentifiers();
    void setTree(Branch* tr);
    Branch* getTree();
    void drawBranch(Branch* tree, QPainter* painter, bool isSelected, QPointF panDiff);
    void drawVariation(Branch* tree, QPainter* painter, QPointF panDiff);
    QString angleCheckOnLine(Branch* tr, QPointF mousePoint);
    QString widthCheckOnPoint(Branch* tr, QPointF mousePoint);
    QString lengthCheckOnPoint(Branch* tr, QPointF mousePoint);
    void modifyAngleVariation(Branch* tr, QPointF mousePoint);
    void modifyWidthVariation(Branch* tr, QPointF mousePoint, QPointF prevPoint, QString vt);
    void modifyLengthVariation(Branch* tr, QPointF mousePoint, QString vt);
    void setWidthVariation(Branch* tr, float newLength, QString varType);
    float getWidthVariation(Branch* tr, QString varType);
    void setLengthVariation(Branch* tr, float newLength, QString varType);
    float getLengthVariation(Branch* tr, QString varType);

    float getDistanceToAngleWidget();
    float getDistanceToLengthWidget();
    float getDistanceToThicknessWidget();

private:
    void drawWidthVariation(QPainter* painter, QPointF panDiff);
    void drawLengthVariation(QPainter* painter, QPointF panDiff);
    void drawAngleVariation(QPainter* painter, QPointF panDiff);
    void drawBar(QPainter* painter, QPointF point1, QPointF point2, QPointF panDiff);
    QPointF calcVarExtremePoint(QLineF norm, QPointF point, float len);
    QPointF findPoint(double angle, QLineF branchLine, float length);
    QPointF calculateOpposingPoint(QPointF point1, QLineF branchLine);
    QPointF closestPointOnLine(QPointF a, QPointF b, QPointF c);
    QPointF closestPointOnLineSegment(QPointF a, QPointF b, QPointF c);
    QPointF findAdjacentPoint(QPointF mid, QPointF point);
    float midpointCalculation(float mid, float point);
    float findDistance(QPointF a, QPointF b);
    void findBranch(Branch* tree, long iden);
    void initialiseWidthVariationPoints();
    void resizeBranchWidths();
    void resizeBranchLengths(float newMinLength, float newMaxLength, QString vt);
    void setWidthValues(Branch* tr, long iden, float startMin, float startMax, float endMin, float endMax);
    float calculateBranchLength(QString varType);
    void checkMaxEndWidthLimits();
    void checkMaxStartWidthLimits();
    QPointF calculateLineIntersection(QLineF line1, QLineF line2);
    QVector<long> branchIdentifiers;
    Branch* tree;
    Branch* neededBranch;

    // Angle Stuff
    QPointF angleOrigPoint;
    QPointF angleVariationPoint;
    double angleLength;
    double angleBranchAngle;
    double angleVariationAngle;

    // Width Stuff
    QPointF widthStartMinPoint;
    QPointF widthStartMaxPoint;
    float widthStartMinLength;
    float widthStartMaxLength;
    QPointF widthEndMinPoint;
    QPointF widthEndMaxPoint;
    float widthEndMinLength;
    float widthEndMaxLength;

    // Length Stuff
    void lengthCalculateLengths();
    void lengthCalculatePoints();
    float lengthMin;
    float lengthMax;
    QPointF lengthMinPoint;
    QPointF lengthMaxPoint;
    Branch* closestBranch;
    long lengthClosestBranchIdentifier;
    float lengthClosestDistance;
    QPointF lengthClosestPointOnLine;
    QVector<float> lengthBranchPercentages;

    float distToAngleWidget;
    float distToLengthWidget;
    float distToThicknessWidget;
};

#endif // SEGMENTEDBRANCH_H
