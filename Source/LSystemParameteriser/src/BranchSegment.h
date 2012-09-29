#ifndef BRANCHSEGMENT_H_
#define BRANCHSEGMENT_H_

#include <utility>

class BranchSegment {
public:
	BranchSegment();
	virtual ~BranchSegment();

	bool operator==(const BranchSegment &rhs) const;
	bool operator!=(const BranchSegment&rhs) const;

	// Min/max angle from parent
	std::pair<float, float> inclination;
	std::pair<float, float> azimuth;

	// Min/max radius of branch
	std::pair<float, float> startRadius;
	std::pair<float, float> endRadius;

	// Min/max length
	std::pair<float, float> length;
};

#endif /* BRANCHSEGMENT_H_ */
