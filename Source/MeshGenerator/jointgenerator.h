#ifndef JOINTGENERATOR_H
#define JOINTGENERATOR_H

#include "mesh.h"
#include "vector"

Mesh *GenerateJoint( std::vector< std::vector<Vertex*>* >& branches, std::vector<Face *> &incomingBranchFaces, std::vector< Vertex* >& otherVertices, Vector3f center);
void SortByLength( std::vector< Edge*>& edges );

#endif // JOINTGENERATOR_H
