#include "BranchLevel.h"

BranchLevel::BranchLevel() {
	for (size_t i = 0; i < 2; ++i) {
		groupsPerUnitLength[i] = 0.0f;
	}
}

BranchLevel::~BranchLevel() {
}

bool curveComparator(const Curve &a, const Curve &b) {
	if (a.angle.azimuth.normalStat.getMean() < b.angle.azimuth.normalStat.getMean()) return true;
	else if (a.angle.azimuth.normalStat.getMean() == b.angle.azimuth.normalStat.getMean()) {
		return a.angle.inclination.normalStat.getMean() < b.angle.inclination.normalStat.getMean();
	}

	return false;
}
