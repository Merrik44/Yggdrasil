#ifndef LEAFTEMPLATE_H
#define LEAFTEMPLATE_H


#include <QVector2D>
#include <QPointF>
#include <QList>
#include <QListIterator>
#include <boost/math/common_factor.hpp>
#include "leafmodel.h"
#include <QInputDialog>

#include <QMessageBox>

class LeafModel;

class LeafTemplate
{
public:
    LeafTemplate();
    QList<LeafModel*> createLeaves(int numLeaves);
    LeafModel* createLeaf();
    QPointF* getRoot();

    //This is for debug
    void setDefaultParameters();
    void setVariables(int growthType, QPointF *hGrowthVar, QPointF *vGrowthVar, QPointF *scaleVar,
                      int colourType, QColor* fCol_MMin, QColor* fCol_MMax, QColor* fCol_AMin, QColor* fCol_AMax,
                      QColor* bCol_MMin, QColor* bCol_MMax, QColor* bCol_AMin, QColor* bCol_AMax,
                      QPointF *specVar, QPointF *bumpVar,
                      QPointF *spawnRateVar, QPointF *bsVar, QPointF *bvVar, QPointF *kdVar,
                      QPointF* intensityVar, QColor* rootColour_min, QColor* rootColour_max, QPointF* startScaleVar);
    int getRand(int low, int high);
    int getIntRand(QPointF* p); //Returns integer value
    QColor* getRandColor(QColor* min, QColor* max);
    float getRand(QPointF* p);

    void setEdgeArray(QList<QVector2D*> *edges);
    void setRoot(const QPointF val);

    //Actually useful functions
    QList<LeafModel*> create(int i);

private:
    //Functions-----------------------------------------------------------------------------------------------------------|
    QList<QPointF*> getBoundaries(QList<QPointF*> edges);
    QRect getBoundingRect(QList<QPointF*> bounds);
    QList<QPointF*> convertToPoints();

    //Variables-----------------------------------------------------------------------------------------------------------|

    //Stores the edges of the leaf, stored as vectors using the root position as (0,0)
    QList<QVector2D*> *edgeArray;
    QPointF *root;

    //Parameters (QPoint (min,max))
    //I need a definition for types of growth. For now...
    int growthType; //Types: 0 = uniform, 1 = linear, 2= functional (need a way to define functions, look up library)
    QPointF *hGrowthVar;
    QPointF *vGrowthVar;

    QPointF *scaleVar;

    int colourType; //Gives the way a leaf is coloured. 0= uniform, 1=from edge, 2=from veins
    QColor* frontCol_MainMin;
    QColor* frontCol_MainMax;
    QColor* frontCol_AltMin;
    QColor* frontCol_AltMax;
    QColor* backCol_MainMin;
    QColor* backCol_MainMax;
    QColor* backCol_AltMin;
    QColor* backCol_AltMax;

    QPointF *specVar;
    QPointF *bumpVar;

    QPointF *spawnRateVar;
    QPointF *bsVar;
    QPointF *bvVar;
    QPointF *kdVar;

    QString name;
    QPointF* intensityVar;
    QColor* rootColour_min;
    QColor* rootColour_max;
    QPointF* startScaleVar;
};

#endif // LEAFTEMPLATE_H
