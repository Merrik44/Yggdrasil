#ifndef BRANCH_H
#define BRANCH_H

#include <QObject>
#include <QVector>
#include <QtGui>

#include "thicknessvariation.h"
#include "lengthvariation.h"
#include "anglevariation.h"

class Branch : public QObject
{
public:
    Branch();
    Branch(QPointF startPos, QPointF endPos);
    Branch(QPointF startPos, QPointF endPos, Branch* parent, double iden);
    ~Branch();
    void setPos (QPointF startPos, QPointF endPos);
    void drawLine(QPainter* painter, QPointF panDiff);
    QPointF calcVarExtremePoint(QLineF norm, QPointF point, float len);
    QPointF findAdjacentPoint(QPointF mid, QPointF point);
    float midpointCalculation(float mid, float point);
    void setParent(Branch* parent);
    void addChild(Branch* child);
    void setBranchLine(QLineF newBranch);
    QPointF getStartPos();
    QPointF getEndPos();
    int numChildren();
    Branch* childAt(int i);
    QLineF getBranchLine();
    void setThickness();
    void drawVariation(QPainter* painter, QPointF panDiff);
    void modifyWidthVariation(QPointF mousePoint, QPointF prevPoint, QString varType);
    void modifyLengthVariation(QPointF mousePoint, QString varType);
    void modifyAngleVariation(QPointF mousePoint);
    QString checkOnPoint(QPointF mousePoint);
    QString checkOnLine(QPointF mousePoint);
    QString checkOnLineA(QPointF mousePoint);
    void removeChildAt(int ind);

    float getThicknessVariationStartMin();
    float getThicknessVariationStartMax();
    float getThicknessVariationEndMin();
    float getThicknessVariationEndMax();

    float getLengthVariationMin();
    float getLengthVariationMax();

    QLineF getAngleLineMin();
    QLineF getAngleLineMax();

    Branch* getParent();

    float getPosOnParent();
    void setPosOnParent(float posOnParent);
    void setOrigPosOnParent(QPointF origPos);
    QPointF getOrigPosOnParent();

    void setIsTrunk(bool isTrunk);
    bool getIsTrunk();

    void setIsSelected(bool isSelected);
    bool getIsSelected();

    void setIdentifier(long iden);
    long getIdentifier();

    void setWidthVariation(float length, QString varType);
    void setCurveWidthVariation(float len, QString vt);
    float getWidthVariation(QString vt);

    void setLengthVariation(float length, QString varType);
    float getLengthVariation(QString vt);

    void setAngleVariation(QPointF point, QString varType);
    QPointF getAngleVariation(QString vt);

    void updateLengthRect();
    QPointF getLengthMinPoint();
    QPointF getLengthMaxPoint();

    QPointF* getMaxRect();

    QLineF moveBranchToEdgeOfRect(QPointF currPoint, QLineF newBranch, float posOnParent);

    void setIsSegment(bool segment);
    bool getIsSegment();

    void setIsRootSegment(bool rootSegment);
    bool getIsRootSegment();

    void setSegmentId(int seg);
    int getSegmentId();

    QPointF getWidthVariationPoint(QString varType);

    void drawAngleVariation(QPainter *painter, QPointF panDiff);

    float getDistanceToAngleWidget();
    float getDistanceToLengthWidget();
    float getDistanceToThicknessWidget();

    void setDefaultThickness(float width);
    void postModificationIntegrity();

private:    
    void drawLineWidth(QPainter* painter);
    void drawLineLength(QPainter* painter);
    void drawBranches(QPainter* painter, QPointF panDiff);
    QPointF calculateLineIntersection(QLineF line1, QLineF line2);
    bool isOnSegment(double xi, double yi, double xj, double yj, double xk, double yk);
    char computeDirection(double xi, double yi, double xj, double yj, double xk, double yk);
    bool doLineSegmentsIntersect(QLineF line1, QLineF line2);
    float findDistance(QPointF a, QPointF b);
    void applyCalculatePosOnRect();
    void updateVariation();
    float calcLengthRect();
    bool isChildSelected(Branch* tree);
    bool isTrunk;
    bool isSelected;
    QPointF startPos;
    QPointF endPos;
    QLineF branchLine;
    Branch* parent;
    QVector<Branch*> children;
    QLineF startThicknessMin;
    QLineF startThicknessMax;
    QLineF endThicknessMin;
    QLineF endThicknessMax;
    ThicknessVariation tv;
    LengthVariation lv;
    AngleVariation av;
    float posOnParent;
    long identifier;
    QPointF origPosOnParent;

    QPointF* minRect;
    QPointF* maxRect;

    bool isSegment;
    bool isRootSegment;
    int segmentId;

signals:

public slots:

};

#endif // BRANCH_H
