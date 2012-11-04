#ifndef TREENODE_H
#define TREENODE_H

#include <QList>
#include <QPointF>
#include <qmath.h>
#include <QImage>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QVector2D>
#include <QVector>

class TreeNode
{
public:
    TreeNode(QPointF* position, TreeNode* parent=NULL);
    TreeNode* findClosest(QPointF* auxin, QVector<int>* borders, int size);
    int lineCheck(TreeNode* node, QPointF* auxin, QVector<int>* borders, int size);
    void shift(QPointF* root, int inc);
    float draw(QImage *leaf, QColor colour);
    void addToGrid(QVector<QList<TreeNode*>*> veinGrid, int size, int gridSize);

    QList<TreeNode*>* children;
    TreeNode* parent;
    QPointF* position;
    float veinSize;
};

#endif // TREENODE_H
