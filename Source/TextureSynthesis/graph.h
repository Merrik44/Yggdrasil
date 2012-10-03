#ifndef GRAPH_H
#define GRAPH_H

#include "node.h"
#include "texture.h"
#include "vector2.h"

class Graph
{
public:
    Graph();
    Graph(int,int);
private:
    int width;
    int height;
    Node** nodes;
    Node sinkNode;
    Node tapNode;
    Texture texture;
};

#endif // GRAPH_H
