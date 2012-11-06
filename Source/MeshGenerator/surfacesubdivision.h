// Author: Richard Pieterse
// Date: October 2012

#ifndef SURFACESUBDIVISION_H
#define SURFACESUBDIVISION_H

#include "mesh.h"
#include <QProgressDialog>

void ApplyLoopSubvision( Mesh* model,  int levels, QProgressDialog *progBar);
void LoopSubdivision( Mesh* model, int levels, QProgressDialog*  progBar);


#endif // SURFACESUBDIVISION_H
