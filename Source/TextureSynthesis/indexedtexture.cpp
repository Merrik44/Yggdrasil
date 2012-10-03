#include "indexedtexture.h"

#include "coherenttexture.h"
#include <iostream>
using namespace std;
IndexedTexture::IndexedTexture(): Texture()
{
    createBlankImageIndex();
}
IndexedTexture::IndexedTexture(Vector2 dimensions): Texture(dimensions)
{
    createBlankImageIndex();
}
IndexedTexture::IndexedTexture(QImage& imageInput)
{
    image = imageInput;
    
    size = Vector2(image.width(),image.height());
        
    pixels = new QRgb*[(int)size.x];
    pixelLocations = new Vector2*[size.xInt()];
    
    for(int x = 0;x<size.x;x++)
    {
        pixels[x] = new QRgb[(int)size.y];
        pixelLocations[x] = new Vector2[size.yInt()];
    }
    
    for(int y = 0; y<size.y;y++)
    {
        QRgb* pixelRow = (QRgb*) image.scanLine(y);
        for(int x = 0;x<size.x;x++)
        {
            pixels[x][y] = pixelRow[x];
            Vector2 index = Vector2(x,y);
            pixelLocations[x][y] = index;
        }
    }
}
IndexedTexture::IndexedTexture(const IndexedTexture &input)
{
    image = input.image;
    
    size = Vector2(input.size.x,input.size.y);
        
    pixels = new QRgb*[(int)size.x];
    pixelLocations = new Vector2*[size.xInt()];
    
    for(int x = 0;x<size.x;x++)
    {
        pixels[x] = new QRgb[(int)size.y];
        pixelLocations[x] = new Vector2[size.yInt()];
    }
    
    for(int y = 0; y<size.y;y++)
    {
        for(int x = 0;x<size.x;x++)
        {
            pixels[x][y] = input.pixels[x][y];
            pixelLocations[x][y] = input.pixelLocations[x][y];
        }
    }
}

IndexedTexture::IndexedTexture(Vector2 startPoint,Vector2 dimensions, Texture& input)
{
    size = dimensions;
    
    pixels = new QRgb*[(int)size.x];
    pixelLocations = new Vector2*[size.xInt()];
    
    inputTexture = &input;
    for(int x = 0;x<size.x;x++)
    {
        pixels[x] = new QRgb[(int)size.y];
        pixelLocations[x] = new Vector2[size.yInt()];
    }
    
    for(int y = 0; y<size.y;y++)
    {
        for(int x = 0;x<size.x;x++)
        {
            QRgb temp = input.pixels[(int)startPoint.x+x][(int)startPoint.y+y];

            pixels[x][y] = temp;
            
            Vector2 index = Vector2(startPoint.xInt()+x,startPoint.yInt()+y);
            pixelLocations[x][y] = index;
        }
    }
}

void IndexedTexture::createBlankImageIndex()
{
    pixelLocations = new Vector2*[size.xInt()];
    for(int x = 0; x<size.x;x++)
    {
        pixelLocations[x] = new Vector2[size.yInt()];
        for(int y = 0; y<size.y;y++)
        {
            Vector2 index = Vector2();
            pixelLocations[x][y] = index;
        }
    }
}

void IndexedTexture::doubleSize(CoherentTexture &originalImage)
{
    
    
    
    Vector2 sizeTemp = size;
    sizeTemp*=2;
    QRgb** pixelsTemp = new QRgb*[(int)sizeTemp.x];
    Vector2** pixelLocationsTemp = new Vector2*[sizeTemp.xInt()];
    
    for(int x = 0;x<sizeTemp.x;x++)
    {
        pixelsTemp[x] = new QRgb[(int)sizeTemp.y];
        pixelLocationsTemp[x] = new Vector2[sizeTemp.yInt()];
    }
    Vector2 temp = Vector2();
    for(int y = 0; y<size.y;y++)
    {
        for(int x = 0;x<size.x;x++)
        {
            for(int i=0;i<2;i++)
            {
                for(int j=0;j<2;j++)
                {
                    temp = pixelLocations[x][y];
                    Vector2 index = Vector2(temp.x*2,temp.y*2);
//                    Vector2 index = Vector2(temp.x*2+i,temp.y*2+j);
//                    cout<<"x "<<index.x<<" y "<<index.y<<endl;
//                    cout<<"-x "<<x*2+1+i<<" y "<<y*2+1+j<<endl;
                    pixelsTemp[x*2+i][y*2+j] = originalImage.pixels[index.xInt()][index.yInt()];
                    pixelLocationsTemp[x*2+i][y*2+j] = index;
                }
            }
        }
    }
    
    pixels = pixelsTemp;
    pixelLocations = pixelLocationsTemp;
    size = sizeTemp;
    
    QSize sizeScaledTemp = QSize(sizeTemp.x,sizeTemp.y);
    
    Qt::AspectRatioMode aspectRatioModePatched = Qt::IgnoreAspectRatio;
    Qt::TransformationMode transformModePatched = Qt::SmoothTransformation;
    //cout<<"here1"<<endl;
 
    image = image.scaled(sizeScaledTemp, aspectRatioModePatched,transformModePatched);
}
