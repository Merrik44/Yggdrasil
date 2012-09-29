#include "widthchanges.h"

WidthChanges::WidthChanges()
{
}

void WidthChanges::setVariation(float startMin, float startMax, float endMin, float endMax)
{
    variationStartMin = startMin;
    variationStartMax = startMax;
    variationEndMin = endMin;
    variationEndMax = endMax;
}

float WidthChanges::getVariation(QString varType)
{
    if (varType.compare("smin") == 0)
        return variationStartMin;
    else if (varType.compare("smax") == 0)
        return variationStartMax;
    else if (varType.compare("emin") == 0)
        return variationEndMin;
    else
        return variationEndMax;
}
