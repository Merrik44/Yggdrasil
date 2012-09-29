#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "displaybranch.h"
#include "operation.h"
#include <list>
#include <string>

class TreeModel
{
private:
    std::list<DisplayBranch*> renderList;
    std::list<Operation*> XMLrenderList;

    int renderMode;

public:
    TreeModel();
    ~TreeModel();
    static TreeModel* createFromLST(const std::string &fileName);
    static TreeModel* createFromXML(const std::string &fileName);
    void Render();

    void calculateCameraSettings(float &x, float &y, float &z, float &ox, float &oy, float &oz);

    void writeToObj(const std::string &fileName);
};

#endif // TREEMODEL_H
