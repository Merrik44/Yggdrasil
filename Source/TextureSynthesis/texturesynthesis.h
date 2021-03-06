#ifndef TEXTURESYNTHESIS_H
#define TEXTURESYNTHESIS_H

#include <vector>
#include <QString>
#include <QImage>
class Vector2D;
class CoherentTexture;
class IndexedTexture;
class Texture;
class TextureSynthesisDialog;


class TextureSynthesis
{
    public:
        TextureSynthesis();
        ~TextureSynthesis();
        
        std::vector<CoherentTexture> originalTextures;
        QImage originalImage;
    //    Texture * patchList;
        int quality;
        IndexedTexture * patchListIndexed;
    //    std::vector<int> neighbourhoodSizes;
        IndexedTexture * patchedTexture;
        QString imagePath;
        
        void loadImage(QString path);
        void synthesise();
        void synthesise(Vector2D & size, int iterations);
        TextureSynthesisDialog * dialog;
        
    private: 
        QRgb featheredPixel(QRgb& patchedPixel, QRgb& patchPixel, float ratio);
        Texture createRandomTexture(Texture & patchedTexture, Texture*& patchList, Vector2D& numPatches,int overlap);
        IndexedTexture createRandomTexture(IndexedTexture & patchedTexture, IndexedTexture*& patchList, Vector2D& numPatches);
        
        Texture textureOptimization(Texture & patchedTexture, Texture& sampleTexture, int iterations);
        IndexedTexture discreetOptimization(IndexedTexture & patchedTexture, CoherentTexture& sampleTexture, int iterations, int neighbourhoodSizeIndex);
        IndexedTexture discreetOptimization2(IndexedTexture & patchedTexture, CoherentTexture& sampleTexture, int iterations, int neighbourhoodSizeIndex);
        
         QImage scaleImage(Vector2D & sizeScaled, QImage & image);
};

#endif // TEXTURESYNTHESIS_H
