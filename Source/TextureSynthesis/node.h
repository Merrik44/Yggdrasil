#ifndef NODE_H
#define NODE_H

enum NodeType
{
    none,
    node,
    oldCut,
    sink,
    tap
};

class Node
{
public:
    Node();
    Node(Node*,Node*,Node*,Node*);
    Node* left;
    Node* right;
    Node* up;
    Node* down;
    NodeType type;
    bool isSaturated;
};

#endif // NODE_H
