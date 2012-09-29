#include <cmath>
#include <iostream>

#include "BranchSegment.h"

using namespace std;

BranchSegment::BranchSegment(const float * const matrix, float baseRadius, float topRadius, float len) {
	this->baseRadius = baseRadius;
	this->topRadius = topRadius;
	this->len = len;

	for (int i = 0; i < 16; ++i) {
		this->matrix[i] = matrix[i];
	}

	quad = gluNewQuadric();
}

BranchSegment::~BranchSegment() {
	if (quad)
		gluDeleteQuadric(quad);
}

void BranchSegment::draw() {
	glPushMatrix();
	glMultMatrixf(matrix);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quad, baseRadius, topRadius, len, 10, 10);
	glPopMatrix();
}
