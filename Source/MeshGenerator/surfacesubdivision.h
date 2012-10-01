#ifndef SURFACESUBDIVISION_H
#define SURFACESUBDIVISION_H

#include "mesh.h"
#include <QProgressDialog>

class SurfaceSubdivision
{
public:
    SurfaceSubdivision();
};



void ApplyLoopSubvision( Mesh* model,  int levels, QProgressDialog *progBar);


#endif // SURFACESUBDIVISION_H
