#ifndef FOLIAGEPARAMETERS_H
#define FOLIAGEPARAMETERS_H

#include <QObject>
#include <QString>
#include <QList>
#include <QTime>
#include <QVector>

#include "MeshGenerator/branchnode.h"
#include "fstream"
#include "math.h"
#include "stdlib.h"
#include <boost/algorithm/string.hpp>
#include <stack>
#include <QPointF>

using namespace std;
using namespace boost;
class BranchNode;

class FoliageParameters : public QObject
{
    Q_OBJECT

public:
    FoliageParameters(QObject *parent = 0);

    QList<QString>* leaves;
    QList<int>* ratios;
    int numberLeavesMin;
    int numberLeavesMax;
    float spreadMin;
    float spreadMax;
    float scaleMin;
    float scaleMax;

signals:
    void foliageChanged();

public slots:
    void valuesReturned(QList<QString>* leaves, QList<int>* ratios,
                        int numberLeavesMin, int numberLeavesMax,
                        float spreadMin, float spreadMax,
                        float scaleMin, float scaleMax);
    bool setValues();
    std::string createMesh(QString lst);

private:
    bool parseLST(QString lst);
    //returns x position of petiole
    int findAttachment(QString mtl);
    string createLeaves();

    int getIntRand(QPointF *p);
    int getIntRand(float x, float y);
    float getRand(QPointF* p);
    float getRand(float x, float y);


    QVector<BranchNode*> endPoints;
    int treeHeight;
    QString lst;
};

#endif // FOLIAGEPARAMETERS_H
