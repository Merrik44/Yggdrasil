#ifndef BRANCHLEVEL_H_
#define BRANCHLEVEL_H_

#include <map>
#include <vector>

#include "util.h"

namespace ChildBranchType {
	const int Monopodial = 0;
	const int Sympodial = 1;
	const int MonopodialAndSympodial = 2;
}

struct Angle {
	NormalDist inclination;
	NormalDist azimuth;
};

struct CurveData {
	// Curve data - first is for inclination, second for azimuth
	std::pair<NormalDist, NormalDist> segmentLengths;			// Length of subsegments in curve
	std::pair<NormalDist, NormalDist> curvatureSignChangeDist;	// Distance between curvature sign changes
	std::pair<NormalDist, NormalDist> curvatureMagnitude;		// Magnitude of curvature along subsegments
	std::pair<int, int> startCurvatureSign;						// The sign of the first curvature value

	// The minimum/maximum total curvature observed over the entire curve
	std::pair<float, float> minTotalCurvature;
	std::pair<float, float> maxTotalCurvature;
};

struct Curve {
	int id;

	Angle angle;
	CurveData curveData;

	NormalDist length;
	NormalDist startRadius;
	NormalDist endRadius;

	NormalDist originalParentLength;
	NormalDist originalParentStartRadius;
	NormalDist originalParentEndRadius;
};
bool curveComparator(const Curve &a, const Curve &b);

struct BranchingPlane {
	// Groups of angles. When branches are placed very close to one another,
	// their angles should be taken from one of the groups
	std::vector<std::vector<Curve> > groups;
};

// Information for a specific branch at this level of the tree
struct BranchGroupInfo {
	float position;
	size_t branchingPlane;
	size_t branchingGroup;
};

struct BranchInfo {
	// childGroups[0] for monopodial, childGroups[1] for sympodial
	// childGroups[1] will have a maximum size of 1
	std::vector<BranchGroupInfo> childGroups[2];
};

class BranchLevel {
public:
	BranchLevel();
	virtual ~BranchLevel();

	size_t branchLevel;

	// [1] for branches that are sympodial children, [0] for branches that are
	// monopodial children.
	// e.g. length[0] is the length of a branch that is a monopodial child,
	// while length[1] is the length of a branch that is a sympodial child

	// Store start angles for monopodial/sympodial branch groups as vectors
	// indicating their grouping. This ensures that we don't run into problems
	// where we generate multiple branches facing the same direction. In
	// addition, we'd like to avoid really asymmetrical branching patterns
	// where such branching patterns aren't present to begin with.

	// Note that curve info at a certain branching level will be info on the
	// curves for that branching level, i.e. the child branches of branches a
	// level above.

	// curveInfo[0][0] gives info about monopodial children of monopodial parents
	// curveInfo[0][1] gives info about monopodial children of sympodial parents
	// curveInfo[1][0] gives info about sympodial children of monopodial parents
	// curveInfo[1][1] gives info about sympodial children of sympodial parents
	// curveInfo[1][j].size() == 1, as sympodial children don't really have a
	// branching plane.

	// Groups in the same branching plane will have branches that point in
	// roughly the same direction, i.e. each pair of branches taken from
	// different groups (at the same index) will have roughly the same azimuth,
	// and an inclination of the same sign.
	// The purpose of this is to capture alternate branching patterns, where
	// the branch plane alternates with each child branch. When reproducing a
	// tree, we want to spread branches with the same branching pattern as far
	// apart as possible.
	std::vector<BranchingPlane> curveInfo[2][2];

	// Info about specific branches at this level
	std::vector<BranchInfo> exampleBranchInfo[2][2];

	// Info about the children of this branch level
	NormalDist childGroupDistribution[2];	// Distance between clusters of child branches

	float groupsPerUnitLength[2];
	DiscreteDist childBranchProbability[2];	// Probability of different types of child branching
	DiscreteDist numGroupSplits[2];			// Non-sympodial splits; the number of clusters of children on a branch

	NormalDist splitStartPoint[2];

};

#endif /* BRANCHLEVEL_H_ */
