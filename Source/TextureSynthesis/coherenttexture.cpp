#include "coherenttexture.h"
#include "indexedtexture.h"
#include "pixelcoherence.h"
#include <iostream>
#include <math.h>
#include <algorithm>
using namespace std;
//CoherentTexture::CoherentTexture(): Texture()
//{
//    //createCoherence(neighbourhoodSizes);
//}
CoherentTexture::CoherentTexture(): Texture()
{
    createCoherence();
}

CoherentTexture::CoherentTexture(Vector2D dimensions): Texture(dimensions)
{
    createCoherence();
}

CoherentTexture::CoherentTexture(QImage& imageInput): Texture(imageInput)
{
    createCoherence();
}

CoherentTexture::CoherentTexture(Vector2D startPoint,Vector2D dimensions, Texture& patch): Texture(startPoint,dimensions,patch)
{
    createCoherence();
}
CoherentTexture::CoherentTexture(QImage& imageInput,CoherentTexture lowerResolution): Texture(imageInput)
{
    pixelCoherence = new PixelCoherence*[size.xInt()];
    for(int x = 0; x<size.xInt();x++)
    {
        pixelCoherence[x] = new PixelCoherence[size.yInt()];
    }
    
    neighbourhoodSizes = vector<int>();
    
    int halfSize = 4;
    if(size.xInt()/2<halfSize*2+1)
    {
        cout<<"halfSize - "<<halfSize<<endl;
        neighbourhoodSizes.push_back(halfSize);
    }
    else
    {
        for(int i = halfSize*2+1; i<=size.xInt()/2;halfSize*=2,i=halfSize*2+1)
        {
            cout<<"halfSize - "<<halfSize<<endl;
            neighbourhoodSizes.push_back(halfSize);
        }
    }
    
    #pragma omp parallel for num_threads(8) 
    for(int x = 0; x<size.xInt();x++)
    {
        for(int y = 0; y<size.yInt();y++)
        {
            int xHalf = floor(x/2);
            int yHalf = floor(y/2);
            if(floor(x/2)>=lowerResolution.size.x)
            {
                xHalf--;
            }
            if(floor(y/2)>=lowerResolution.size.y)
            {
                yHalf--;
            }
//            cout<<"x/2 - "<<floor(x/2)<<" ";
//            cout<<"y/2 - "<<floor(y/2)<<endl;
//            cout<<"xHalf - "<<xHalf<<" ";
//            cout<<"yHalf - "<<yHalf<<endl;
//            cout<<"x - "<<x<<" ";
//            cout<<"y - "<<y<<endl;
//            cout<<"lowerResolution x - "<<lowerResolution.size.x<<" ";
//            cout<<"lowerResolution y - "<<lowerResolution.size.y<<endl;
//            cout<<"size x - "<<size.x<<" ";
//            cout<<"size y - "<<size.y<<endl;
//            cout<<"neighbourPosDim - "<<lowerResolution.pixelCoherence[xHalf][yHalf].pixelPos.x<<endl;
//            cout<<"neighbourPosDim - "<<lowerResolution.pixelCoherence[xHalf][yHalf].neighbourPosDim<<endl;
            
//            if(lowerResolution.pixelCoherence[x/2][y/2]==NULL)
//            {
//                cout<<"++++++\n++++++\n++++++\n++++++\n++++++\n"<<endl;
//            }
//            else
                pixelCoherence[x][y] = PixelCoherence(*this,lowerResolution.pixelCoherence[xHalf][yHalf],Vector2D(x,y),&neighbourhoodSizes);
        }
    }
}

void CoherentTexture::createCoherence()
{
//    cout<<"++++"<<endl;
    pixelCoherence = new PixelCoherence*[size.xInt()];
    for(int x = 0; x<size.xInt();x++)
    {
        pixelCoherence[x] = new PixelCoherence[size.yInt()];
    }
//    cout<<"++++"<<endl;
    neighbourhoodSizes = vector<int>();
//    cout<<"++++"<<endl;
    int halfSize = 4;
    if(size.xInt()/2<halfSize*2+1)
    {
        cout<<"halfSize - "<<halfSize<<endl;
        neighbourhoodSizes.push_back(halfSize);
    }
    else
    {
        for(int i = halfSize*2+1; i<=size.xInt()/2;halfSize*=2,i=halfSize*2+1)
        {
            cout<<"halfSize - "<<halfSize<<endl;
            neighbourhoodSizes.push_back(halfSize);
        }
    }
//    cout<<"++++"<<endl;
    #pragma omp parallel for num_threads(8) 
    for(int x = 0; x<size.xInt();x++)
    {
        for(int y = 0; y<size.yInt();y++)
        {
            pixelCoherence[x][y] = PixelCoherence(*this,Vector2D(x,y),&neighbourhoodSizes);
        }
    }
}

//void CoherentTexture::findCandidates(Vector2& centrePoint,Vector2& samplePoint,IndexedTexture& otherTexture,Vector2& otherPoint, int neighbourhoodSizeIndex, int coherenceQuality, std::vector<Vector2> & candidateSet)
void CoherentTexture::findCandidates(IndexedTexture& otherTexture,Vector2D& otherPoint, int neighbourhoodSizeIndex, int coherenceQuality, std::vector<Vector2D> & candidateSet)
{
    //the set of 11 closest neighbourhoods, created from the pixels neighbourhood coherence
    //vector<Vector2> candidateSet = vector<Vector2>();
    
    //loop through the neighbourhood

//    cout<< "centrePoint x - "<<centrePoint.x<< " y - "<<centrePoint.y<<endl;
//    cout<< "samplePoint x - "<<samplePoint.x<< " y - "<<samplePoint.y<<endl;
//    cout<< "otherPoint x - "<<otherPoint.x<< " y - "<<otherPoint.y<<endl;
    
    int nSize = neighbourhoodSizes[neighbourhoodSizeIndex];
//    int nSize = 0;
    for(int x = -1;x<=1;x++)
    {
        for(int y = -1; y<=1;y++)
        {
//    for(int x = -nSize;x<=nSize;x++)
//    {
//        for(int y = -nSize; y<=nSize;y++)
//        {
            //the refrence point to the next pixel in the neighbourhood
//            Vector2 referencePoint = centrePoint;
//            Vector2 referencePoint = samplePoint;
            Vector2D referencePoint = otherPoint;
            referencePoint.x +=x;
            referencePoint.y +=y;
            
            //the next point, temporarily holds the coherence point
            Vector2D nextpoint = Vector2D();
//            cout<< "referencePoint "<<referencePoint.x<<" _ "<<referencePoint.y<<endl;
//            cout<< "otherTexture.size "<<otherTexture.size.x<<" _ "<<otherTexture.size.y<<endl;
            //mirror the point if its not in bounds
            referencePoint = otherTexture.wrapNotInBounds(referencePoint);
//            cout<< "referencePoint2 "<<referencePoint.x<<" _ "<<referencePoint.y<<endl;
            //get the sample texture index from the indexedtexture, according to the referencepoint
            Vector2D inputPoint = otherTexture.pixelLocations[referencePoint.xInt()][referencePoint.yInt()];
            
            
            
//            Vector2 offset = otherPoint;
//            offset-= referencePoint;
            
            //loop through each coherent point of the reference point and add them to the candidate list.
            for(int j = 0;j<coherenceQuality;j++)
            {
//                cout<< "j"<<j<<endl;
                //for testing
//                PixelCoherence pixel = pixelCoherence[inputPoint.xInt()][inputPoint.yInt()];
//                    nextpoint = pixelCoherence[referencePoint.xInt()][referencePoint.yInt()].neighbourPos[neighbourhoodSize][j];
//                cout<< "pixel"<<pixel.neighbourPosDim.y<<endl;
                
                //gets the point
                nextpoint = pixelCoherence[inputPoint.xInt()][inputPoint.yInt()].neighbourPos[neighbourhoodSizeIndex][j];
//                cout<< "nextpoint"<<j<<endl;
                
                //offsets the point to be in the relative position from the reference point
//                nextpoint+=offset;
//                cout<< "offset x - "<<offset.x<< " y - "<<offset.y<<endl;
//                nextpoint = mirrorNotInBounds(nextpoint);
                
                nextpoint.x -=x;
                nextpoint.y -=y;
                nextpoint = mirrorNotInBounds(nextpoint);
                
//                        cout<< "nextpoint x - "<<nextpoint.x<< " y - "<<nextpoint.y<<endl;
//                        candidateSet.find_if(candidateSet.begin(), candidateSet.end(),);
//                #pragma omp critical
                {
                    candidateSet.push_back(nextpoint);
                }
            }
            
        }
    }
    
//    candidateSet.erase( unique( candidateSet.begin(), candidateSet.end() ), candidateSet.end());
//    cout<<"created candidate set"<<endl;
    
//    Pixel closest = Pixel();
//    closest.dist = 1000000;
    
//    Vector2 current = Vector2();
//    int currentDist = 1000000;
//    Vector2 testPoint = mirrorNotInBounds(samplePoint);
//    //loop through the candidate set
//    for(int x = 0;x<candidateSet.size();x++)
//    {
        
////        cout<<"x "<<x<<endl;
//        current = candidateSet[x];
        
//        //get the neighbourhood distance
        
//        currentDist = euclideanDistanceSquared(current, *this, testPoint);
        
////        cout<< "current x - "<<current.x<< " y - "<<current.y<<endl;
////        cout<< "testPoint x - "<<testPoint.x<< " y - "<<testPoint.y<<endl;
////        cout<< "currentDist - "<<currentDist<<endl;
////        cout<< "Red 1 - "<<qRed(pixels[current.xInt()][current.yInt()])<< " 2 - "<<qRed(pixels[testPoint.xInt()][testPoint.yInt()])<<endl;
////        cout<< "Green 1 - "<<qGreen(pixels[current.xInt()][current.yInt()])<< " 2 - "<<qGreen(pixels[testPoint.xInt()][testPoint.yInt()])<<endl;
////        cout<< "Blue 1 - "<<qBlue(pixels[current.xInt()][current.yInt()])<< " 2 - "<<qBlue(pixels[testPoint.xInt()][testPoint.yInt()])<<endl;
        
//        //check if its closer than the current model
//        if(currentDist<closest.dist)
//        {
            
            
//            closest.dist = currentDist;
//            closest.pos = current;
//        }
//    }
//    if(otherPoint.x==0&&otherPoint.y==0)
//    {
////        cout<< "testPoint x - "<<testPoint.x<< " y - "<<testPoint.y<<endl;
////        cout<< "closest x - "<<closest.pos.x<< " y - "<<closest.pos.y<<endl;
//    }
//    cout<<"found closest"<<endl;
//    return candidateSet;
}


Pixel CoherentTexture::findClosestNeighbourhood(IndexedTexture& otherTexture,Vector2D& otherPoint, int neighbourhoodSizeIndex, int coherenceQuality)
{
    //the overloaded method for finding the closest neighbourhood with k-coherence
//    cout<<"quality "<<coherenceQuality<<endl;
    //the set of 11 closest neighbourhoods, created from the pixels neighbourhood coherence
    vector<Vector2D> candidateSet = vector<Vector2D>();
//    cout<<"=============================================================="<<endl;
//    cout<<"creating the candidate set"<<endl;
//    cout<< "closest x - "<<otherPoint.x<< " y - "<<otherPoint.y<<endl;
    
    //loop through the neighbourhood
//    for(int x = -1;x<=1;x++)
//    {
//        for(int y = -1; y<=1;y++)
//        {
            //i dont think its necessary to go through the whole neighbourhood, just the imediately surrounding pixels
            
    int nSize = neighbourhoodSizes[neighbourhoodSizeIndex];
    for(int x = -1;x<=1;x++)
    {
        for(int y = -1; y<=1;y++)
        {    
//    for(int x = -nSize;x<=nSize;x++)
//    {
//        for(int y = -nSize; y<=nSize;y++)
//        {            
            
            //the refrence point to the next pixel in the neighbourhood
            Vector2D referencePoint = otherPoint;
            referencePoint.x +=x;
            referencePoint.y +=y;
            
            //the next point 
            Vector2D nextpoint = Vector2D();
            
            if(otherTexture.isInBounds(referencePoint))
            {
//                cout<< "referencePoint x - "<<referencePoint.x<< " y - "<<referencePoint.y<<endl;
                
                //get the sample texture index from the indexedtexture, according to the referencepoint
                Vector2D inputPoint = otherTexture.pixelLocations[referencePoint.xInt()][referencePoint.yInt()];
                
    //            cout<< "inputPoint x - "<<inputPoint.x<< " y - "<<inputPoint.y<<endl;
                
                //loop through each coherent point of the reference point and add them to the candidate list.
                for(int j = 0;j<coherenceQuality;j++)
                {
    //                cout<< "j"<<j<<endl;
                    //for testing
                    PixelCoherence pixel = pixelCoherence[inputPoint.xInt()][inputPoint.yInt()];
//                    nextpoint = pixelCoherence[referencePoint.xInt()][referencePoint.yInt()].neighbourPos[neighbourhoodSize][j];
    //                cout<< "pixel"<<pixel.neighbourPosDim.y<<endl;
                    //gets the point
                    nextpoint = pixelCoherence[inputPoint.xInt()][inputPoint.yInt()].neighbourPos[neighbourhoodSizeIndex][j];
    //                cout<< "nextpoint"<<j<<endl;
                    
                    //offsets the point to be in the relative position from the reference point
                    nextpoint.x -=x;
                    nextpoint.y -=y;
                    if(isInBounds(nextpoint))
                    {
//                        cout<< "nextpoint x - "<<nextpoint.x<< " y - "<<nextpoint.y<<endl;
//                        candidateSet.find_if(candidateSet.begin(), candidateSet.end(),);
                        candidateSet.push_back(nextpoint);
                    }
                }
            }
        }
    }
    
//    candidateSet.erase( unique( candidateSet.begin(), candidateSet.end() ), candidateSet.end());
//    cout<<"created candidate set"<<endl;
    
    Pixel closest = Pixel();
    closest.dist = 1000000;
    
    Vector2D current = Vector2D();
    int currentDist = 0;
    
    //loop through the candidate set
    for(int x = 0;x<candidateSet.size();x++)
    {
        
//        cout<<"x "<<x<<endl;
        current = candidateSet[x];
        
        //get the neighbourhood distance
        currentDist = euclideanDistanceSquaredNeighbourhood(current, otherTexture,otherPoint, neighbourhoodSizes[neighbourhoodSizeIndex]);
//        cout<< "current x - "<<current.x<< " y - "<<current.y<<" ";
//        cout<< "currentDist - "<<currentDist<<endl;
        //check if its closer than the current model
        if(currentDist<closest.dist)
        {
            
            
            closest.dist = currentDist;
            closest.pos.x = current.x;
            closest.pos.y = current.y;
    
        }
    }
//    cout<< "closest x - "<<closest.pos.x<< " y - "<<closest.pos.y<<endl;
    
//    cout<<"found closest"<<endl;
    return closest;
}
