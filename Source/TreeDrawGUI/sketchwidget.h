#ifndef SKETCHWIDGET_H
#define SKETCHWIDGET_H

#include <QWidget>
#include <QtGui>
#include "branch.h"
#include "newbranch.h"
#include "treefilexml.h"
#include "changeaddbranch.h"
#include "changeremovebranch.h"
#include "changewidthvariation.h"
#include "changelengthvariation.h"
#include "changeanglevariation.h"
#include "changeaddsegmentedbranch.h"
#include "segmentedbranch.h"
#include "widthchanges.h"
#include "selectbox.h"
#include "branchplacement.h"
#include "loadsketch.h"

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
QT_END_NAMESPACE

class SketchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SketchWidget(QWidget *parent = 0);
    void clear();
    void writeToXMLFile(QString path);
    void undoAction();
    void redoAction();
    void deleteKeyPressed();
    void toggleMode();
    void ctrlPressed();
    void ctrlReleased();
    void selectAll();
    void deselectAll();
    void setLineMode();
    void setPencilMode();
    void setSelectMode();
    void newSketch();
    void setInitialWidth(float initWidth);
    void loadSketch(QString path);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);    

private:
    QPointF startPos;
    QPointF endPos;
    QPointF prevMousePos;
    bool currentLine;
    bool treeEmpty;
    Branch* tree;
    void deselectBranch(Branch* tr);
    void selectAllBranches(Branch* tr);
    float FindDistance(QPointF a, QPointF b);
    void SelectBranch(QPointF nb, Branch* tr);
    QPointF FindPoint(QPointF a, QPointF b, QPointF c);
    void paintTree(QPainter* painter, Branch* branch, bool drawSelected);
    void paintVariation(QPainter* painter, Branch* tree);
    bool checkForSelectedBranch(Branch* tr);
    void findOtherSelectedBranches(Branch* tr, QString varType, QString var);
    QPointF* drawFirstCurrentLine(QPainter* painter, QLineF currentLine, float width);
    QPointF* drawFollowingCurrentLine(QPainter* painter, QLineF currentLine, float width, QPointF* bottomPoints);
    QPointF calcVarExtremePoint(QLineF norm, QPointF point, float len);
    QPointF findAdjacentPoint(QPointF mid, QPointF point);
    float midpointCalculation(float mid, float point);
    NewBranch newBranch;
    bool mouseButtonPressed;

    bool thickVar;
    bool lengthVar;
    bool angleVar;
    bool segAngleVar;
    bool segThickVar;
    bool segLengthVar;

    bool toggleAccentSelected;
    QString thickVarType;
    QString lengthVarType;
    QString angleVarType;
    Branch* branchBeingModified;
    long branchBeingModifiedIdentifier;
    float origVariationLength;
    float finalVariationLength;
    WidthChanges currBranchWidthOrig;
    WidthChanges currBranchWidthFinal;
    QPointF origVariationPoint;
    QPointF finalVariationPoint;
    QVector<long> branchesBeingModifiedIdentifier;
    QVector<float> origVariationLengths;
    QVector<float> finalVariationLengths;
    QVector<QPointF> origVariationPoints;
    QVector<QPointF> finalVariationPoints;
    QVector<WidthChanges> origWidthChanges;
    QVector<WidthChanges> finalWidthChanges;
    void checkVariation(Branch* tree, QPointF mousePoint);
    void applyWidthChangeToOther(float widthChange, QString thickVarType);
    void setSelectedBranchWidth(Branch* tr, long id, float widthChange, QString varType);
    void setFinalWidthVariations(Branch* tr, long branchIdentifier, QString varType);
    void applyLengthChangeToOther(float lengthChange, QString varType);
    void setSelectedBranchLength(Branch* tr, long id, float lengthChange, QString varType);
    void setFinalLengthVariations(Branch* tr, long branchIdentifier, QString varType);
    void applyAngleChangeToOther(float angleChange, QString varType);
    void setSelectedBranchAngle(Branch* tr, long id, float angleChange, QString varType);
    void setFinalAngleVariations(Branch* tr, long branchIdentifier, QString varType);
    void setBranchStatus(long iden, Branch* tr, bool setTo);
    int findSegmentedBranchIndex(long branchIdentifier);
    void findRootSegment(Branch* tr, long iden);

    QVector<SegmentedBranch> segBranches;

    TreeFileXML treeFileXML;
    float smallDist;
    Branch* closestBranch;
    bool isBranchSelected;
    QPen pen;

    QStack<Change*> changeStack;
    QStack<Change*> redoStack;

    int identifierIncrementer;

    bool pencilMode;
    bool lineMode;
    bool selectMode;

    QTime segBranchTimer;
    QVector<QLineF> newBranchSegs;
    QPointF currentSegStartPos;

    SelectBox sb;

    bool ctrlKeyDown;

    BranchPlacement bp;
    QPointF drawClosePoint;

    bool mouseInputAlreadyAccepted;

    float defaultThickness;
    QPointF panDiff;
    bool isPan;

    QVector<QLineF> applyPanDiffToBranchSegs(QVector<QLineF> newBranchSegs, QPointF panDiff);

signals:
    void sketchChanged();
    void sketchEmpty();
    void sketchNonEmpty();

public slots:

};

#endif // SKETCHWIDGET_H
