#include "indexedtexture.h"

#include "coherenttexture.h"
#include <iostream>
using namespace std;
IndexedTexture::IndexedTexture(): Texture()
{
    createBlankImageIndex();
}
IndexedTexture::IndexedTexture(Vector2D dimensions): Texture(dimensions)
{
    createBlankImageIndex();
}
IndexedTexture::IndexedTexture(QImage& imageInput)
{
    image = imageInput;
    
    size = Vector2D(image.width(),image.height());
        
    pixels = new QRgb*[(int)size.x];
    pixelLocations = new Vector2D*[size.xInt()];
    
    for(int x = 0;x<size.x;x++)
    {
        pixels[x] = new QRgb[(int)size.y];
        pixelLocations[x] = new Vector2D[size.yInt()];
    }
    
    for(int y = 0; y<size.y;y++)
    {
        QRgb* pixelRow = (QRgb*) image.scanLine(y);
        for(int x = 0;x<size.x;x++)
        {
            pixels[x][y] = pixelRow[x];
            Vector2D index = Vector2D(x,y);
            pixelLocations[x][y] = index;
        }
    }
}
IndexedTexture::IndexedTexture(const IndexedTexture &input)
{
    image = input.image;
    
    size = Vector2D(input.size.x,input.size.y);
        
    pixels = new QRgb*[(int)size.x];
    pixelLocations = new Vector2D*[size.xInt()];
    
    for(int x = 0;x<size.x;x++)
    {
        pixels[x] = new QRgb[(int)size.y];
        pixelLocations[x] = new Vector2D[size.yInt()];
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

IndexedTexture::IndexedTexture(Vector2D startPoint,Vector2D dimensions, Texture& input)
{
    size = dimensions;
    
    pixels = new QRgb*[(int)size.x];
    pixelLocations = new Vector2D*[size.xInt()];
    
    inputTexture = &input;
    for(int x = 0;x<size.x;x++)
    {
        pixels[x] = new QRgb[(int)size.y];
        pixelLocations[x] = new Vector2D[size.yInt()];
    }
    
    for(int y = 0; y<size.y;y++)
    {
        for(int x = 0;x<size.x;x++)
        {
            QRgb temp = input.pixels[(int)startPoint.x+x][(int)startPoint.y+y];

            pixels[x][y] = temp;
            
            Vector2D index = Vector2D(startPoint.xInt()+x,startPoint.yInt()+y);
            pixelLocations[x][y] = index;
        }
    }
}

void IndexedTexture::createBlankImageIndex()
{
    pixelLocations = new Vector2D*[size.xInt()];
    for(int x = 0; x<size.x;x++)
    {
        pixelLocations[x] = new Vector2D[size.yInt()];
        for(int y = 0; y<size.y;y++)
        {
            Vector2D index = Vector2D();
            pixelLocations[x][y] = index;
        }
    }
}

void IndexedTexture::doubleSize(CoherentTexture &originalImage)
{
    
    
    
    Vector2D sizeTemp = size;
    sizeTemp*=2;
    QRgb** pixelsTemp = new QRgb*[(int)sizeTemp.x];
    Vector2D** pixelLocationsTemp = new Vector2D*[sizeTemp.xInt()];
    
    for(int x = 0;x<sizeTemp.x;x++)
    {
        pixelsTemp[x] = new QRgb[(int)sizeTemp.y];
        pixelLocationsTemp[x] = new Vector2D[sizeTemp.yInt()];
    }
    Vector2D temp = Vector2D();
    for(int y = 0; y<size.y;y++)
    {
        for(int x = 0;x<size.x;x++)
        {
            for(int i=0;i<2;i++)
            {
                for(int j=0;j<2;j++)
                {
                    temp = pixelLocations[x][y];
                    Vector2D index = Vector2D(temp.x*2,temp.y*2);
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
