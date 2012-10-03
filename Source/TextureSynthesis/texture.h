#ifndef PATCH_H
#define PATCH_H
#include <QMainWindow>
#include "vector2.h"
#include <vector>
#include <map>

struct classcomp 
{
    bool operator() (const int& lhs, const int& rhs) const
    {
        return lhs<rhs;
    }
};

class Texture
{
    public:
        ~Texture();
        Texture();
        Texture(Vector2 dimensions);
        Texture(QImage& imageInput);
        Texture(Vector2 startPoint,Vector2 dimensions, Texture& patch);
        
        QRgb** pixels;
        Vector2 size;
        QImage image;
        
        void clearPixelValues();
        void displayPixelValues();
        QImage getImage();
        
        void createSets();
        
        bool isInBounds(Vector2& point);
        Vector2 mirrorNotInBounds(Vector2 & point, Vector2 & bounds);
        Vector2 wrapNotInBounds(Vector2 & point, Vector2 & bounds);
        
        int euclideanDistanceSquared(Texture & texture);
        int euclideanDistanceSquared(Vector2 inputPoint,Texture &otherTexture, Vector2 otherPoint);
        int euclideanDistanceSquaredNeighbourhood(Vector2 inputPoint, Texture& otherTexture,Vector2 otherPoint, int neighbourhoodSize);
        
        Vector2 findClosestNeighbourhood(Texture& otherTexture,Vector2& otherPoint, int neighbourhoodSize);
        void halfSize();
        
//        Texture& operator = (Texture);
        
//        Texture subset(Vector2&,Vector2&);
    
};

#endif // PATCH_H
