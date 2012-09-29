#include <algorithm>

#include "Branch.h"
#include "util.h"

using namespace std;

Branch::Branch() {
	isSubSegment = false;
	isSympodialChild = false;
}

Branch::~Branch() {
}

size_t Branch::depth() const {
	size_t depth = 0;

	for (vector<Branch>::const_iterator i = children.begin(); i != children.end(); ++i) {
		depth = max(depth, i->depth());
	}

	for (vector<Branch>::const_iterator i = sympodialChildren.begin(); i != sympodialChildren.end(); ++i) {
		depth = max(depth, i->depth());
	}

	return depth + 1;
}

int Branch::assignId(int startAt) {
	this->id = startAt++;

	for (vector<Branch>::iterator i = children.begin(); i != children.end(); ++i) {
		startAt = i->assignId(startAt);
	}

	for (vector<Branch>::iterator i = sympodialChildren.begin(); i != sympodialChildren.end(); ++i) {
		startAt = i->assignId(startAt);
	}

	return startAt;
}

bool Branch::operator==(const Branch &rhs) const {
	return
		(id == rhs.id) &&
		(position == rhs.position) &&
		(isSympodialChild == rhs.isSympodialChild) &&
		equal(segments, rhs.segments) &&
		equal(children, rhs.children) &&
		equal(sympodialChildren, rhs.sympodialChildren);
}

bool Branch::operator!=(const Branch &rhs) const {
  return !(*this == rhs);
}

bool positionComparator(const Branch &a, const Branch &b) {
	return a.position < b.position;
}
