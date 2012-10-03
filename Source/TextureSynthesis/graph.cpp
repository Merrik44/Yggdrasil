#include "graph.h"
Graph::Graph()
{
}
Graph::Graph(int w, int h)
{
    width = w;   
    height = h;
    
    sinkNode = Node();
    tapNode = Node();
    sinkNode.type = sink;
    tapNode.type = tap;
    Vector2 v = Vector2(width,height);
    texture = Texture(v);
    
    nodes = new Node*[height];
    
    for(int y = 0; y<height;y++)
    {
        nodes[y] = new Node[width];
    
        for(int x = 0; x<width;x++)
        {
            nodes[y][x] = Node();
        }
    }
    
    for(int y = 0; y<height;y++)
    {    
        for(int x = 0; x<width;x++)
        {
            if(y+1<height)
            {
                nodes[y][x].down = &nodes[y+1][x];
            }
            if(y-1>=0)
            {
                nodes[y][x].up = &nodes[y-1][x];
            }
            if(x+1<width)
            {
                nodes[y][x].right = &nodes[y][x+1];
            }
            if(x-1>=0)
            {
                nodes[y][x].left = &nodes[y][x-1];
            }
        }
    }
    
}
