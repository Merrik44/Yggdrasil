// Author: Richard Pieterse
// Date: October 2012

#ifndef JOINTGENERATOR_H
#define JOINTGENERATOR_H

#include "mesh.h"
#include "vector"

void SortByLength( std::vector< Edge*>& edges );
Mesh *GenerateJoint( std::vector< std::vector<Vertex*>* >& branches, std::vector<Face *> &incomingBranchFaces, std::vector<Vector3f> &brancheDirections);

#endif // JOINTGENERATOR_H
