#ifndef BRANCH_SEGMENT_H_
#define BRANCH_SEGMENT_H_

#include <GL/glut.h>

class BranchSegment {
public:
	BranchSegment(const float * const matrix, float baseRadius, float topRadius, float len);
	virtual ~BranchSegment();

	void draw();
private:
	float matrix[16];
	float baseRadius;
	float topRadius;
	float len;

	GLUquadric *quad;
};

#endif /* BRANCH_H_ */
