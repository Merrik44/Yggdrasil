#include "node.h"

Node::Node()
{
    
    type = none;
    isSaturated = false;
}

Node::Node(Node*l,Node*r,Node*u,Node*d)
{
    left = l;
    right =r;
    up = u;
    down = d;
    type = node;
    isSaturated = false;
}
