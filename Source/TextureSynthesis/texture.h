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
        Texture(Vector2D dimensions);
        Texture(QImage& imageInput);
        Texture(Vector2D startPoint,Vector2D dimensions, Texture& patch);
        
        QRgb** pixels;
        Vector2D size;
        QImage image;
        
        void clearPixelValues();
        void displayPixelValues();
        QImage getImage();
        
        void createSets();
        
        bool isInBounds(Vector2D& point);
        Vector2D mirrorNotInBounds(Vector2D & point);
        Vector2D wrapNotInBounds(Vector2D & point);
        
        int euclideanDistanceSquared(Texture & texture);
        int euclideanDistanceSquared(Vector2D inputPoint,Texture &otherTexture, Vector2D otherPoint);
        int euclideanDistanceSquaredNeighbourhood(Vector2D inputPoint, Texture& otherTexture,Vector2D otherPoint, int neighbourhoodSize);
        
        int euclideanDistanceSquaredNeighbourhoodWrap(Vector2D inputPoint, Texture& otherTexture,Vector2D otherPoint, int neighbourhoodSize);
        
        
        Vector2D findClosestNeighbourhood(Texture& otherTexture,Vector2D& otherPoint, int neighbourhoodSize);
        void halfSize();
        
//        Texture& operator = (Texture);
        
//        Texture subset(Vector2&,Vector2&);
    
};

#endif // PATCH_H
