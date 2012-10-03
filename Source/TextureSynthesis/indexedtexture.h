#ifndef INDEXEDTEXTURE_H
#define INDEXEDTEXTURE_H
#include "texture.h"
class CoherentTexture;
class IndexedTexture : public Texture
{
public:
    IndexedTexture();
    IndexedTexture(Vector2 dimensions);
    IndexedTexture(QImage& imageInput);
    IndexedTexture(const IndexedTexture & imageInput);
    IndexedTexture(Vector2 startPoint,Vector2 dimensions, Texture& input);
        
    Vector2 ** pixelLocations;
    Texture* inputTexture;
    
    void createBlankImageIndex();
    void doubleSize(CoherentTexture & originalImage);
};

#endif // INDEXEDTEXTURE_H
