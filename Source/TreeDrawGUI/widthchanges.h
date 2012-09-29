#ifndef WIDTHCHANGES_H
#define WIDTHCHANGES_H
#include<QtGui>

class WidthChanges
{
public:
    WidthChanges();
    void setVariation(float startMin, float startMax, float endMin, float endMax);
    float getVariation(QString varType);

private:
    float variationStartMin;
    float variationStartMax;
    float variationEndMin;
    float variationEndMax;

};

#endif // WIDTHCHANGES_H
