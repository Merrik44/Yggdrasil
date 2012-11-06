#include "pixelcoherence.h"
#include <queue>
#include <iostream>
#include <math.h>
#include <omp.h>
#include <QApplication>
using namespace std;

PixelCoherence::PixelCoherence()
{
    neighbourPosDim = 11;
    neighbourPos = new Vector2D*[1];
    neighbourPos[0] = new Vector2D[neighbourPosDim];
    
}
PixelCoherence::PixelCoherence(CoherentTexture& sampleTexture, Vector2D pos, std::vector<int>* neighbourhood_Sizes)
{
    
    //---------------------------------creates an array of 11 coherent pixels for each pixel for each neighbourhood size--------------------------------
    neighbourhoodSizes = neighbourhood_Sizes;
    neighbourPosDim = 11;
    neighbourPos = new Vector2D*[neighbourhoodSizes->size()];
    for(int x = 0; x<neighbourhoodSizes->size();x++)
    {
        neighbourPos[x] = new Vector2D[neighbourPosDim];
    }
    //---------------------------------end---------------------------------
    //the pixels position
    pixelPos = pos;
    
    //cout<<"-"<<pos.x<<"  -  "<<pos.y<<endl;
    
    //loop through each neighbourhood size
    for(int i =0;i<neighbourhoodSizes->size();i++)
    {
        priority_queue<Pixel, vector<Pixel>, ComparePixels> pq;
        //cout<<" "<<i<<endl;
        
        //increment according to an the size of the neighbourhood
        int incrementValue = ceil((float)((*neighbourhoodSizes)[i])/4.0f);
        incrementValue = 1;
        int start = 0;
        int end = sampleTexture.size.x;
        int endY = sampleTexture.size.y;
        
        //find the neighbourhood distance to each pixel
        //#pragma omp parallel for num_threads(8) 
        for(int x = start;x<end;x+=incrementValue)
        {
            for(int y = start;y<endY;y+=incrementValue)
            {
        //                cout<<"-"<<x<<"  -  "<<y<<endl;
                int dist = sampleTexture.euclideanDistanceSquaredNeighbourhood(pixelPos, sampleTexture, Vector2D(x,y),(*neighbourhoodSizes)[i]);
        //                cout<<"-"<<x<<"  -  "<<y<<endl;
                Pixel p = Pixel(Vector2D(x,y),dist);
                pq.push(p);
            }
        }
        
        //cout<<"Pixelpos- "<<pixelPos.x<<"  -  "<<pixelPos.y<<endl;
        for(int t = 0; t<neighbourPosDim;t++)
        {
//            cout<<"dist- "<<((Pixel)pq.top()).dist<<endl;
            neighbourPos[i][t] = ((Pixel)pq.top()).pos;
            //cout<<" pos- "<<neighbourPos[t].x<<"  -  "<<neighbourPos[t].y<<endl;
            pq.pop();
        }
    }
}

PixelCoherence::PixelCoherence(CoherentTexture& sampleTexture, PixelCoherence& lowResCoherence,Vector2D pos, std::vector<int>* neighbourhood_Sizes)
{
    //---------------------------------creates an array of 11 coherent pixels for each pixel for each neighbourhood size--------------------------------
    neighbourhoodSizes = neighbourhood_Sizes;
    neighbourPosDim = 11;
    neighbourPos = new Vector2D*[neighbourhoodSizes->size()];
    for(int x = 0; x<neighbourhoodSizes->size();x++)
    {
        neighbourPos[x] = new Vector2D[neighbourPosDim];
    }
    
    //---------------------------------end---------------------------------
    pixelPos = pos;
//    cout<<"=============================================================="<<endl;
//    cout<<pos.x<<"  -  "<<pos.y<<endl;
//    for(int i =0;i<neighbourPosDim;i++)
//    {
        priority_queue<Pixel, vector<Pixel>, ComparePixels> pq;
//        cout<<" "<<i<<endl;
        
        //increment according to an the size of the neighbourhood
        
        //find the neighbourhood distance to each pixel
        //#pragma omp parallel for num_threads(8)
//        cout<<"+-  "<<endl;
//        cout<<"+-  "<<lowResCoherence.neighbourPosDim<<endl;
        
        for(int i =0;i<neighbourhoodSizes->size();i++)
        {
//            cout<<"  1-  "<<i<<endl;
            int index = i-1;
            
            if(index<0)
                index = 0;
//            cout<<"  2-  "<<i<<endl;
//            cout<<"  2-  "<<lowResCoherence.neighbourPosDim<<endl;
//            cout<<"  22-  "<<i<<endl;
            for(int t = 0;t<lowResCoherence.neighbourPosDim;t++)
            {
//                cout<<"  3-  "<<i<<endl;
    //            if(pos.x ==0&&pos.y ==0)
    //                cout<<"low "<<lowResCoherence.neighbourPos[i].x<<"  -  "<<lowResCoherence.neighbourPos[i].y<<endl;
                Vector2D coherencePoint = lowResCoherence.neighbourPos[index][t];
//                cout<<"  =  "<<i<<endl;
                for(int x = 0;x<2;x++)
                {
                    for(int y = 0;y<2;y++)
                    {
                        Vector2D closestPoint = coherencePoint;
                        closestPoint*=2;
                        closestPoint.x +=x;
                        closestPoint.y +=y;
                        
//                        if(pixelPos.x==0&&pixelPos.y==0)
//                        {
//                            cout<<"coherencePoint x - "<<coherencePoint.x<<" y - "<<coherencePoint.y<<endl;
//                            cout<<"closestPoint x - "<<closestPoint.x<<" y - "<<closestPoint.y<<endl;
//                        }
                        if(sampleTexture.isInBounds(closestPoint))
                        {
                            int dist = sampleTexture.euclideanDistanceSquaredNeighbourhood(pixelPos, sampleTexture, closestPoint,(*neighbourhoodSizes)[i]);
                            Pixel p = Pixel(closestPoint,dist);
        //                    cout<<"-"<<p.pos.x<<"  -  "<<p.pos.y<<endl;
                            pq.push(p);
                        }
                        else
                        {
//                            cout<<"NotInBounds PixelCoherence - x - "<< closestPoint.x<<" y - "<<closestPoint.y<<endl;
                        }
                    }
                }
            }
            for(int t = 0; t<neighbourPosDim;t++)
            {
                neighbourPos[i][t] = ((Pixel)pq.top()).pos;
    //            if(pos.x ==0&&pos.y ==0)
    //                cout<<"High "<<neighbourPos[t].x<<"  -  "<<neighbourPos[t].y<<endl;
                pq.pop();
            }
        }        
//    }
//        cout<<"----------------------------------------------------------"<<endl;
//        cout<<pos.x<<"  -  "<<pos.y<<endl;
}
