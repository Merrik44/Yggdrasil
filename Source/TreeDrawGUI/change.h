#ifndef CHANGE_H
#define CHANGE_H

#include <QtGui>
#include "branch.h"

class Change
{
public:
    Change();
    virtual void apply() = 0;
    virtual void reverse() = 0;
    virtual Branch* getTree() = 0;
    virtual void setTree(Branch* tree) = 0;
    virtual QString getType() = 0;
};

#endif // CHANGE_H
