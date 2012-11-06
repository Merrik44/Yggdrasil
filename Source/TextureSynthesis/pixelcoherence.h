#ifndef PIXELCOHERENCE_H
#define PIXELCOHERENCE_H

#include "vector2.h"
#include "coherenttexture.h"
#include <vector>
struct Pixel
{
    Vector2D pos;
    int dist;
    Pixel()
    {
        pos = Vector2D();
        dist = 0;
    }
    Pixel(Vector2D position, int distance)
    {
        pos = position;
        dist = distance;
    }
};
class PixelCoherence
{
    
    
    class ComparePixels
    {
        public:
            bool operator () (Pixel & p1, Pixel & p2)
            {
                if(p1.dist>p2.dist)
                {
                    return true;
                }
                
                return false;
            }
    };
    
    public:
        PixelCoherence();
        
        PixelCoherence(CoherentTexture& sampleTexture, Vector2D pos, std::vector<int>* neighbourhood_Sizes);
        PixelCoherence(CoherentTexture& sampleTexture, PixelCoherence& lowResCoherence,Vector2D pos, std::vector<int>* neighbourhood_Sizes);
        
        std::vector<int>* neighbourhoodSizes;
        
        Vector2D ** neighbourPos;
//        int neighbourhoodSize;s
        int neighbourPosDim;
        Vector2D pixelPos;
        
};

#endif // PIXELCOHERENCE_H
