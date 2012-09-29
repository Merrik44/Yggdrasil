#ifndef JOINTGENERATOR_H
#define JOINTGENERATOR_H

#include "mesh.h"
#include "vector"

void GenerateJoint( std::vector< std::vector<Vertex*>* >& branches, Vector3f center, Mesh* model);
void SortByLength( std::vector< Edge*>& edges );

#endif // JOINTGENERATOR_H
