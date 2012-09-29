#ifndef BRANCH_H_
#define BRANCH_H_

#include <vector>

#include "BranchSegment.h"

class Branch {
public:
	Branch();
	virtual ~Branch();

	bool operator==(const Branch &rhs) const;
	bool operator!=(const Branch &rhs) const;

	size_t depth() const;

	int assignId(int startAt);

	int id;

	// Position along the parent branch; range 0-1
	float position;

	bool isSubSegment;
	bool isSympodialChild;

	std::vector<BranchSegment> segments;
	std::vector<Branch> children;
	std::vector<Branch> sympodialChildren;
};

bool positionComparator(const Branch &a, const Branch &b);

#endif /* BRANCH_H_ */
