// Author: Richard Pieterse
// Date: October 2012

#ifndef MESHGENERATOR_H
#define MESHGENERATOR_H

#include "lstskeleton.h"
#include "mesh.h"
#include "branchnode.h"
#include "vector"
#include <QProgressDialog>


Mesh* generateMesh( std::vector< BranchNode*>& branches,  QProgressDialog* progressBar = NULL);
Vertex* mergeVertices( Vertex* A, Vertex* B, std::vector<Face *> incomingBranchFace, int bWeight );

#endif // MESHGENERATOR_H
