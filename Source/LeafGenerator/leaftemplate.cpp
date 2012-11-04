#include "leaftemplate.h"

LeafTemplate::LeafTemplate()
{
    root = NULL;
}

QPointF* LeafTemplate::getRoot()
{
    if(root != NULL)
        return new QPointF(root->x(), root->y());
    else return NULL;
}

void LeafTemplate::setEdgeArray(QList<QVector2D*> *edges)
{
   edgeArray = edges;
}

void LeafTemplate::setRoot(const QPointF val)
{
    root = new QPointF(val.x(),val.y());
}

void LeafTemplate::setDefaultParameters()
{
    growthType = 0; //Types: 0 = uniform, 1 = linear, 2= functional (need a way to define functions, look up library)
    hGrowthVar = new QPointF(10,10);
    vGrowthVar = new QPointF(10,10);

    scaleVar= new QPointF(1,1);

    colourType = 0; //Gives the way a leaf is coloured. 0= uniform, 1=from edge, 2=from veins, 3 = speckled
    frontCol_MainMin = new QColor(Qt::green);
    frontCol_MainMax = new QColor(Qt::green);
    frontCol_AltMin = new QColor(Qt::green);
    frontCol_AltMax = new QColor(Qt::green);
    backCol_MainMin = new QColor(Qt::green);
    backCol_MainMax = new QColor(Qt::green);
    backCol_AltMin = new QColor(Qt::green);
    backCol_AltMax = new QColor(Qt::green);

    specVar = new QPointF(1,1);
    bumpVar = new QPointF(1,1);

    spawnRateVar = new QPointF(1,1);
    bsVar = new QPointF(1,1);
    bvVar = new QPointF(1,1);
    kdVar = new QPointF(1,1);
}

void LeafTemplate::setVariables(int growthType, QPointF *hGrowthVar, QPointF *vGrowthVar, QPointF *scaleVar,
                                int colourType, QColor* fCol_MMin, QColor* fCol_MMax, QColor* fCol_AMin, QColor* fCol_AMax,
                                QColor* bCol_MMin, QColor* bCol_MMax, QColor* bCol_AMin, QColor* bCol_AMax,
                                QPointF *specVar, QPointF *bumpVar,
                                QPointF *spawnRateVar, QPointF *bsVar, QPointF *bvVar, QPointF *kdVar,
                                QPointF* intensityVar, QColor* rootColour_min, QColor* rootColour_max, QPointF* startScaleVar)
{
    this->growthType = growthType;
    this->hGrowthVar = hGrowthVar;
    this->vGrowthVar = vGrowthVar;

    this->scaleVar= scaleVar;

    this->colourType = colourType;
    frontCol_MainMin = fCol_MMin;
    frontCol_MainMax = fCol_MMax;
    frontCol_AltMin = fCol_AMin;
    frontCol_AltMax = fCol_AMax;
    backCol_MainMin = bCol_MMin;
    backCol_MainMax = bCol_MMax;
    backCol_AltMin = bCol_AMin;
    backCol_AltMax = bCol_AMax;

    this->specVar = specVar;
    this->bumpVar = bumpVar;

    this->spawnRateVar = spawnRateVar;
    this->bsVar = bsVar;
    this->bvVar = bvVar;
    this->kdVar = kdVar;

    this->intensityVar = intensityVar;
    this->rootColour_min = rootColour_min;
    this->rootColour_max = rootColour_max;
    this->startScaleVar = startScaleVar;
}


QList<LeafModel*> LeafTemplate::createLeaves(const int numLeaves)
{
    //FOR DEBUG ONLY
    //setDefaultParameters();
    QString name;
    bool ok;
    QList<LeafModel*> finished = QList<LeafModel*>();

    QString fileName;
    fileName= QInputDialog::getText(NULL,QString("Leaf Group?"),
                                              QString("Store Leaf in Group: "), QLineEdit::Normal,
                                    NULL, &ok);
    if(ok)
    {
        if(fileName == NULL)
        {
            fileName = QString("default");
        }



        for(int i=0; i<numLeaves; i++)
        {
            name = fileName;// + QString::number(i);
            finished.append(new LeafModel(edgeArray, root,
                                          growthType, getIntRand(hGrowthVar), getIntRand(vGrowthVar), getRand(scaleVar),
                                          colourType, getRandColor(frontCol_MainMin, frontCol_MainMax), getRandColor(frontCol_AltMin,frontCol_AltMax),
                                          getRandColor(backCol_MainMin, backCol_MainMax), getRandColor(backCol_AltMin, backCol_AltMax),
                                          getRand(specVar), getRand(bumpVar),
                                          getRand(spawnRateVar), getRand(bsVar), getRand(bvVar), getRand(kdVar),
                                          name, getIntRand(intensityVar), getRandColor(rootColour_min,rootColour_max), getIntRand(startScaleVar)));
        }
    }
    return finished;
}


//Math-------------------------------------------------------------------------------------------------------------------------|

int LeafTemplate::getRand(int low, int high)
{
    return qrand()%(high+1-low)+low;
}

QColor* LeafTemplate::getRandColor(QColor* min, QColor* max)
{
    int i = qrand();
    int h = (int)(max->hue()*(float)i/(float)RAND_MAX + min->hue()*(float)(RAND_MAX-i)/(float)RAND_MAX);
    i = qrand();
    int s = (int)(max->saturation()*(float)i/(float)RAND_MAX + min->saturation()*(float)(RAND_MAX-i)/(float)RAND_MAX);
    i = qrand();
    int v = (int)(max->value()*(float)i/(float)RAND_MAX + min->value()*(float)(RAND_MAX-i)/(float)RAND_MAX);

    QColor *c = new QColor(Qt::black);
    *c = c->fromHsv(h,s,v);
    return c;
}

int LeafTemplate::getIntRand(QPointF* p)
{
    int x = (int)(p->x()+0.5);
    int y = (int)(p->y()+0.5);
    return x + qrand()%(y+1-x);
}

float LeafTemplate::getRand(QPointF* p)
{
    return p->x() + (float)qrand()/((float)RAND_MAX/(p->y()-p->x()));
}

