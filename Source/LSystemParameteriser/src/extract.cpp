#include <algorithm>
#include <cfloat>
#include <cmath>

#include "extract.h"
#include "util.h"
#include "Random.h"

using namespace std;

#define SYMPODIAL_MIN_POSITION 0.95f
#define SAMPLE_VALUE_COUNT 100
#define MIN_POS_RANGE 0.05f
#define CHILD_POS_RANGE 0.05f;

void extractCurve(const Branch &branch, Curve &curve, Branch *parentBranch);
void extractCurvature(const Branch &branch, CurveData &curveData,
		NormalDist &(*normalDistAccessor)(pair<NormalDist, NormalDist> &val),
		pair<float, float> &(*segmentAngleType)(BranchSegment &segment),
		int &(*intPairAccessor)(pair<int, int> &val),
		float &(*floatPairAccessor)(pair<float, float> &val));
void extractLevelData(Branch &branch, vector<BranchLevel> &levels, size_t currentLevel, Branch *parentBranch);

void calculateBranchingPlanes(BranchLevel &level);
void calculateInferredLevel(vector<BranchLevel> &levels, BranchLevel &inferredLevel, size_t inferenceLevels, size_t propagation);

template <class T>
T &azimuthAccessor(pair<T, T> &val);
template<class T>
T &inclinationAccessor(pair<T, T> &val);

pair<float, float> &inclinationAccessor(BranchSegment &segment);
pair<float, float> &azimuthAccessor(BranchSegment &segment);

void generateNormalValues(NormalDist &from, NormalDist &stats, int sampleCount);
void generateNormalValues(float minVal, float maxVal, NormalDist &stats, bool clampPositive, int sampleCount = SAMPLE_VALUE_COUNT);
void generateUniformValues(float minVal, float maxVal, NormalDist &stats, bool clampPositive);

void extractData(Branch &branch, vector<BranchLevel> &levels, size_t inferenceLevels, size_t propagation, bool storeRootBranchStructure) {
	branch.assignId(0);

	// Create all levels
	size_t depth = branch.depth();
	for (size_t i = 0; i < depth; ++i) {
		if (levels.size() <= i) {
			BranchLevel level;
			level.branchLevel = i + 1;

			// Make a dummy branching plane, we'll do the actual sorting
			// of groups into planes after level extraction
			BranchingPlane branchingPlane;
			for (size_t j = 0; j < 2; ++j) {
				for (size_t k = 0; k < 2; ++k) {
					level.curveInfo[j][k].push_back(branchingPlane);
				}
			}

			levels.push_back(level);
		}
	}

	if (levels.size() > 0) {
		// Calculate curve info for the root curve
		// Add PI/2 to the branch's inclinations
		branch.segments[0].inclination.first -= M_PI / 2.0f;
		branch.segments[0].inclination.second -= M_PI / 2.0f;

		vector<Curve> group;
		Curve curve;
		extractCurve(branch, curve, 0);
		group.push_back(curve);
		levels[0].curveInfo[0][0][0].groups.push_back(group);

		// Extract the data for all levels
		extractLevelData(branch, levels, 1, 0);

		// Calculate branching planes
		for (vector<BranchLevel>::iterator i = levels.begin(); i != levels.end(); ++i) {
			calculateBranchingPlanes(*i);
		}

		// For each level, check if there is no child data for monopodial/sympodial cases
		// If there is none, add it from the other case
		for (size_t i = 0; i < levels.size(); ++i) {
			for (size_t isParentSympodial = 0; isParentSympodial < 2; ++isParentSympodial) {
				if (levels[i].curveInfo[0][isParentSympodial].size() == 0 &&
						levels[i].curveInfo[1][isParentSympodial][0].groups.size() == 0) {
					size_t copyFrom = 1 - isParentSympodial;

					for (size_t isChildSympodial = 0; isChildSympodial < 2; ++isChildSympodial) {
						levels[i].curveInfo[isChildSympodial][isParentSympodial] = levels[i].curveInfo[isChildSympodial][copyFrom];
					}

					if (i > 0) {
						levels[i - 1].childGroupDistribution[isParentSympodial] = levels[i - 1].childGroupDistribution[copyFrom];
						levels[i - 1].groupsPerUnitLength[isParentSympodial] = levels[i - 1].groupsPerUnitLength[copyFrom];
						levels[i - 1].childBranchProbability[isParentSympodial] = levels[i - 1].childBranchProbability[copyFrom];
						levels[i - 1].numGroupSplits[isParentSympodial] = levels[i - 1].numGroupSplits[copyFrom];
						levels[i - 1].splitStartPoint[isParentSympodial] = levels[i - 1].splitStartPoint[copyFrom];
					}
				}
			}
		}

		BranchLevel inferredLevel;
		calculateInferredLevel(levels, inferredLevel, inferenceLevels, propagation);
		levels.push_back(inferredLevel);

		if (storeRootBranchStructure) {
			// Calculate specific branch information for the root branch
			BranchInfo branchInfo;

			// Add monopodial groups
			for (size_t i = 0; i < branch.children.size(); ++i) {
				bool matched = false;
				BranchGroupInfo groupInfo;
				groupInfo.position = branch.children[i].position;

				for (size_t plane = 0; plane < levels[1].curveInfo[0][0].size(); ++plane) {
					for (size_t group = 0; group < levels[1].curveInfo[0][0][plane].groups.size(); ++group) {
						// Match the current child to a group
						for (size_t j = 0; j < levels[1].curveInfo[0][0][plane].groups[group].size(); ++j) {
							if (i >= branch.children.size()) break;

							// If there is a match, continue matching other children to the same group, until
							// a child that isn't in the group is hit -- then break out, add the group to
							// the list of child groups, and start over
							if (levels[1].curveInfo[0][0][plane].groups[group][j].id == branch.children[i].id) {
								groupInfo.branchingPlane = plane;
								groupInfo.branchingGroup = group;
								++i;
								j = -1;
								matched = true;
								continue;
							}
						}

						if (matched || i >= branch.children.size()) break;
					}

					if (matched || i >= branch.children.size()) break;
				}

				if (matched) {
					branchInfo.childGroups[0].push_back(groupInfo);
					--i;
				}
			}

			// Add sympodial groups (they will be at the default branching plane)
			if (branch.sympodialChildren.size() > 0) {
				BranchGroupInfo groupInfo;
				groupInfo.branchingPlane = 0;
				groupInfo.position = 1.0f;

				for (size_t i = 0; i < levels[1].curveInfo[1][0][0].groups.size(); ++i) {
					for (size_t j = 0; j < levels[1].curveInfo[1][0][0].groups[i].size(); ++j) {
						if (levels[1].curveInfo[1][0][0].groups[i][j].id == branch.sympodialChildren[0].id) {
							groupInfo.branchingGroup = i;
							break;
						}
					}
				}

				branchInfo.childGroups[1].push_back(groupInfo);
			}

			levels[0].exampleBranchInfo[0][0].push_back(branchInfo);
		}
	}
}

void extractLevelData(Branch &branch, vector<BranchLevel> &levels, size_t currentLevel, Branch *parentBranch) {
	int isSymp = branch.isSympodialChild ? 1 : 0;

	BranchLevel &level = levels[currentLevel - 1];

	// Actually perform the level extraction

	float minPos = 0.0f;	// Minimum position of a child

	for (vector<Branch>::const_iterator i = branch.children.begin(); i != branch.children.end(); ++i) {
		// Find the smallest position of a child branch
		if (i == branch.children.begin()) minPos = i->position;
		else minPos = min(minPos, i->position);
	}

	if (branch.children.size() > 0) {
		generateUniformValues(minPos - MIN_POS_RANGE, minPos + MIN_POS_RANGE, level.splitStartPoint[isSymp], true);
	}

	// Separate child branches into branch groups.
	// We define a branch group simply as child branches that are placed near
	// each other. We want to preserve such groups, as in the case with even
	// branching.
	sort(branch.children.begin(), branch.children.end(), positionComparator);

	vector< vector<Branch> > childGroups;
	vector<Branch> currentGroup;
	float groupPos;
	for (vector<Branch>::const_iterator i = branch.children.begin(); i != branch.children.end(); ++i) {
		if (currentGroup.size() > 0 && !approxEqual(i->position, groupPos, 0.05f)) {
			childGroups.push_back(currentGroup);
			currentGroup.clear();
		}

		currentGroup.push_back(*i);

		if (currentGroup.size() == 1) {
			groupPos = i->position;
		}
	}

	// At the end, push back whatever group we're left with
	if (currentGroup.size() > 0) {
		childGroups.push_back(currentGroup);
	}

	if (childGroups.size() > 0) {
		++level.numGroupSplits[isSymp].values[childGroups.size()];
	}

	pair<float, float> length = make_pair(0.0f, 0.0f);
	for (size_t i = 0; i < branch.segments.size(); ++i) {
		length.first += branch.segments[i].length.first;
		length.second += branch.segments[i].length.second;
	}
	if (average(length) > 0.0f) {
		float groupsPerUnitLength = childGroups.size() / length.first;
		level.groupsPerUnitLength[isSymp] = max(level.groupsPerUnitLength[isSymp], groupsPerUnitLength);
	}

	// Calculate the distance between groups of child branches
	float val, minVal, maxVal;
	for (size_t i = 1; i < childGroups.size(); ++i) {
		if (childGroups[i].size() == 0) continue;

		val = childGroups[i][0].position - childGroups[i - 1][0].position;
		minVal = val - CHILD_POS_RANGE;
		maxVal = val + CHILD_POS_RANGE;

		if (minVal < 0.0f) minVal = 0.0f;
		if (maxVal > 1.0f) maxVal = 1.0f;

		generateNormalValues(minVal, maxVal, level.childGroupDistribution[isSymp], true);
	}

	// Calculate the curves and angles for each (non-sympodial) group and add them to the level
	vector<Curve> groupInfo;
	for (vector<vector<Branch> >::const_iterator i = childGroups.begin(); i != childGroups.end(); ++i) {
		// Calculate the curve/angle information for each child in the group
		for (vector<Branch>::const_iterator j = i->begin(); j != i->end(); ++j) {
			Curve curve;
			extractCurve(*j, curve, &branch);
			groupInfo.push_back(curve);
		}

		// Add the group to the next level's groups, at the dummy plane
		if (groupInfo.size() > 0) {
			levels[currentLevel].curveInfo[0][isSymp][0].groups.push_back(groupInfo);
			groupInfo.clear();
		}
	}

	// Calculate sympodial curve information
	groupInfo.clear();
	for (vector<Branch>::iterator i = branch.sympodialChildren.begin(); i != branch.sympodialChildren.end(); ++i) {
		Curve curve;
		extractCurve(*i, curve, &branch);
		groupInfo.push_back(curve);
	}

	if (levels.size() > currentLevel && groupInfo.size() > 0) {
		levels[currentLevel].curveInfo[1][isSymp][0].groups.push_back(groupInfo);
	}

	// Calculate the probability of different types of child branching
	if (branch.children.size() > 0 && branch.sympodialChildren.size() > 0) {
		++level.childBranchProbability[isSymp].values[ChildBranchType::MonopodialAndSympodial];
	} else if (branch.children.size() > 0) {
		++level.childBranchProbability[isSymp].values[ChildBranchType::Monopodial];
	} else if (branch.sympodialChildren.size() > 0) {
		++level.childBranchProbability[isSymp].values[ChildBranchType::Sympodial];
	}

	// Extract data for children
	for (vector<Branch>::iterator i = branch.children.begin(); i != branch.children.end(); ++i) {
		extractLevelData(*i, levels, currentLevel + 1, &branch);
	}

	// Extract data for sympodial children
	for (vector<Branch>::iterator i = branch.sympodialChildren.begin(); i != branch.sympodialChildren.end(); ++i) {
		extractLevelData(*i, levels, currentLevel + 1, &branch);
	}
}

void extractCurve(const Branch &branch, Curve &curve, Branch *parentBranch) {
	curve.id = branch.id;

	// Extract curvature information
	extractCurvature(branch, curve.curveData, inclinationAccessor<NormalDist>, inclinationAccessor, inclinationAccessor<int>, inclinationAccessor<float>);
	extractCurvature(branch, curve.curveData, azimuthAccessor<NormalDist>, azimuthAccessor, azimuthAccessor<int>, azimuthAccessor<float>);

	// Extract angle information
	generateNormalValues(branch.segments[0].inclination.first, branch.segments[0].inclination.second, curve.angle.inclination, false);
	generateNormalValues(branch.segments[0].azimuth.first, branch.segments[0].azimuth.second, curve.angle.azimuth, false);

	// Extract radius information
	generateNormalValues(branch.segments[0].startRadius.first, branch.segments[0].startRadius.second, curve.startRadius, true);
	generateNormalValues(branch.segments[branch.segments.size() - 1].endRadius.first, branch.segments[branch.segments.size() - 1].endRadius.second, curve.endRadius, true);

	// Extract length information
	// Length should include other segments
	pair<float, float> length = make_pair(0.0f, 0.0f);
	for (size_t i = 0; i < branch.segments.size(); ++i) {
		length.first += branch.segments[i].length.first;
		length.second += branch.segments[i].length.second;
	}
	generateNormalValues(length.first, length.second, curve.length, true);

	// Extract parent length information
	if (parentBranch) {
		// Extract radius information
		generateNormalValues(parentBranch->segments[0].startRadius.first, parentBranch->segments[0].startRadius.second, curve.originalParentStartRadius, true);
		generateNormalValues(parentBranch->segments[parentBranch->segments.size() - 1].endRadius.first, parentBranch->segments[parentBranch->segments.size() - 1].endRadius.second, curve.originalParentEndRadius, true);

		// Extract length information
		// Length should include other segments
		length = make_pair(0.0f, 0.0f);
		for (size_t i = 0; i < parentBranch->segments.size(); ++i) {
			length.first += parentBranch->segments[i].length.first;
			length.second += parentBranch->segments[i].length.second;
		}
		generateNormalValues(length.first, length.second, curve.originalParentLength, true);
	}
}

void extractCurvature(const Branch &branch, CurveData &curveData,
		NormalDist &(*normalDistAccessor)(pair<NormalDist, NormalDist> &val),
		pair<float, float> &(*segmentAngleType)(BranchSegment &segment),
		int &(*intPairAccessor)(pair<int, int> &val),
		float &(*floatPairAccessor)(pair<float, float> &val)) {

	pair<float, float> totalLength = make_pair(0.0f, 0.0f);
	for (size_t i = 0; i < branch.segments.size(); ++i) {
		totalLength.first += branch.segments[i].length.first;
		totalLength.second += branch.segments[i].length.second;
	}

	// Merge segments with angles that are near zero
	// Also merge segments with lengths close to zero
	vector<BranchSegment> segments = branch.segments;
	for (size_t i = 1; i < segments.size(); ++i) {
		if (segmentAngleType(segments[i]).first > -M_PI / 20.0f && segmentAngleType(segments[i]).second < M_PI / 20.0f) {
			segments[i - 1].length.first += segments[i].length.first;
			segments[i - 1].length.second += segments[i].length.second;

			segments.erase(segments.begin() + i);
			--i;
		} else if (segments[i].length.first < 5.0f && segments[i].length.second < 5.0f) {
			segments[i - 1].length.first += segments[i].length.first;
			segments[i - 1].length.second += segments[i].length.second;

			// Average out the angles
			segmentAngleType(segments[i - 1]).first = average(segmentAngleType(segments[i - 1]).first, segmentAngleType(segments[i]).first);
			segmentAngleType(segments[i - 1]).second = average(segmentAngleType(segments[i - 1]).second, segmentAngleType(segments[i]).second);

			segments.erase(segments.begin() + i);
			--i;
		}
	}

	// Add the segment lengths (the length of a segment before a curvature change)
	for (size_t i = 0; i < segments.size(); ++i) {
		generateNormalValues(segments[i].length.first / totalLength.first,
				segments[i].length.second / totalLength.second,
				normalDistAccessor(curveData.segmentLengths), true);
	}

	// Calculate the curvature at each non-root segment
	vector<pair<float, float> > curvature;

	// Ignore the first segment, as that contains the angle to the parent
	// branch, which we don't want to include in our calculations
	for (size_t i = 1; i < segments.size(); ++i) {
		curvature.push_back(make_pair(
				segmentAngleType(segments[i]).first,
				segmentAngleType(segments[i]).second));
	}

	// Record distance between curvature sign changes
	// At the last level, we always record a dummy sign change
	float lastSign = 0.0f;
	float currentCurve = 0.0f;
	size_t lastSignChange = -1;
	float minLength, maxLength;
	for (size_t i = 0; i < curvature.size(); ++i) {
		currentCurve = average(curvature[i].first, curvature[i].second);

		// If there is a sign change, record the distance to the last sign
		// change
		if (lastSign * currentCurve < 0.0f) {
			// Get the distance to the previous sign change
			minLength = 0.0f;
			maxLength = 0.0f;

			// Indexing is slightly tricky, since curvature.size() == segments.size() - 1
			for (size_t j = lastSignChange + 1; j < i + 1; ++j) {
				minLength += segments[j].length.first;
				maxLength += segments[j].length.second;
			}

			if (maxLength > 0.0f)
				generateNormalValues(minLength / totalLength.first, maxLength / totalLength.second, normalDistAccessor(curveData.curvatureSignChangeDist), true);

			lastSignChange = i;
		}

		// Change the sign, but only if to a non-zero value
		if (currentCurve != 0.0f) {
			lastSign = currentCurve;
		}
	}

	// Record a final sign change
	minLength = 0.0f;
	maxLength = 0.0f;

	for (size_t i = lastSignChange + 1; i < segments.size(); ++i) {
		minLength += segments[i].length.first;
		maxLength += segments[i].length.second;
	}

	if (maxLength > 0.0f)
		generateNormalValues(minLength / totalLength.first, maxLength / totalLength.second, normalDistAccessor(curveData.curvatureSignChangeDist), true);

	// Record curvature magnitudes
	if (curvature.size() > 0) {
		for (size_t i = 0; i < curvature.size(); ++i) {
			float minVal = abs(curvature[i].first);
			float maxVal = abs(curvature[i].second);
			int sampleCount = (int)ceil(average(segments[i + 1].length.first, segments[i + 1].length.second) * 100.0f);
			sampleCount = max(sampleCount, 1);
			generateNormalValues(minVal, maxVal, normalDistAccessor(curveData.curvatureMagnitude), true, sampleCount);
		}
	} else {
		generateNormalValues(0.0f, 0.0f, normalDistAccessor(curveData.curvatureMagnitude), true);
	}

	// Record the min/max curvature observed along the whole length of the curve
	float minCurve = FLT_MAX;
	float maxCurve = -FLT_MAX;
	float currentCurvature = 0.0f;

	for (size_t i = 0; i < curvature.size(); ++i) {
		currentCurvature += average(curvature[i]);
		minCurve = min(minCurve, currentCurvature - 0.1f);
		maxCurve = max(maxCurve, currentCurvature + 0.1f);
	}

	floatPairAccessor(curveData.minTotalCurvature) = minCurve;
	floatPairAccessor(curveData.maxTotalCurvature) = maxCurve;

	// Record sign of first curvature value
	if (curvature.size() > 0) {
		int sign;
		float curve = average(curvature[0].first, curvature[0].second);
		if (curve >= 0) {
			sign = 1;
		} else {
			sign = -1;
		}
		intPairAccessor(curveData.startCurvatureSign) = sign;
	} else {
		intPairAccessor(curveData.startCurvatureSign) = 1;
	}
}

void calculateBranchingPlanes(BranchLevel &level) {
	// We only calculate branching planes for non-sympodial children
	for (int isSympodialParent = 0; isSympodialParent < 2; ++isSympodialParent) {
		// Sort each branching group, first by azimuth, and then by inclination
		// This helps us compare branching groups to see if they should be in the
		// same branching plane
		vector<vector<Curve> > &groups = level.curveInfo[0][isSympodialParent][0].groups;
		for (vector<vector<Curve> >::iterator i = groups.begin(); i != groups.end(); ++i) {
			sort(i->begin(), i->end(), curveComparator);
		}

		vector<BranchingPlane> planes;

		// Generate planes
		for (vector<vector<Curve> >::iterator i = groups.begin(); i != groups.end(); ++i) {
			// Check if the current group matches any of the branching planes; if
			// not, create a new plane
			bool isMatch = false;

			for (vector<BranchingPlane>::iterator j = planes.begin(); j != planes.end(); ++j) {
				if (i->size() == j->groups[0].size()) {
					// If the inclination signs are the same, and the azimuths are
					// the same, the branching plane is a match
					for (size_t k = 0; k < i->size(); ++k) {
						if (((j->groups[0][k].angle.inclination.normalStat.getMean() < 0.0f && (*i)[k].angle.inclination.normalStat.getMean() < 0.0f) ||
								(j->groups[0][k].angle.inclination.normalStat.getMean() > 0.0f && (*i)[k].angle.inclination.normalStat.getMean() > 0.0f)) &&
								approxEqual(j->groups[0][k].angle.azimuth.normalStat.getMean(), (*i)[k].angle.azimuth.normalStat.getMean(), M_PI / 8)) {
							j->groups.push_back(*i);
							isMatch = true;
							break;
						}
					}
				}

				if (isMatch) break;
			}

			if (!isMatch) {
				BranchingPlane plane;
				plane.groups.push_back(*i);
				planes.push_back(plane);
			}
		}

		// Finally, assign the planes back to the level
		level.curveInfo[0][isSympodialParent].assign(planes.begin(), planes.end());
	}
}

void calculateInferredLevel(vector<BranchLevel> &levels, BranchLevel &inferredLevel, size_t inferenceLevels, size_t propagation) {
	if (levels.size() <= 1) return;

	if (inferenceLevels <= 0) inferenceLevels = 1;
	// Infer a propagation level if necessary
	if (propagation <= 0) {
		// Levels   Propagation Level
		// ------ | -----------------
		//   1    |        1
		//   2    |        1
		//   3    |        1
		//   4    |        2
		//   5    |        2
		//   6    |        2
		//   7    |        3
		// etc.
		propagation = (levels.size() - 1) / 3 + 1;
	}

	inferredLevel.branchLevel = levels[levels.size() - 1].branchLevel + 1;

	// The level we'll start inferring from
	size_t inferFromLevel;
	if (levels.size() > inferenceLevels + 2) inferFromLevel = levels.size() - inferenceLevels;
	else if (levels.size() > 2) inferFromLevel = 2;
	else inferFromLevel = 1;

	// Add curve info from a number of levels

	// Use dummy branching planes for now
	for (size_t isChildSympodial = 0; isChildSympodial < 2; ++isChildSympodial) {
		for (size_t isParentSympodial = 0; isParentSympodial < 2; ++isParentSympodial) {
			BranchingPlane plane;
			inferredLevel.curveInfo[isChildSympodial][isParentSympodial].push_back(plane);
		}
	}

	for (size_t i = inferFromLevel; i < levels.size(); ++i) {
		for (size_t isChildSympodial = 0; isChildSympodial < 2; ++isChildSympodial) {
			for (size_t isParentSympodial = 0; isParentSympodial < 2; ++isParentSympodial) {
				for (size_t j = 0; j < levels[i].curveInfo[isChildSympodial][isParentSympodial].size(); ++j) {
					for (size_t k = 0; k < levels[i].curveInfo[isChildSympodial][isParentSympodial][j].groups.size(); ++k) {
						if (levels[i].curveInfo[isChildSympodial][isParentSympodial][j].groups[k].size() > 0) {
							// Treat children of sympodial/monopodial parents the same for our purposes
							inferredLevel.curveInfo[isChildSympodial][0][0].groups.push_back(levels[i].curveInfo[isChildSympodial][isParentSympodial][j].groups[k]);
							inferredLevel.curveInfo[isChildSympodial][1][0].groups.push_back(levels[i].curveInfo[isChildSympodial][isParentSympodial][j].groups[k]);
						}
					}
				}
			}
		}
	}

	// Add child distribution info from a number of levels
	// Note that we don't include the last level, since that has
	// no child distribution information
	// In addition, we treat monopodial/sympodial branching the same way here
	for (size_t i = inferFromLevel - 1; i < levels.size() - 1; ++i) {
		for (size_t isParentSympodial = 0; isParentSympodial < 2; ++isParentSympodial) {
			for (size_t j = 0; j < 2; ++j) {

				generateNormalValues(
						levels[i].childGroupDistribution[isParentSympodial],
						inferredLevel.childGroupDistribution[j],
						levels[i].childGroupDistribution[isParentSympodial].normalStat.getNumValues());

				generateNormalValues(
						levels[i].splitStartPoint[isParentSympodial],
						inferredLevel.splitStartPoint[j],
						levels[i].splitStartPoint[isParentSympodial].normalStat.getNumValues());

				for (std::map<int, float>::const_iterator k = levels[i].childBranchProbability[isParentSympodial].values.begin();
						k != levels[i].childBranchProbability[isParentSympodial].values.end(); ++k) {
					inferredLevel.childBranchProbability[j].values[k->first] += k->second;
				}

				for (std::map<int, float>::const_iterator k = levels[i].numGroupSplits[isParentSympodial].values.begin();
						k != levels[i].numGroupSplits[isParentSympodial].values.end(); ++k) {
					inferredLevel.numGroupSplits[j].values[k->first] += k->second;
				}
			}
		}
	}

	// Calculate the actual branching planes
	calculateBranchingPlanes(inferredLevel);

	// Propagate changes up through the levels
	size_t idx;

	for (size_t i = 0; i < propagation; ++i) {
		if (levels.size() > i + 1) {
			idx = levels.size() - i - 1;

			for (size_t j = 0; j < 2; ++j) {
				if (i > 0 && levels[levels.size() - i].curveInfo[0][j].size() == 0) {
					levels[levels.size() - i].curveInfo[0][j] = inferredLevel.curveInfo[0][j];
				}
				levels[idx].childBranchProbability[j] = inferredLevel.childBranchProbability[j];
				levels[idx].childGroupDistribution[j] = inferredLevel.childGroupDistribution[j];
				levels[idx].numGroupSplits[j] = inferredLevel.numGroupSplits[j];
				levels[idx].splitStartPoint[j] = inferredLevel.splitStartPoint[j];
			}
		}
	}
}

void generateNormalValues(NormalDist &from, NormalDist &stats, int sampleCount) {
	stats.min = min(stats.min, from.min);
	stats.max = max(stats.max, from.max);

	float mean = from.normalStat.getMean();
	float stddev = from.normalStat.getStandardDeviation();

	for (int i = 0; i < sampleCount; ++i) {
		stats.normalStat.addValue(Random::normal(mean, stddev));
	}
}

void generateNormalValues(float minVal, float maxVal, NormalDist &stats, bool clampPositive, int sampleCount) {
	if (minVal > maxVal) {
		generateNormalValues(maxVal, minVal, stats, clampPositive);
		return;
	}

	if (clampPositive && minVal < 0.0f) minVal = 0.0f;

	stats.min = min(stats.min, minVal);
	stats.max = max(stats.max, maxVal);

	float mean = (minVal + maxVal) / 2.0f;
	float stddev = (maxVal - mean) * 0.5f;

	for (int i = 0; i < sampleCount; ++i) {
		stats.normalStat.addValue(Random::normal(mean, stddev));
	}
}

void generateUniformValues(float minVal, float maxVal, NormalDist &stats, bool clampPositive) {
	if (minVal > maxVal) {
		generateUniformValues(maxVal, minVal, stats, clampPositive);
		return;
	}

	if (clampPositive && minVal < 0.0f) minVal = 0.0f;

	stats.min = min(stats.min, minVal);
	stats.max = max(stats.max, maxVal);
	for (int i = 0; i < SAMPLE_VALUE_COUNT; ++i) {
		stats.normalStat.addValue(Random::uniform(minVal, maxVal));
	}
}

template<class T>
T &azimuthAccessor(pair<T, T> &val) {
	return val.second;
}

template<class T>
T &inclinationAccessor(pair<T, T> &val) {
	return val.first;
}

pair<float, float> &inclinationAccessor(BranchSegment &segment) {
	return segment.inclination;
}

pair<float, float> &azimuthAccessor(BranchSegment &segment) {
	return segment.azimuth;
}
