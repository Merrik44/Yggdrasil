#ifndef LOADSKETCH_H
#define LOADSKETCH_H

#include <QXmlStreamReader>
#include <QFile>
#include "branch.h"
#include "segmentedbranch.h"

class LoadSketch
{
public:
    LoadSketch(QString filename);
    Branch* getTree();
    QVector<SegmentedBranch> getCurvedBranches();
    long getIdentifierIncrementer();

private:
    struct DataInfo;
    void readSketch(QString filename);
    void parseNewChild(QXmlStreamReader& xmlReader, Branch* parent);
    DataInfo parseAngleToParent(QXmlStreamReader& xmlReader, DataInfo data);
    DataInfo parseThickness(QXmlStreamReader& xmlReader, DataInfo data);
    DataInfo parseLength(QXmlStreamReader& xmlReader, DataInfo data);
    QString filename;
    Branch* tree;
    Branch* createBranch(DataInfo data, Branch* parent, bool trunk, long branchIdentifiers);
    long branchIdentifiers;
    void printTree(Branch* tr);
    Branch* findTrunk(Branch* tr);
    QVector<SegmentedBranch> curvedBranches;
};

#endif // LOADSKETCH_H
