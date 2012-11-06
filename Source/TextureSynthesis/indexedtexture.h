#ifndef INDEXEDTEXTURE_H
#define INDEXEDTEXTURE_H
#include "texture.h"
class CoherentTexture;
class IndexedTexture : public Texture
{
public:
    IndexedTexture();
    IndexedTexture(Vector2D dimensions);
    IndexedTexture(QImage& imageInput);
    IndexedTexture(const IndexedTexture & imageInput);
    IndexedTexture(Vector2D startPoint,Vector2D dimensions, Texture& input);
        
    Vector2D ** pixelLocations;
    Texture* inputTexture;
    
    void createBlankImageIndex();
    void doubleSize(CoherentTexture & originalImage);
};

#endif // INDEXEDTEXTURE_H
