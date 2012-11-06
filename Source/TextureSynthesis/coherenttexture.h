#ifndef INPUTTEXTURE_H
#define INPUTTEXTURE_H
#include "texture.h"
class PixelCoherence;
struct Pixel;
class IndexedTexture;
class CoherentTexture : public Texture
{
public:
    
    CoherentTexture();
    CoherentTexture(Vector2 dimensions);
    CoherentTexture(QImage& imageInput);
    CoherentTexture(Vector2 startPoint, Vector2 dimensions, Texture& patch);
    CoherentTexture(QImage& imageInput,CoherentTexture lowerResolution);
    
    PixelCoherence ** pixelCoherence;
    std::vector<int> neighbourhoodSizes;
    Pixel findClosestNeighbourhood(IndexedTexture& otherTexture,Vector2& otherPoint, int neighbourhoodSizeIndex, int coherenceQuality);
//    void findCandidates(Vector2& centrePoint,Vector2& samplePoint,IndexedTexture& otherTexture,Vector2& otherPoint, int neighbourhoodSizeIndex, int coherenceQuality, std::vector<Vector2> & candidateSet);
    void findCandidates(IndexedTexture& otherTexture,Vector2& otherPoint, int neighbourhoodSizeIndex, int coherenceQuality, std::vector<Vector2> & candidateSet);
    
    void createCoherence();
};

#endif // INPUTTEXTURE_H
