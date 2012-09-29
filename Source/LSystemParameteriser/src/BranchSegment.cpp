#include "BranchSegment.h"

BranchSegment::BranchSegment() {
}

BranchSegment::~BranchSegment() {
}

bool BranchSegment::operator==(const BranchSegment &rhs) const {
	return (inclination == rhs.inclination) &&
		(azimuth == rhs.azimuth) &&
		(startRadius == rhs.startRadius) &&
		(endRadius == rhs.endRadius) &&
		(length == rhs.length);
}

bool BranchSegment::operator!=(const BranchSegment&rhs) const {
	  return !(*this == rhs);
}
