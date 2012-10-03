#include "texturesynthesis.h"

#include <QtCore/qmath.h>

#include <vector>
#include <iostream>
#include "pixelcoherence.h"
#include "indexedtexture.h"
using namespace std;
TextureSynthesis::TextureSynthesis()
{
}
void TextureSynthesis::synthesise()
{
    Vector2 v = Vector2(256,256);
    synthesise(v, 10);
}

void TextureSynthesis::synthesise(Vector2 & size, int iterations)
{
    
    //==========================The patch image===================================
    //Properties to copy the sample picture 
    Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio;
    Qt::TransformationMode transformMode = Qt::FastTransformation;
    
    //set the size of the synthesized image
    int sizeSquare = originalTextures[0].size.x;
   
    
    Vector2 sizePatchedImage = size;
    for(int i = 0; i<originalTextures.size()-1; i++)
    {
        sizePatchedImage/=2;
    }
    QSize sizePatchedImageTemp = QSize((int)sizePatchedImage.x,(int)sizePatchedImage.y);
    //the amount of overlap for the random texture
    int overlap = 0;
    
    //the size of each patch in the random texture
    int sizePatchSquare = originalTextures[0].size.x/8;
    Vector2 sizePatches = Vector2(sizePatchSquare,sizePatchSquare);
    QSize sizePatchesTemp = QSize((int)sizePatches.x,(int)sizePatches.y);
    
    
    //copy the image and scale it to the synthesized image size
    QImage patchedImage;
    patchedImage = originalTextures[0].image.scaled(sizePatchedImageTemp, aspectRatioMode,transformMode);
    cout<< "Synthesise"<<endl;
    
    //create an indexed texture from the Qimage
    patchedTexture = new IndexedTexture(patchedImage);
    cout<< "Synthesise"<<endl;
    
    //clear the pixels
    patchedTexture->clearPixelValues();
    cout<< "Synthesise"<<endl;
    
    //create the amount of patches needed
    float numPatchesXTemp = 1 + (float)(sizePatchedImage.x-sizePatches.x) / (float)(sizePatches.x-overlap);
    float numPatchesYTemp = 1 + (float)(sizePatchedImage.y-sizePatches.y) / (float)(sizePatches.y-overlap);
    
    //create a vector for the amount of patches needed
    Vector2 numPatches = Vector2((float)ceil(numPatchesXTemp),(float)ceil(numPatchesYTemp));
    
    //======================Old code================================
    
//    patchList = new Texture[(int)(numPatches.y*numPatches.x*2)];
    
//    for(int y = 0;y<numPatches.y*numPatches.x*2;y++)
//    {
//        Vector2 startingPosition = Vector2();
//        startingPosition.x = rand()%(originalTexture.image.width()- (int)sizePatches.x);
//        startingPosition.y = rand()%(originalTexture.image.height()-(int)sizePatches.y);

//        patchList[y] = Texture(startingPosition,sizePatches,originalTexture);
//    }
    //=====================Old code End================================
    
    //==============================Patch Logic=========================================
    // An array of all the patches
    patchListIndexed = new IndexedTexture[(int)(numPatches.y*numPatches.x*2)];
    
    //create twice the amount of needed patches, this can be taken down to the required amount
    for(int y = 0;y<numPatches.y*numPatches.x*2;y++)
    {
        Vector2 startingPosition = Vector2();
        startingPosition.x = rand()%(originalTextures[0].image.width()- (int)sizePatches.x);
        startingPosition.y = rand()%(originalTextures[0].image.height()-(int)sizePatches.y);

        patchListIndexed[y] = IndexedTexture(startingPosition,sizePatches,originalTextures[originalTextures.size()-1]);
    }

    cout<<"createRandomTexture======"<<endl;
    //create a random texture to be iterated over
    *patchedTexture = createRandomTexture(*patchedTexture, patchListIndexed,numPatches);
    cout<<"createdRandomTexture======"<<endl;
    
    
    //size of the previews
    Vector2 sizeScaledPatched = Vector2(128,128);
    cout<<"seze======"<<endl;
    //optimization and display preview of textures
    
//    cout<<"set preview======"<<endl;
//    scaleAndDisplayImage(sizeScaledPatched,patchedTexture, ui->label_4);
    
    cout<<"discreetOptimization======"<<endl;
    for(int s = originalTextures[0].neighbourhoodSizes.size()-1;s>=0;s--)
    {
        cout<< "s "<<s <<endl;
        cout<< "size "<<originalTextures[0].neighbourhoodSizes[s] <<endl;
        *patchedTexture = discreetOptimization2(*patchedTexture,originalTextures[0],iterations,s);
    }
    
    for(int i = 1; i<originalTextures.size();i++)
    {
        cout<<"discreetOptimization Starting====== "<<i<<endl;
        patchedTexture->doubleSize(originalTextures[i]);
        for(int s = originalTextures[i].neighbourhoodSizes.size()-1;s>=0;s--)
        {
            cout<< "s "<<s <<endl;
            cout<< "size "<<originalTextures[i].neighbourhoodSizes[s] <<endl;
            
            *patchedTexture = discreetOptimization2(*patchedTexture,originalTextures[i],iterations,s);
        }
        cout<<"discreetOptimization Finished======"<<endl;
    }
    
//    scaleAndDisplayImage(sizeScaledPatched,patchedTexture, ui->label_8);
    
}

void TextureSynthesis::loadImage(QString path)
{
    originalImage;
    originalTextures.clear();
    imagePath = path;

    originalImage.load(imagePath);

    Qt::ImageConversionFlags flags = Qt::AutoColor;
    QImage::Format f = QImage::Format_ARGB32;
    originalImage = originalImage.convertToFormat(f,flags);
    
    Vector2 currentSize = Vector2(originalImage.size().width(),originalImage.size().height());
    
    vector<QImage> imageSizes;
    
//    Texture first = Texture(originalImage);
    imageSizes.push_back(originalImage);
    
    
//    cout<<"<<<<<START "<<endl;
    //if the image is > 40x40 pixels in size then make low res copies of the images until there is one thats smaller
    //each copy is half the size of one lower in the heirarchy
//    Texture tempImage = first;
    
    while(currentSize.x>40 && currentSize.y>40)
    {
        currentSize.x = ceil(currentSize.x/2);
        currentSize.y = ceil(currentSize.y/2);
//        cout<<"<<<<<START "<<endl;
//        first.halfSize();
//        cout<<"<<<<<FIN "<<endl;
//        QImage tempImage = first.getImage();
        QImage tempImage = scaleImage(currentSize,originalImage);
        imageSizes.push_back(tempImage);
    }
    
//    cout<<"<<<<<START "<<endl;
    
    //loop through all the resolutions and create a k-coherence map for it
    for(int i = imageSizes.size()-1; i>=0;i--)
    {
        cout<<"<<<<<START "<<i<<endl;
        QImage current = imageSizes[i];
        CoherentTexture coherentTexture;
        cout<<"<<<<< "<<i<<endl;
        //the first one is an exhaustive search
        if(i==imageSizes.size()-1)
        {
           cout<<"==== "<<current.width()<<" "<<current.height()<<endl;
           coherentTexture = CoherentTexture(current);
           cout<<"====end "<<current.width()<<" "<<current.height()<<endl;
        }
        //the next uses the information stored in the previous example
        else
        {
           cout<<"--- "<<current.width()<<" "<<current.height()<<endl;
           coherentTexture = CoherentTexture(current,originalTextures.back());
           cout<<"---end "<<current.width()<<" "<<current.height()<<endl;
        }
        originalTextures.push_back(coherentTexture);
        cout<<"<<<<<END "<<i<<endl;
    }
    
    //create a coherent texture that stores the coherence between pixels, this is parallelizable
//    originalTextures.push_back(CoherentTexture(originalImage,neighbourhoodSizes));
    
//    PixelCoherence pc = originalTextures[0].pixelCoherence[21][37];
        
//    for(int i = 0; i<11;i++)
//    {
////        cout<<pc.neighbourPos[i].x<<"  -  "<<pc.neighbourPos[i].y<<endl;
        
//    }
    
//    pc = originalTextures[0].pixelCoherence[36][12];
    
//    for(int i = 0; i<11;i++)
//    {
        
////        cout<<pc.neighbourPos[i].x<<"  -  "<<pc.neighbourPos[i].y<<endl;
        
//    }
    cout << "Pyramid size  -  " << originalTextures.size() << endl;
    
    
}

QImage TextureSynthesis::scaleImage(Vector2 & sizeScaled, QImage & image)
{
    
    //==============================Display Logic=========================================

   
   QSize sizeScaledTemp = QSize(sizeScaled.x,sizeScaled.y);
   
   Qt::AspectRatioMode aspectRatioModePatched = Qt::IgnoreAspectRatio;
   Qt::TransformationMode transformModePatched = Qt::SmoothTransformation;
   //cout<<"here1"<<endl;

   QImage scaled = image.scaled(sizeScaledTemp, aspectRatioModePatched,transformModePatched);
   //cout<<"here2"<<endl;

   return scaled;
}

//blends between pixels according to the ratio provided
QRgb TextureSynthesis::featheredPixel(QRgb& patchedPixel, QRgb& patchPixel, float ratio)
{
//    cout<< "red "<<(qRed(patchedPixel)*ratio)<<endl;
//    cout<< "red2 "<<(qRed(patchPixel)*(1-ratio))<<endl;
//    cout<< "red3 "<< (int)(qRed(patchedPixel)*ratio + qRed(patchPixel)*(1-ratio))<<endl;
    
    int red = (int)(qRed(patchedPixel)*ratio + qRed(patchPixel)*(1-ratio));
    int green = (int)(qGreen(patchedPixel)*ratio + qGreen(patchPixel)*(1-ratio));
    int blue = (int)(qBlue(patchedPixel)*ratio + qBlue(patchPixel)*(1-ratio));
    int alpha = (int)(qAlpha(patchedPixel)*ratio + qAlpha(patchPixel)*(1-ratio));
    
    return qRgba(red, green, blue, alpha);
}


//This is the logic for the discreetOptimization, similar to Texture optimization but using the k-coherence algorithm
IndexedTexture TextureSynthesis::discreetOptimization(IndexedTexture & patchedTexture, CoherentTexture& sampleTexture, int iterations, int neighbourhoodSizeIndex)
{
    
    //number of iterations
    QImage image = patchedTexture.getImage();
    IndexedTexture oldTexture = IndexedTexture(patchedTexture);
    int oldDist = 0;
    IndexedTexture newTexture = IndexedTexture(patchedTexture);
    int newDist = 0;
//    cout<< "discreetOptimization1"<<endl;
    newTexture.clearPixelValues();
    //width of the surround pixel neighbourhood -- A value of 2 corresponds to a 5x5 matrix 
    int neighbourhoodSize = sampleTexture.neighbourhoodSizes[neighbourhoodSizeIndex];
//    cout<< "discreetOptimization2"<<endl;
    
    Vector2 sizeScaledPatched = Vector2(128,128);
    for(int n = 0; n <iterations; n++)
    {
        newDist = 0;
//        cout<< "discreetOptimization iteration"<< n <<endl;
        newTexture.clearPixelValues();
        Pixel closest = Pixel();
        Vector2 current = Vector2();
        
        //this loops through each pixel in the neighbourhood of the pixel being synthesized
        //atm there is a border of pixels not being synthsized, should implement wrapping or mirroring of pixels
        int min = 0;
        int xmax = oldTexture.size.x;
        int ymax = oldTexture.size.y;
//        cout<< "ymax x - "<<xmax<< " y - "<<ymax<<endl;
        
        //#pragma omp parallel for shared(newTexture, oldTexture, sampleTexture) num_threads(8)
        #pragma omp parallel for private(closest,current) reduction(+:newDist) num_threads(8) 
        for(int x = min;x<xmax ;x++)
        {
            for(int y = min; y< ymax;y++)
            {
                current.x = x;
                current.y = y;
                
//                cout<< "current x - "<<current.x<< " y - "<<current.y<<endl;
                
                //find the closest neighbourhood according to the coherence set created
                closest = sampleTexture.findClosestNeighbourhood(oldTexture, current, neighbourhoodSizeIndex, quality);
//                cout<< "Found closest"<<endl;
                
                //set the pixels value and pixel location according to the neighbourhood
                
                newTexture.pixels[(int)current.x][(int)current.y] = sampleTexture.pixels[(int)closest.pos.x][(int)closest.pos.y];
                
                newTexture.pixelLocations[(int)current.x][(int)current.y] = closest.pos;
                
                if(x==0&&y==0)
                {
                    cout<< "closest x - "<<closest.pos.x<< " y - "<<closest.pos.y<<endl;
                    cout<< "pixelLocations x - "<<newTexture.pixelLocations[(int)current.x][(int)current.y].x<< " y - "<<newTexture.pixelLocations[(int)current.x][(int)current.y].y<<endl;
                    cout<< "Red - "<<qRed(sampleTexture.pixels[(int)closest.pos.x][(int)closest.pos.y])<< " closest - "<<qRed(oldTexture.pixels[(int)current.x][(int)current.y])<<endl;
                    cout<< "Blue - "<<qBlue(sampleTexture.pixels[(int)closest.pos.x][(int)closest.pos.y])<< " closest - "<<qBlue(oldTexture.pixels[(int)current.x][(int)current.y])<<endl;
                    cout<< "Green - "<<qGreen(sampleTexture.pixels[(int)closest.pos.x][(int)closest.pos.y])<< " closest - "<<qGreen(oldTexture.pixels[(int)current.x][(int)current.y])<<endl;
                }
//                cout<< "Found closest2"<<endl;
                newDist += closest.dist;
                
                
            }
            
        }
        newDist/=xmax*ymax;
//        cout<<"old -  "<<oldDist<<" new -  "<<newDist<<endl;
        
        if(oldDist==newDist)
        {
//            cout<<"FOUND old -  "<<oldDist<<" new -  "<<newDist<<endl;
//            break;
        }
        
        oldDist = newDist;
        //set the old texture to be the new texture for iteration
        oldTexture = IndexedTexture(newTexture);
//        if(n==1)
//        {
//            scaleAndDisplayImage(sizeScaledPatched,oldTexture, ui->label_5);
//        }
//        else if(n==2)
//        {
//            scaleAndDisplayImage(sizeScaledPatched,oldTexture, ui->label_6);
//        }
//        else if(n==3)
//        {
//            scaleAndDisplayImage(sizeScaledPatched,oldTexture, ui->label_7);
//        }
        
//        scaleAndDisplayImage(sizeScaledPatched,oldTexture, ui->label_2);
//        #pragma omp parallel
//        {
//            Vector2 sizeScaledPatched = Vector2(128,128);
//            scaleAndDisplayImage(sizeScaledPatched,oldTexture, ui->label_6);
//        }
    }
    return oldTexture;
}


IndexedTexture TextureSynthesis::discreetOptimization2(IndexedTexture & patchedTexture, CoherentTexture& sampleTexture, int iterations, int neighbourhoodSizeIndex)
{
    
    //number of iterations
    QImage image = patchedTexture.getImage();
    IndexedTexture oldTexture = IndexedTexture(patchedTexture);
    int oldDist = 0;
    IndexedTexture newTexture = IndexedTexture(patchedTexture);
    int newDist = 0;
//    cout<< "discreetOptimization1"<<endl;
    newTexture.clearPixelValues();
    //width of the surround pixel neighbourhood -- A value of 2 corresponds to a 5x5 matrix 
    int neighbourhoodSize = sampleTexture.neighbourhoodSizes[neighbourhoodSizeIndex];
//    cout<< "discreetOptimization2"<<endl;
    
    //hold the current best distance for each pixel
    float** distance = new float*[oldTexture.size.xInt()];
    for(int x = 0; x<oldTexture.size.x;x++)
    {
        distance[x] = new float[oldTexture.size.yInt()];
    }
    
    
    Vector2 sizeScaledPatched = Vector2(128,128);
    for(int n = 0; n <iterations; n++)
    {
        for(int x = 0; x<oldTexture.size.x;x++)
        {
            for(int y = 0; y<oldTexture.size.y;y++)
            {
                distance[x][y] = 100000;
            }
        }
        
        
        newDist = 0;
//        cout<< "discreetOptimization iteration"<< n <<endl;
        newTexture.clearPixelValues();
        Pixel closest = Pixel();
        Vector2 current = Vector2();
        
        //this loops through each pixel in the neighbourhood of the pixel being synthesized
        //atm there is a border of pixels not being synthsized, should implement wrapping or mirroring of pixels
        int min =(neighbourhoodSize*2+1)/2;
        int xmax = oldTexture.size.x;
        int ymax = oldTexture.size.y;
        int iterationDistance = (neighbourhoodSize*2+1)/4;
//        cout<< "ymax x - "<<xmax<< " y - "<<ymax<<endl;
        
        //#pragma omp parallel for shared(newTexture, oldTexture, sampleTexture) num_threads(8)
//        #pragma omp parallel for private(closest,current) reduction(+:newDist) num_threads(8) 
        for(int x = min;x<xmax ;x+=iterationDistance)
        {
            for(int y = min; y< ymax;y+=iterationDistance)
            {
                current.x = x;
                current.y = y;
                
//                cout<< "current x - "<<current.x<< " y - "<<current.y<<endl;
                
                //find the closest neighbourhood according to the coherence set created
                closest = sampleTexture.findClosestNeighbourhood(oldTexture, current, neighbourhoodSizeIndex, quality);
//                cout<< "Found closest"<<endl;
//                cout<< "closest x - "<<closest.pos.x<< " y - "<<closest.pos.y<<endl;
                //set the pixels value and pixel location according to the neighbourhood
                
//                newTexture.pixels[(int)current.x][(int)current.y] = sampleTexture.pixels[(int)closest.pos.x][(int)closest.pos.y];
                
//                newTexture.pixelLocations[(int)current.x][(int)current.y] = closest.pos;
                
                Pixel nextClosest = Pixel();
                //loop through the neighbourhood
                for(int i = -neighbourhoodSize; i < neighbourhoodSize ;i++)
                {
                    for(int j = -neighbourhoodSize; j < neighbourhoodSize;j++)
                    {
                        //the centre of the closest matching neighbourhood
                        Vector2 closestTemp = closest.pos;
                        closestTemp.x +=i;
                        closestTemp.y +=j;
                        
                        //the current position in the neighbourhood (of oldTexture)
                        Vector2 currentTemp = current;
                        currentTemp.x +=i;
                        currentTemp.y +=j;
                        
                        Vector2 samplePos = Vector2(i,j);
                        
                        //oldTexture = oldTexture.wrapNotInBounds(currentTemp, oldTexture)
                        
                        if(oldTexture.isInBounds(currentTemp))
                        {
                            //find point that minimizes the energy from neighbourhood pixels
                            nextClosest = sampleTexture.findClosestPoint(closest.pos,closestTemp,oldTexture,current,neighbourhoodSizeIndex,quality);
                            
                            
                            if(nextClosest.dist<distance[currentTemp.xInt()][currentTemp.yInt()])
                            {
                                #pragma omp critical
                                {
//                                cout<< "nextClosest x - "<<nextClosest.pos.x<< " y - "<<nextClosest.pos.y<<endl;
//                                cout<< "currentTemp x - "<<currentTemp.x<< " y - "<<currentTemp.y<<endl;
                                    distance[currentTemp.xInt()][currentTemp.yInt()] = nextClosest.dist;
                                    
                                    newTexture.pixels[currentTemp.xInt()][currentTemp.yInt()] = sampleTexture.pixels[nextClosest.pos.xInt()][nextClosest.pos.yInt()];
                                    
                                    newTexture.pixelLocations[currentTemp.xInt()][currentTemp.yInt()] = nextClosest.pos;
                                    
//                                    int pp = neighbourhoodSize*2+1;
                                    
//                                    newTexture.pixels[currentTemp.xInt()+pp][currentTemp.yInt()+pp] = sampleTexture.pixels[closestTemp.xInt()][closestTemp.yInt()];
                                    
//                                    newTexture.pixelLocations[currentTemp.xInt()+pp][currentTemp.yInt()+pp] = closestTemp;
                                    
                                    
                                }
                            }
                            
                            
                        }
                    }
                }
//                if(x==0&&y==0)
//                {
//                    cout<< "closest x - "<<closest.pos.x<< " y - "<<closest.pos.y<<endl;
//                    cout<< "pixelLocations x - "<<newTexture.pixelLocations[(int)current.x][(int)current.y].x<< " y - "<<newTexture.pixelLocations[(int)current.x][(int)current.y].y<<endl;
//                    cout<< "Red - "<<qRed(sampleTexture.pixels[(int)closest.pos.x][(int)closest.pos.y])<< " closest - "<<qRed(oldTexture.pixels[(int)current.x][(int)current.y])<<endl;
//                    cout<< "Blue - "<<qBlue(sampleTexture.pixels[(int)closest.pos.x][(int)closest.pos.y])<< " closest - "<<qBlue(oldTexture.pixels[(int)current.x][(int)current.y])<<endl;
//                    cout<< "Green - "<<qGreen(sampleTexture.pixels[(int)closest.pos.x][(int)closest.pos.y])<< " closest - "<<qGreen(oldTexture.pixels[(int)current.x][(int)current.y])<<endl;
//                }
//                cout<< "Found closest2"<<endl;
                newDist += closest.dist;
                
//                break;
                
            }
//            break;
        }
        newDist/=xmax*ymax;
//        cout<<"old -  "<<oldDist<<" new -  "<<newDist<<endl;
        
        if(oldDist==newDist)
        {
//            cout<<"FOUND old -  "<<oldDist<<" new -  "<<newDist<<endl;
            break;
        }
        
        oldDist = newDist;
        //set the old texture to be the new texture for iteration
        oldTexture = IndexedTexture(newTexture);
//        if(n==1)
//        {
//            scaleAndDisplayImage(sizeScaledPatched,oldTexture, ui->label_5);
//        }
//        else if(n==2)
//        {
//            scaleAndDisplayImage(sizeScaledPatched,oldTexture, ui->label_6);
//        }
//        else if(n==3)
//        {
//            scaleAndDisplayImage(sizeScaledPatched,oldTexture, ui->label_7);
//        }
        
//        scaleAndDisplayImage(sizeScaledPatched,oldTexture, ui->label_2);
//        #pragma omp parallel
//        {
//            Vector2 sizeScaledPatched = Vector2(128,128);
//            scaleAndDisplayImage(sizeScaledPatched,oldTexture, ui->label_6);
//        }
    }
    return oldTexture;
}
//creates a random texture from patches with each pixels location
IndexedTexture TextureSynthesis::createRandomTexture(IndexedTexture & patchedTexture,  IndexedTexture* & patchList, Vector2& numPatches)
{
     //loop through all the patches
    for(int x = 0;x<numPatches.x;x++)
    {
        for(int y = 0;y<numPatches.y;y++)
        {
            //select a random patch to place
            //cout<< "x - "<<x<< " y - "<<y<<endl;
            int randomPatch = rand()%((int)(numPatches.x*numPatches.y*2));
            IndexedTexture patchDetails = patchList[randomPatch];
//            cout<<"Patch x - "<<patchDetails.pixelLocations[0][0].x<<"y - "<<patchDetails.pixelLocations[0][0].y<<endl;   
               
            //place the patch in the texture
            for(int i =0 ;i<patchDetails.size.x;i++)
            {
                for(int j =0 ;j<patchDetails.size.y;j++)
                {
                    Vector2 pixelPosition = Vector2(x*patchDetails.size.x+i,y*patchDetails.size.y+j);
                    
                    if(patchedTexture.isInBounds(pixelPosition))
                    {
                        patchedTexture.pixels[pixelPosition.xInt()][(int)pixelPosition.y] = patchDetails.pixels[i][j];
                        patchedTexture.pixelLocations[pixelPosition.xInt()][(int)pixelPosition.y] = patchDetails.pixelLocations[i][j];
                        
                    }
                }
            }
        }
    }
    return patchedTexture;
}



Texture TextureSynthesis::textureOptimization(Texture & patchedTexture, Texture& sampleTexture, int iterations)
{
    //number of iterations
    QImage image = patchedTexture.getImage();
    Texture oldTexture = Texture(image);
    
    Texture newTexture = Texture(image);
    
    newTexture.clearPixelValues();
    //width of the surround pixel neighbourhood -- A value of 2 corresponds to a 5x5 matrix 
    int neighbourhoodSize = 2;
    int neighbourhoodIncrement = (int)(neighbourhoodSize*2+1)/4;
    
    for(int n = 0; n <iterations; n++)
    {
        newTexture.clearPixelValues();
        Vector2 closest = Vector2();
        Vector2 current = Vector2();
        
        for(int x = neighbourhoodSize;x< oldTexture.size.x-neighbourhoodSize;x+=neighbourhoodIncrement)
        {
            for(int y = neighbourhoodSize; y<oldTexture.size.y-neighbourhoodSize;y+=neighbourhoodIncrement)
            {
                current.x = x;
                current.y = y;
//                cout<< "current x - "<<current.x<< " y - "<<current.y<<endl;
                closest = sampleTexture.findClosestNeighbourhood(oldTexture, current, neighbourhoodSize);
                Vector2 posPatch = Vector2();
                Vector2 posSample = Vector2();
//                cout<< "closest x - "<<closest.x<< " y - "<<closest.y<<endl;
                for(int i = -neighbourhoodSize;i<=neighbourhoodSize;i++)
                {
                    for(int j = -neighbourhoodSize; j<=neighbourhoodSize;j++)
                    {
                        posSample.x = closest.x + i;
                        posSample.y = closest.y + j;
                        posPatch.x = x+i;
                        posPatch.y = y+j;
//                        cout<< "posSample x - "<<posSample.x<< " y - "<<posSample.y<<endl;
//                        cout<< "posPatch x - "<<posPatch.x<< " y - "<<posPatch.y<<endl;
                        if(oldTexture.isInBounds(posPatch) && sampleTexture.isInBounds(posSample))
                        {
                            int red = qRed(newTexture.pixels[(int)posPatch.x][(int)posPatch.y]);
                            int blue = qBlue(newTexture.pixels[(int)posPatch.x][(int)posPatch.y]);
                            int green = qGreen(newTexture.pixels[(int)posPatch.x][(int)posPatch.y]);
                            int alpha = qAlpha(newTexture.pixels[(int)posPatch.x][(int)posPatch.y]);
                            
                            if(red == 0 && blue == 0 && green == 0 && alpha == 0)
                            {
                                newTexture.pixels[(int)posPatch.x][(int)posPatch.y] = sampleTexture.pixels[(int)posSample.x][(int)posSample.y];
                            }
                            else
                            {
                                newTexture.pixels[(int)posPatch.x][(int)posPatch.y] = featheredPixel(newTexture.pixels[(int)posPatch.x][(int)posPatch.y],sampleTexture.pixels[(int)posSample.x][(int)posSample.y],0.5f);
                            }
                        }
                    }
                }
            }
        }
        
        oldTexture = Texture(newTexture);
    }
    return oldTexture;
}

//creates a random texture from patches
Texture TextureSynthesis::createRandomTexture(Texture & patchedTexture,  Texture* & patchList, Vector2& numPatches,int overlap)
{
    //loop through all the patches
    for(int x = 0;x<numPatches.x;x++)
    {
        for(int y = 0;y<numPatches.y;y++)
        {
            //select a random patch to place
            int randomPatch = rand()%((int)(numPatches.x*numPatches.y*2));
            Texture patchDetails = patchList[randomPatch];
            
            //place the patch in the texture
            for(int i =0 ;i<patchDetails.size.x;i++)
            {
                for(int j =0 ;j<patchDetails.size.y;j++)
                {
                    Vector2 pixelPosition = Vector2(x*patchDetails.size.x+i,y*patchDetails.size.y+j);
                    
                    if(patchedTexture.isInBounds(pixelPosition))
                    {
                        patchedTexture.pixels[(int)pixelPosition.x][(int)pixelPosition.y] = patchDetails.pixels[i][j];
                        
                    }
                }
            }
        }
    }
    return patchedTexture;
}

