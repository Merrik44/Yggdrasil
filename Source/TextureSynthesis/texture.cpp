#include <iostream>
#include "texture.h"
#include "math.h"
using namespace std;
//==================================Constructor/destructors=========================================

Texture::~Texture()
{
//    for(int y = 0; y<size.height();y++)
//    {
//        delete[] pixels[y];
//    }
//    delete[] pixels;
//    std::cout<<"Patch deleted"<<std::endl;
}

Texture::Texture()
{
    size = Vector2(32,32);
    pixels = new QRgb*[(int)size.x];
    for(int x = 0; x<size.x;x++)
    {
        pixels[x] = new QRgb[(int)size.y];
        for(int y = 0; y<size.y;y++)
        {
            QRgb temp = qRgba(0,0,0,0);
            pixels[x][y] = temp;
        }
    }
}

Texture::Texture(Vector2 dimensions)
{
    size = Vector2(dimensions.x,dimensions.y);
    pixels = new QRgb*[(int)size.x];
    for(int x = 0; x<size.x;x++)
    {
        pixels[x] = new QRgb[(int)size.y];
        for(int y = 0; y<size.y;y++)
        {
            QRgb temp = qRgba(0,0,0,0);
            pixels[x][y] = temp;
        }
    }
}

Texture::Texture(QImage &imageInput)
{
    image = imageInput;
    
    size = Vector2(image.width(),image.height());
        
    pixels = new QRgb*[(int)size.x];
    
    for(int x = 0;x<size.x;x++)
    {
        pixels[x] = new QRgb[(int)size.y];
    }
    
    for(int y = 0; y<size.y;y++)
    {
        QRgb* pixelRow = (QRgb*) image.scanLine(y);
        for(int x = 0;x<size.x;x++)
        {
            pixels[x][y] = pixelRow[x];
        }
    }
}

Texture::Texture(Vector2 startPoint, Vector2 dimensions, Texture &input)
{
    size = dimensions;
    
    
    pixels = new QRgb*[(int)size.x];
    
    for(int x = 0;x<size.x;x++)
    {
        pixels[x] = new QRgb[(int)size.y];
    }
    
    for(int y = 0; y<size.y;y++)
    {
        for(int x = 0; x<size.x;x++)
        {
            QRgb temp = input.pixels[(int)startPoint.x+x][(int)startPoint.y+y];

            pixels[x][y] = temp;
        }
    }
}

//==================================Functions=========================================

void Texture::clearPixelValues()
{
    for(int x = 0; x<size.x;x++)
    {
        for(int y = 0; y<size.y;y++)
        {
            QRgb temp = qRgba(0,0,0,0);
            pixels[x][y] = temp;
        }
    }
}

void Texture::displayPixelValues()
{
    for(int x = 0; x<size.x;x++)
    {
        for(int y = 0; y<size.y;y++)
        {
            std::cout<<qGreen(pixels[x][y])<<" ";
        }
        std::cout<<std::endl;
    }
}

QImage Texture::getImage()
{
    
    for(int y = 0; y<size.y;y++)
    {
        QRgb* pixelRow = (QRgb*) image.scanLine(y);

        for(int x = 0;x<size.x;x++)
        {
            pixelRow[x] = pixels[x][y];
        }
    }

    return image;
}

bool Texture::isInBounds(Vector2& point)
{
    if(point.x<0 || point.y<0)
    {
        return false;
    }
    if(point.x>size.x-1 || point.y>size.y-1)
    {
        return false;
    }
    return true;
}

int Texture::euclideanDistanceSquared(Texture &t)
{
    int red = 0;
    int blue = 0;
    int green = 0;
    int alpha = 0;
    int numPixels = size.y*size.x;
    
    
    for(int x = 0;x<size.x;x++)
    {
        for(int y = 0; y<size.y;y++)
        {
            int distRed = (qRed(pixels[x][y]) - qRed(t.pixels[x][y]));
            int distBlue = (qBlue(pixels[x][y]) - qBlue(t.pixels[x][y]));
            int distGreen = (qGreen(pixels[x][y]) - qGreen(t.pixels[x][y]));
            int distAlpha = (qAlpha(pixels[x][y]) - qAlpha(t.pixels[x][y]));
            
            red += distRed * distRed;
            blue += distBlue * distBlue;
            green += distGreen * distGreen;
            alpha += distAlpha * distAlpha;
            
        }
    }
    
    red /= numPixels;
    blue /= numPixels;
    green /= numPixels;
    alpha /= numPixels;
    
    return red + blue + green + alpha;
}
int Texture::euclideanDistanceSquared(Vector2 inputPoint, Texture &otherTexture, Vector2 otherPoint)
{
    int red = 0;
    int blue = 0;
    int green = 0;
    int alpha = 0;

    int distRed = (qRed(pixels[inputPoint.xInt()][inputPoint.yInt()]) - qRed(otherTexture.pixels[otherPoint.xInt()][otherPoint.yInt()]));
    int distBlue = (qBlue(pixels[inputPoint.xInt()][inputPoint.yInt()]) - qBlue(otherTexture.pixels[otherPoint.xInt()][otherPoint.yInt()]));
    int distGreen = (qGreen(pixels[inputPoint.xInt()][inputPoint.yInt()]) - qGreen(otherTexture.pixels[otherPoint.xInt()][otherPoint.yInt()]));
    int distAlpha = (qAlpha(pixels[inputPoint.xInt()][inputPoint.yInt()]) - qAlpha(otherTexture.pixels[otherPoint.xInt()][otherPoint.yInt()]));
    
    red += distRed * distRed;
    blue += distBlue * distBlue;
    green += distGreen * distGreen;
    alpha += distAlpha * distAlpha;
    return red + blue + green + alpha;
}

int Texture::euclideanDistanceSquaredNeighbourhood(Vector2 inputPoint, Texture& otherTexture,Vector2 otherPoint, int neighbourhoodSize)
{
    //the total squared distance for each colour
    int red = 0;
    int blue = 0;
    int green = 0;
    int alpha = 0;
//    if(inputPoint.x == 0 &&inputPoint.y==0 && otherPoint.x==1&&otherPoint.y==1)
//    cout<<"RedTotalStart "<<red<<endl;
//    if(inputPoint.x == 0 &&inputPoint.y==0 && otherPoint.x==1&&otherPoint.y==1)
//    cout<<"inputPoint = "<<inputPoint.x<<"  -  "<<inputPoint.y<<endl;
//    if(inputPoint.x == 0 &&inputPoint.y==0 && otherPoint.x==1&&otherPoint.y==1)
//    cout<<"======================================================================"<<endl;
    //the total number of pixels
    //    int totalpixels = (neighbourhoodSize*2+1)*(neighbourhoodSize*2+1);
    int totalpixels = 0;
    
    Vector2 thisPos = Vector2();
    Vector2 otherPos = Vector2();
    
    for(int x = -neighbourhoodSize;x<=neighbourhoodSize;x++)
    {
        for(int y = -neighbourhoodSize; y<=neighbourhoodSize;y++)
        {
            
            thisPos.x = x+inputPoint.x;
            thisPos.y = y+inputPoint.y;
            
            thisPos = mirrorNotInBounds(thisPos,size);
            
            otherPos.x = x+otherPoint.x;
            otherPos.y = y+otherPoint.y;
            
            otherPos = mirrorNotInBounds(otherPos,otherTexture.size);
//            if(inputPoint.x == 0 &&inputPoint.y==0 && otherPoint.x==1&&otherPoint.y==1)
//            cout<<"ThisPos = "<<thisPos.x<<"  -  "<<thisPos.y<<endl;
//            if(inputPoint.x == 0 &&inputPoint.y==0 && otherPoint.x==1&&otherPoint.y==1)
//            cout<<"Red "<<qRed(pixels[(int)thisPos.x][(int)thisPos.y])<<"  -  "<<qRed(otherTexture.pixels[(int)otherPos.x][(int)otherPos.y])<<endl;
            if(this->isInBounds(thisPos) && otherTexture.isInBounds(otherPos))
            {
                int distRed = (qRed(pixels[(int)thisPos.x][(int)thisPos.y]) - qRed(otherTexture.pixels[(int)otherPos.x][(int)otherPos.y]));
                int distBlue = (qBlue(pixels[(int)thisPos.x][(int)thisPos.y]) - qBlue(otherTexture.pixels[(int)otherPos.x][(int)otherPos.y]));
                int distGreen = (qGreen(pixels[(int)thisPos.x][(int)thisPos.y]) - qGreen(otherTexture.pixels[(int)otherPos.x][(int)otherPos.y]));
                int distAlpha = (qAlpha(pixels[(int)thisPos.x][(int)thisPos.y]) - qAlpha(otherTexture.pixels[(int)otherPos.x][(int)otherPos.y]));
//                cout<<">"<<thisPos.x<<"  -  "<<thisPos.y<<endl;
//                cout<<">"<<thisPos.x<<"  -  "<<thisPos.y<<endl;
                red += (distRed * distRed);
                blue += (distBlue * distBlue);
                green += (distGreen * distGreen);
                alpha += (distAlpha * distAlpha);
                totalpixels++;
//                if(inputPoint.x == 0 &&inputPoint.y==0 && otherPoint.x==1&&otherPoint.y==1)
//                cout<<"RedSum "<<red<<endl;
            }
        }
    }
    
    red /= totalpixels;
//    if(inputPoint.x == 0 &&inputPoint.y==0 && otherPoint.x==1&&otherPoint.y==1)
//    cout<<"RedTotal "<<red<<endl;
    blue /= totalpixels;
    green /= totalpixels;
    alpha /= totalpixels;
//    return red + blue + green;
    return red + blue + green + alpha;
}

Vector2 Texture::mirrorNotInBounds(Vector2 & point, Vector2 & bounds)
{
    Vector2 temp = point;
    
    if(point.x<0)
    {
        temp.x = -temp.x;
    }
    else if(point.x>=bounds.x)
    {
        temp.x = bounds.x - 1 + (bounds.x - 1 - temp.x);
    }
    if(point.y<0)
    {
        temp.y = -temp.y;
    }
    else if(point.y>=bounds.y)
    {
        temp.y = bounds.y - 1 + (bounds.y - 1 - temp.y);
    }
    
    return temp;
}
Vector2 Texture::wrapNotInBounds(Vector2 & point, Vector2 & bounds)
{
    Vector2 temp = point;
    
    if(point.x<0)
    {
        temp.x = bounds.x + temp.x;
    }
    else if(point.x>=bounds.x)
    {
        temp.x = (temp.x - (bounds.x - 1));
    }
    if(point.y<0)
    {
        temp.y = bounds.y + temp.y;
    }
    else if(point.y>=bounds.y)
    {
        temp.y = (temp.y - (bounds.y - 1));
    }
    
    return temp;
}
Vector2 Texture::findClosestNeighbourhood(Texture& otherTexture,Vector2& otherPoint, int neighbourhoodSize)
{
    Vector2 closest = Vector2();
    int closestDist = 1000000;
    
    Vector2 current = Vector2();
    int currentDist = 0;
    int neighbourhoodIncrement = (int)(neighbourhoodSize*2+1)/4;
    for(int x = 0;x< size.x;x+=neighbourhoodIncrement)
    {
        for(int y = 0; y<size.y;y+=neighbourhoodIncrement)
        {
            current.x = x;
            current.y = y;
            currentDist = euclideanDistanceSquaredNeighbourhood(current, otherTexture,otherPoint, neighbourhoodSize);
            if(currentDist<closestDist)
            {
                closestDist = currentDist;
                closest.x = current.x;
                closest.y = current.y;
           }
        }
    }
    return closest;
    
}

void Texture::halfSize()
{
    Vector2 sizeTemp = size;
    sizeTemp/=2;
    QRgb** pixelsTemp = new QRgb*[(int)sizeTemp.x];
//    Vector2** pixelLocationsTemp = new Vector2*[sizeTemp.xInt()];
    
    for(int x = 0;x<sizeTemp.x;x++)
    {
        pixelsTemp[x] = new QRgb[(int)sizeTemp.y];
//        pixelLocationsTemp[x] = new Vector2[sizeTemp.yInt()];
    }
    Vector2 temp = Vector2();
    
    for(int y = 0; y<sizeTemp.y;y++)
    {
        for(int x = 0;x<sizeTemp.x;x++)
        {
            int r = qRed(pixels[x][y]) + qRed(pixels[x+1][y])+qRed(pixels[x][y+1])+qRed(pixels[x+1][y+1]);
            int g = qBlue(pixels[x][y]) + qBlue(pixels[x+1][y])+qBlue(pixels[x][y+1])+qBlue(pixels[x+1][y+1]);
            int b = qGreen(pixels[x][y]) + qGreen(pixels[x+1][y])+qGreen(pixels[x][y+1])+qGreen(pixels[x+1][y+1]);
            int a = qAlpha(pixels[x][y]) + qAlpha(pixels[x+1][y])+qAlpha(pixels[x][y+1])+qAlpha(pixels[x+1][y+1]);
            r/=4;
            g/=4;
            b/=4;
            a/=4;
            QRgb guassianBlurredPixel = qRgba(r,g,b,a);
            
//                    Vector2 index = Vector2(temp.x*2+i,temp.y*2+j);
//                    cout<<"x "<<index.x<<" y "<<index.y<<endl;
//                    cout<<"-x "<<x*2+1+i<<" y "<<y*2+1+j<<endl;
            pixelsTemp[x][y] = guassianBlurredPixel;
            //pixelLocationsTemp[x][y] = Vector;
        }
    }
    
    pixels = pixelsTemp;
//    pixelLocations = pixelLocationsTemp;
    size = sizeTemp;
    
    QSize sizeScaledTemp = QSize(sizeTemp.x,sizeTemp.y);
    
    Qt::AspectRatioMode aspectRatioModePatched = Qt::IgnoreAspectRatio;
    Qt::TransformationMode transformModePatched = Qt::SmoothTransformation;
    //cout<<"here1"<<endl;
 
    image = image.scaled(sizeScaledTemp, aspectRatioModePatched,transformModePatched);
}

//Texture& Texture::operator= (Texture param)
//{
//    QImage image = param.getImage();
//    Texture newTexture = Texture(image);
//    return newTexture;
//}
