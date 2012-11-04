#include "treenode.h"

TreeNode::TreeNode(QPointF* position, TreeNode* parent)
{
    this->position = position;
    children = new QList<TreeNode*>();
    this->parent = parent;
    veinSize = 1;
}

/*TreeNode* TreeNode::findClosest(QPointF* auxin,  QVector<int> *borders, int size)
{
    TreeNode* closest;
    TreeNode* next;
    if(children->size() > 0)
    {
        QListIterator<TreeNode*> nodeIt(*children);
        //We start with this node (it may well be the closest)
        closest = this;
        int lineClosest = lineCheck(closest, auxin, borders, size);
        while(nodeIt.hasNext())
        {
            next = nodeIt.next()->findClosest(auxin, borders, size);
            int lineNext = lineCheck(next, auxin, borders, size);
            float closestDist = lineClosest + sqrt(pow(closest->position->x()-auxin->x(), 2) + pow(closest->position->y()-auxin->y(), 2));
            float nextDist = lineNext + sqrt(pow(next->position->x()-auxin->x(), 2) + pow(next->position->y()-auxin->y(), 2));

            if(nextDist < closestDist)
            {
                closest = next;
                lineClosest = lineNext;
            }
        }
        return closest;
    }
    else
        return this;
}*/

//Does a straight line check to see if a node is in sight of a vein, else returns 10,000 (so not impossible to grow to, but not preferable)
int TreeNode::lineCheck(TreeNode* node, QPointF* auxin, QVector<int>* borders, int size)
{
    /*QPointF p(node->position->x(), node->position->y());
    while (p.toPoint() != auxin->toPoint())
    {
        if(auxin->x() - p.x() >= 0.5)
            p.setX(p.x()+1);
        else if(auxin->x() - p.x() <= -0.5)
             p.setX(p.x()-1);

        if(auxin->y() - p.y() >= 0.5)
            p.setY(p.y()+1);
        else if(auxin->y() - p.y() <= -0.5)
             p.setY(p.y()-1);

        //If not in traversible space
        int x = p.toPoint().x();
        int y = p.toPoint().y();
        if(borders->at(x+size*y) != 0)
            return 0;//10000;
    }*/
    return 0;
}

void TreeNode::shift(QPointF* root, int inc)
{
    //This seems right...
    position->setX(position->x() + (position->x()-root->x())/(float)(inc+1));
    position->setY(position->y() + (position->y()-root->y())/(float)(inc+1));

    QListIterator<TreeNode*> nodeIt(*children);
    while(nodeIt.hasNext())
    {
        TreeNode* next = nodeIt.next();
        next->shift(root, inc);
    }
}

//Draws itself to it's parent and returns its width
float TreeNode::draw(QImage* leaf, QColor colour)
{
    if(children->size() > 0)
    {
        float rad = 0;
        QListIterator<TreeNode*> nodeIt(*children);
        while(nodeIt.hasNext())
        {
            TreeNode* next = nodeIt.next();
            float newRad = next->draw(leaf, colour);
            rad += pow(newRad, 3);
        }
        //"Murray's Law"
        rad = pow(rad,(double)1/(double)3);// + 1;

        if(parent != NULL)
        {
            QPainter p;
            p.begin(leaf);
            p.setPen(QPen(colour.toRgb(), rad, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.drawLine(*position, *(parent->position));
            p.end();
        }
        veinSize = rad;
        return rad;

    }
    else
    {
        if(parent != NULL)
        {
            QPainter p;
            p.begin(leaf);
            p.setPen(QPen(colour.toRgb(), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

            p.drawLine(*position, *(parent->position));
            p.end();
        }
        veinSize = 1;
        return 1;
    }
    veinSize = 1;
    return 1;
}

void TreeNode::addToGrid(QVector<QList<TreeNode*>*> veinGrid, int gridCount, int gridSize)
{
    QPoint pos = position->toPoint();
    //This may put things into the wrong bracket (ie 9.87, should be in 0 bucket, but gets put into 1)
    //This is not an issue though, since surrounding boxes are always checked anyway
    veinGrid[pos.x()/gridSize + (gridCount)*(pos.y()/gridSize)]->append(this);

    QListIterator<TreeNode*> childIt(*children);
    while(childIt.hasNext())
    {
        TreeNode* c = childIt.next();
        c->addToGrid(veinGrid,gridCount, gridSize);
    }
}
