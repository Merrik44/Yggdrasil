#include <iostream>
#include <map>
#include <sstream>

#include "construct.h"
#include "util.h"

using namespace std;

#define CHILD_END_SYMP_POS 0.95f
#define CURVATURE_BOUNDARY_NUDGE "-0.2f"

void appendLookupTables(const vector<BranchLevel> &branchLevels, string &lpfgFile);
void appendLookupFunctions(const vector<BranchLevel> &branchLevels, string &lpfgFile);
void appendProduction(const vector<BranchLevel> &branchLevels,
		float monopodialScalingFactor, float sympodialScalingFactor,
		float minRadius, float minLength, int branchLimit,
		string &lpfgFile);
void appendRandomisationFunctions(string &lpfgFile);

string constructLookupTable(const string &tableType, const string &tableName,
		size_t dims, size_t finalDimElements,
		size_t (*size)(const size_t *idx, size_t idxLen),
		string (*get)(const size_t *idx));
void constructLookupTable(string &lookup, size_t *idx, size_t current, size_t dims,
		size_t (*size)(const size_t *idx, size_t idxLen),
		string (*get)(const size_t *idx));
size_t lookupTableMaxElements(size_t *idx, size_t current, size_t depth,
		size_t (*size)(const size_t *idx, size_t idxLen));

string constructNormalArgs(const NormalDist &norm);
string constructPiecewiseConstantGenerator(const DiscreteDist &discreteDist,
		const string &varName, const string &lineStart);

void constructLpfgFile(const vector<BranchLevel> &branchLevels, string &lpfgFile,
		int derivationLength,
		float monopodialScalingFactor, float sympodialScalingFactor,
		float minRadius, float minLength, int branchLimit) {
	if (branchLevels.size() == 0) return;

	// The final derivation step is just to clean up outstanding non-terminal branches
	if (derivationLength < 0) {
		derivationLength = branchLevels.size() - 1;
	} else {
		derivationLength += branchLevels.size() - 1;
	}
	++derivationLength;

	// Includes
	lpfgFile += "#include <algorithm>\n"
		"#include <ctime>\n"
		"#include <boost/random.hpp>\n";

	// Global variables
	lpfgFile +=
			"\nint derivationLength = " + toString(derivationLength) + ";\n"
			"int derivationCounter = 0;\n"
			"int totalBranchCount = 0;\n"
			"int branchLimit = " + toString(branchLimit) + ";\n";

	// Write out random functions
	appendRandomisationFunctions(lpfgFile);

	// Enum for specifying branching type when creating a branch
	lpfgFile += "\nnamespace BranchType {\n"
		"\tenum Enum {\n"
		"\t\tChildBranch,\n"
		"\t\tNewInclinationSegment,\n"
		"\t\tNewAzimuthSegment,\n"
		"\t\tInclinationCurvatureSignChange,\n"
		"\t\tAzimuthCurvatureSignChange\n"
		"\t};\n"
		"}\n";

	// Append the lookup tables
	appendLookupTables(branchLevels, lpfgFile);
	appendLookupFunctions(branchLevels, lpfgFile);

	// Write out Start function to set random seed and counter
	lpfgFile += "\nStart: {\n"
		"\trng.seed(static_cast<unsigned int>(std::time(0)));\n"
		"\tderivationCounter = 0;\n"
		"}\n";

	// Write out EndEach function to increment derivation counter
	lpfgFile += "\nEndEach: {\n"
		"\t++derivationCounter;\n"
		"}\n";

	// Write out derivation length
	lpfgFile += "\nderivation length: " + toString(derivationLength) + ";\n";

	// Write out module definitions

	// Default modules
	lpfgFile += "\nmodule SB();\n"
		"module EB();\n"
		"module MovRel3f(float, float, float);\n"
		"module RotRel3f(float, float, float, float);\n"
		"module Cylinder(float, float, float);\n";

	// Branch module
							  // branchLevel, parentRadius, parentLength, positionOnParent, isChildSympodial, isParentSympodial, childBranchingPlane, childBranchingGroup, childCurveType, inferLevel
	lpfgFile += "\nmodule Branch(int, float, float, float, int, int, int, int, int, int);\n";

	// Write out axiom
	lpfgFile += "\naxiom: Branch(0, -1.0f, -1.0f, 0.0f, 0, 0, 0, 0, 0, 0);\n";

	// Write out the production
	appendProduction(branchLevels,
			monopodialScalingFactor, sympodialScalingFactor,
			minRadius, minLength, branchLimit, lpfgFile);
}

void appendProduction(const vector<BranchLevel> &branchLevels,
		float monopodialScalingFactor, float sympodialScalingFactor,
		float minRadius, float minLength, int branchLimit,
		string &lpfgFile) {
	lpfgFile += "\nBranch(branchLevel, parentRadius, parentLength, positionOnParent, isChildSympodial, isParentSympodial, childBranchingPlane, childBranchingGroup, childCurveType, inferLevel): {\n";

	// If at the final derivation step, don't do anything
	lpfgFile +=
		"\tif (derivationCounter >= derivationLength - 1) {\n"
			"\t\tproduce;\n"
		"\t}\n";

	// If we're over the branch limit, don't do anything
	lpfgFile +=
		"\tif (branchLimit > 0 && totalBranchCount >= branchLimit) {\n"
			"\t\tproduce;\n"
		"\t}\n";

	// If the derivation counter hasn't reached our level yet, don't do anything during this derivation step
	lpfgFile +=
		"\tif (derivationCounter < branchLevel) {\n"
			"\t\treturn;\n"
		"\t}\n";

	// The level after this one
	lpfgFile += "\n\tint nextLevel = (branchLevel == " + toString(branchLevels.size() - 1) + ") ? branchLevel : branchLevel + 1;\n";

	// The level we use for generating children
	lpfgFile += "\n\tint childInfoLevel = branchLevel;\n";

	// The inference level of any of this branch's children
	lpfgFile += "\n\tint nextInferLevel = (branchLevel >= " + toString(branchLevels.size() - 2) + ") ? inferLevel + 1 : 0;\n";

	// Get the type of child branching (monopodial, sympodial, or both)
	lpfgFile +=
		"\n\tint childBranchType = getChildBranchType(childInfoLevel, isChildSympodial);\n";

	// If at the root branch level, generate a curve type
	// Usually branches generate curve types for their children, however,
	// the root has no parent to do this for them
	lpfgFile +=
		"\n\tif (branchLevel == 0) {\n"
			"\t\tchildBranchingPlane = uniformDist(0, numBranchingPlanes[0][isChildSympodial][isParentSympodial][0] - 1);\n"
			"\t\tchildBranchingGroup = uniformDist(0, numGroupsInBranchingPlane[0][isChildSympodial][isParentSympodial][childBranchingPlane][0] - 1);\n"
			"\t\tchildCurveType = uniformDist(0, numCurvesInGroup[0][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][0] - 1);\n"
		"\t}\n";

	// Radius and length
	lpfgFile +=
		"\n\tfloat startRadius = normalDist(branchStartRadius[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType]);\n"
		"\tfloat endRadius = normalDist(branchEndRadius[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType]);\n"
		"\tfloat length = normalDist(branchLength[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType]);\n";

	// If we're not inferring, infer based on a comparison between the actual parent information, and the original parent information
	lpfgFile +=
		"\n\tif (branchLevel > 0 && inferLevel == 0) {\n"
			"\t\tfloat originalParentLength = normalDist(branchOriginalParentLength[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType]);\n"
			"\t\tif (originalParentLength > 0.0f) length *= std::min(parentLength / originalParentLength, 1.0f);\n"
			"\t\tfloat originalParentStartRadius = normalDist(branchOriginalParentStartRadius[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType]);\n"
			"\t\tfloat originalParentEndRadius = normalDist(branchOriginalParentEndRadius[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType]);\n"
			"\t\tfloat originalParentRadius = originalParentStartRadius + positionOnParent * (originalParentEndRadius - originalParentStartRadius);\n"
			"\t\tif (originalParentRadius > 0.0f) {\n"
				"\t\t\tstartRadius *= std::min(parentRadius / originalParentRadius, 1.0f);\n"
				"\t\t\tendRadius *= std::min(parentRadius / originalParentRadius, 1.0f);\n"
			"\t\t}\n"
		"\t}\n";

	lpfgFile +=
		"\nif (isChildSympodial) startRadius = parentRadius;\n";

	// Ensure that start radius is greater than or equal to the parent's
	// radius, and end radius is less than or equal to the start radius
	lpfgFile +=
		"\n\tif (parentRadius > 0.0f && startRadius > parentRadius) startRadius = parentRadius;\n"
		"\tif (endRadius > startRadius) endRadius = startRadius;\n";

	// Scale the radii and length by our infer level
	lpfgFile +=
		"\n\tfloat scaling = isChildSympodial ? " + toString(sympodialScalingFactor) + " : " + toString(monopodialScalingFactor) + ";\n"
		"\tif (inferLevel > 0) {\n"
			"\t\tstartRadius = parentRadius;\n"
			"\t\tendRadius = startRadius;\n"

			"\n\t\tfloat mean = branchLength[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType][0];\n"
			"\t\tlength = parentLength * length / (mean == 0 ? length : mean);\n"

			"\n\t\tif (!isChildSympodial) {\n"
				"\t\t\tstartRadius *= scaling;\n"
				"\t\t\tendRadius *= scaling;\n"
			"\t\t}\n"
			"\t\tendRadius *= scaling;\n"

			"\n\t\tif (isChildSympodial) length *= sqrtf(scaling);\n"
			"\t\telse length *= scaling;\n"
		"\t}\n";

	// If, for some reason, we have a really small radius/length, don't output the branch
	lpfgFile +=
		"\n\tif (startRadius <= " + toString(minRadius) + " || length <= " + toString(minLength) + ") {\n"
			"\t\tproduce;\n"
		"\t}\n";

	// Generate sub-branch groups (non-sympodial)
	// This may not actually end up being the number of groups generated
	lpfgFile +=
		"\tint maxSubBranchGroups = -1;\n"
		"\tif (childGroupsPerUnitLength[childInfoLevel][isChildSympodial][0] > 0.0f) {\n"
			"\t\tmaxSubBranchGroups = (int)std::floor(length * childGroupsPerUnitLength[childInfoLevel][isChildSympodial][0] + 0.5f);\n"
		"\t}\n";

	lpfgFile +=
		"\n\tint subBranchGroupCount = 0;\n"
		"\tif (childBranchType == " + toString(ChildBranchType::Monopodial) + " || childBranchType == " + toString(ChildBranchType::MonopodialAndSympodial) + ") {\n"
			"\t\tsubBranchGroupCount = getSubBranchGroupCount(childInfoLevel, isChildSympodial);\n"
		"\t}\n"
		"\tif (maxSubBranchGroups >= 0) {\n"
			"\t\tsubBranchGroupCount = std::min(subBranchGroupCount, maxSubBranchGroups);\n"
		"\t}\n";

	// Generate sympodial branch group
	lpfgFile +=
		"\n\tint sympodialSubBranchGroup, sympodialBranchCount = 0;\n"
		"\tif ((childBranchType == " + toString(ChildBranchType::Sympodial) + " || childBranchType == " + toString(ChildBranchType::MonopodialAndSympodial) + ") && numGroupsInBranchingPlane[nextLevel][1][isChildSympodial][0][0] > 0) {\n"
			"\t\tsympodialSubBranchGroup = uniformDist(0, numGroupsInBranchingPlane[nextLevel][1][isChildSympodial][0][0] - 1);\n"
			"\t\tsympodialBranchCount = numCurvesInGroup[nextLevel][1][isChildSympodial][0][sympodialSubBranchGroup][0];\n"
		"\t}\n";

	lpfgFile +=
		"\n\tfloat splitStart = normalDist(splitStartPoint[childInfoLevel][isChildSympodial]);\n";

	// Generate the positions of each sub-branch group
	// Here's where we actually determine how many groups will be generated
	lpfgFile +=
		"\n\tstd::vector<std::pair<float, BranchType::Enum> > subBranchGroupPositions;\n"
		"\tfloat pos;\n"
		"\tif (subBranchGroupCount > 0) {\n"
			"\t\tpos = splitStart;\n"
			"\t\twhile (pos < " + toString(CHILD_END_SYMP_POS) + " && subBranchGroupPositions.size() < subBranchGroupCount) {\n"
				"\t\t\tsubBranchGroupPositions.push_back(std::make_pair(pos, BranchType::ChildBranch));\n"
				"pos += normalDist(childGroupDistribution[childInfoLevel][isChildSympodial]);\n"
			"\t\t}\n"
		"\t}\n";

	// Generate the number of branch groups of each plane type
	lpfgFile +=
		"\n\tsize_t totalSubBranchGroups = 0;\n"
		"\tfor (size_t i = 0; i < numBranchingPlanes[nextLevel][0][isChildSympodial][0]; ++i) {\n"
			"\t\ttotalSubBranchGroups += numGroupsInBranchingPlane[nextLevel][0][isChildSympodial][i][0];\n"
		"\t}\n";

	// branchPlaneCount will contain the number of groups of each plane type
	// The number of branches of a given plane type will be:
	//   the number of groups in that plane * the number of group clusters on this branch / the total number of groups for all planes
	lpfgFile +=
		"\n\tstd::vector<size_t> branchPlaneCount;\n"
		"\tfor (size_t i = 0; i < numBranchingPlanes[nextLevel][0][isChildSympodial][0]; ++i) {\n"
			"\t\tbranchPlaneCount.push_back((size_t)ceil("
				"numGroupsInBranchingPlane[nextLevel][0][isChildSympodial][i][0] * "
				"subBranchGroupPositions.size() / "
				"(float)totalSubBranchGroups));\n"
		"\t}\n";

	// Generate a permutation for branch planes
	// This will be the order that branch planes are produced on the final
	// tree, although an individual branch plane may be omitted.
	lpfgFile +=
		"\n\tstd::vector<size_t> branchPlaneOrder;\n"
		"\tfor (size_t i = 0; i < numBranchingPlanes[nextLevel][0][isChildSympodial][0]; ++i) {\n"
			"\t\tbranchPlaneOrder.push_back(i);\n"
		"\t}\n";
	lpfgFile +=
		"\tstd::random_shuffle(branchPlaneOrder.begin(), branchPlaneOrder.end(), uniformDistExcl);\n";

	// Generate plane indexes for the branch planes to use for non-sympodial children
	lpfgFile +=
		"\n\tstd::vector<std::pair<size_t, size_t> > childBranchPlanes;\n"
		"\twhile (childBranchPlanes.size() < subBranchGroupPositions.size()) {\n"
			"\t\tfor (size_t i = 0; i < branchPlaneOrder.size(); ++i) {\n"
				"\t\t\tif (childBranchPlanes.size() >= subBranchGroupPositions.size()) break;\n"
				"\t\t\tfloat prob = branchPlaneCount[branchPlaneOrder[i]] * totalSubBranchGroups * totalSubBranchGroups;\n"
				"\t\t\tprob /= subBranchGroupPositions.size() * (subBranchGroupPositions.size() - childBranchPlanes.size());\n"
				"\t\t\tif (uniformDist(0.0f, 1.0f) < prob) {\n"
					"\t\t\t\tsize_t groupType = 0;\n"
					"\t\t\t\tif (numGroupsInBranchingPlane[nextLevel][0][isChildSympodial][branchPlaneOrder[i]][0] > 0)\n"
						"\t\t\t\t\tgroupType = uniformDist(0, numGroupsInBranchingPlane[nextLevel][0][isChildSympodial][branchPlaneOrder[i]][0] - 1);\n"
					"\t\t\t\tchildBranchPlanes.push_back(std::make_pair(branchPlaneOrder[i], groupType));\n"
					"\t\t\t\t--branchPlaneCount[branchPlaneOrder[i]];\n"
				"\t\t\t}\n"
			"\t\t}\n"
		"\t}\n";

	// If there is already an example of a branch at this level, use that
	lpfgFile +=
		"\n\tif (numExampleBranches[childInfoLevel][isChildSympodial][isParentSympodial][0] > 0) {\n"
			"\t\tint exampleBranch = uniformDist(0, numExampleBranches[childInfoLevel][isChildSympodial][isParentSympodial][0] - 1);\n"

			"\n\t\tsympodialBranchCount = 0;\n"
			"\t\t\nsubBranchGroupPositions.clear();\n"
			"\t\tchildBranchPlanes.clear();\n"

			"\n\t\tif (numExampleBranchGroups[childInfoLevel][isChildSympodial][isParentSympodial][exampleBranch][1][0] > 0) {\n"
				"\t\t\tsympodialSubBranchGroup = exampleBranchGroupNumber[childInfoLevel][isChildSympodial][isParentSympodial][exampleBranch][1][0][0];\n"
				"\t\t\tsympodialBranchCount = numCurvesInGroup[nextLevel][1][isChildSympodial][0][sympodialSubBranchGroup][0];"
			"\t\t}\n"

			"\n\t\tsubBranchGroupCount = numExampleBranchGroups[childInfoLevel][isChildSympodial][isParentSympodial][exampleBranch][0][0];\n"
			"\t\tfor (int i = 0; i < subBranchGroupCount; ++i) {\n"
				"\t\t\tfloat pos = exampleBranchGroupPosition[childInfoLevel][isChildSympodial][isParentSympodial][exampleBranch][0][i][0];\n"
				"\t\t\tint branchingPlane = exampleBranchGroupBranchingPlane[childInfoLevel][isChildSympodial][isParentSympodial][exampleBranch][0][i][0];\n"
				"\t\t\tint groupType = exampleBranchGroupNumber[childInfoLevel][isChildSympodial][isParentSympodial][exampleBranch][0][i][0];\n"
				"\t\t\tsubBranchGroupPositions.push_back(std::make_pair(pos, BranchType::ChildBranch));\n"
				"\t\t\tchildBranchPlanes.push_back(std::make_pair(branchingPlane, groupType));\n"
			"\t\t}\n"
		"\n\t}\n";

	// Generate the curve for this branch

	// Generate sub-segments (inclination and azimuth)
	lpfgFile += "\n\tstd::vector<std::pair<float, BranchType::Enum> > inclinationSubSegmentPositions;\n";
	lpfgFile +=
		"\tpos = 0.0f;\n"
		"\twhile (pos < 1.0f) {\n"
			"\t\tpos += normalDist(inclinationSegmentLength[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType]);\n"
			"\t\tif (pos >= 1.0f) break;\n"
			"\t\tinclinationSubSegmentPositions.push_back(std::make_pair(pos, BranchType::NewInclinationSegment));\n"
		"\t}\n";

	lpfgFile += "\n\tstd::vector<std::pair<float, BranchType::Enum> > azimuthSubSegmentPositions;\n";
	lpfgFile +=
		"\tpos = 0.0f;\n"
		"\twhile (pos < 1.0f) {\n"
			"\t\tpos += normalDist(azimuthSegmentLength[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType]);\n"
			"\t\tif (pos >= 1.0f) break;\n"
			"\t\tazimuthSubSegmentPositions.push_back(std::make_pair(pos, BranchType::NewAzimuthSegment));\n"
		"\t}\n";

	// Generate curvature sign change positions (inclination and azimuth)
	lpfgFile += "\n\tstd::vector<std::pair<float, BranchType::Enum> > inclinationCurvatureSignChangePositions;\n";
	lpfgFile +=
		"\tpos = 0.0f;\n"
		"\twhile (pos < 1.0f) {\n"
			"\t\tpos += normalDist(inclinationCurvatureSignChange[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType]);\n"
			"\t\tif (pos >= 1.0f) break;\n"
			"\t\tinclinationCurvatureSignChangePositions.push_back(std::make_pair(pos, BranchType::InclinationCurvatureSignChange));\n"
		"\t}\n";

	// Generate curvature sign change positions (inclination and azimuth)
	lpfgFile += "\n\tstd::vector<std::pair<float, BranchType::Enum> > azimuthCurvatureSignChangePositions;\n";
	lpfgFile +=
		"\tpos = 0.0f;\n"
		"\twhile (pos < 1.0f) {\n"
			"\t\tpos += normalDist(azimuthCurvatureSignChange[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType]);\n"
			"\t\tif (pos >= 1.0f) break;\n"
			"\t\tazimuthCurvatureSignChangePositions.push_back(std::make_pair(pos, BranchType::AzimuthCurvatureSignChange));\n"
		"\t}\n";

	// Generate the curvature sign
	lpfgFile +=
		"\n\tint inclinationSign = inclinationCurvatureSign[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType][0];\n"
		"\tint azimuthSign = azimuthCurvatureSign[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType][0];\n";

	// Get the min/max total curvature
	lpfgFile += "\n\tfloat minInclinationCurvature = inclinationMinTotalCurvature[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType][0];\n";
	lpfgFile += "\tfloat maxInclinationCurvature = inclinationMaxTotalCurvature[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType][0];\n";

	// Output branch structure, as well as child branch "buds"

	// Start the branch
	lpfgFile +=
		"\n\tnproduce SB();\n"
		"\t++totalBranchCount;\n";

	// Set the angle
	lpfgFile +=
		"\tstd::pair<float, float> angle = std::make_pair(\n"
			"\t\tnormalDist(inclinationBranchAngle[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType]),\n"
			"\t\tnormalDist(azimuthBranchAngle[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType]));\n"
		"\tnproduce RotRel3f(angle.second, 0, 1, 0);\n"
		"\tnproduce RotRel3f(angle.first, 0, 0, 1);\n";

	// Combine all branch/curvature/etc. positions into one vector and sort
	// it. Then, progress through the vector, performing the appropriate
	// action at the appropriate position.
	lpfgFile +=
		"\n\tstd::vector<std::pair<float, BranchType::Enum> > branchActions;\n"
		"\tbranchActions.insert(branchActions.end(), subBranchGroupPositions.begin(), subBranchGroupPositions.end());\n"
		"\tbranchActions.insert(branchActions.end(), inclinationSubSegmentPositions.begin(), inclinationSubSegmentPositions.end());\n"
		"\tbranchActions.insert(branchActions.end(), azimuthSubSegmentPositions.begin(), azimuthSubSegmentPositions.end());\n"
		"\tbranchActions.insert(branchActions.end(), inclinationCurvatureSignChangePositions.begin(), inclinationCurvatureSignChangePositions.end());\n"
		"\tbranchActions.insert(branchActions.end(), azimuthCurvatureSignChangePositions.begin(), azimuthCurvatureSignChangePositions.end());\n"
		"\tstd::sort(branchActions.begin(), branchActions.end());\n";

	// Generate the branches
	lpfgFile +=
		"\n\tbranchActions.push_back(std::make_pair(1.0f, BranchType::ChildBranch));\n"
		"\tfloat currentLength, currentRadius, lengthChange, oldLength = 0.0f, oldRadius = startRadius, totalInclinationCurvature = 0.0f;\n"
		"\tfor (std::vector<std::pair<float, BranchType::Enum> >::const_iterator i = branchActions.begin(); i != branchActions.end(); ++i) {\n"
			"\t\tcurrentLength = i->first * length;\n"
			"\t\tcurrentRadius = startRadius + (endRadius - startRadius) * i->first;\n"
			"\t\tlengthChange = currentLength - oldLength;\n"
			"\t\tnproduce Cylinder(oldRadius, currentRadius, lengthChange) MovRel3f(0.0f, lengthChange, 0.0f);\n"
			"\t\toldRadius = currentRadius;\n"
			"\t\toldLength = currentLength;\n"

			"\n\t\tif (i == branchActions.end() - 1) continue;\n"

			"\n\t\tif (i->second == BranchType::ChildBranch) {\n"
				"\t\t\tsize_t planeType = childBranchPlanes[0].first;\n"
				"\t\t\tsize_t groupType = childBranchPlanes[0].second;\n"
				"\t\t\tchildBranchPlanes.erase(childBranchPlanes.begin());\n"

				"\n\t\t\tif (numGroupsInBranchingPlane[nextLevel][0][isChildSympodial][planeType][0] == 0) continue;\n"

				"\t\t\tsize_t childCount = numCurvesInGroup[nextLevel][0][isChildSympodial][planeType][groupType][0];\n"

				"\n\t\t\tfor (size_t j = 0; j < childCount; ++j) {\n"
					"\t\t\t\tnproduce Branch(nextLevel, currentRadius, length, i->first, 0, isChildSympodial, planeType, groupType, j, nextInferLevel);\n"
				"\t\t\t}\n"
			"\t\t} else if (i->second == BranchType::NewInclinationSegment) {\n"
				"\t\t\tfloat curvature = normalDist(inclinationCurvature[branchLevel][isChildSympodial][isParentSympodial][childBranchingPlane][childBranchingGroup][childCurveType]);\n"

				"\n\t\t\tfloat desiredTotalCurvature = totalInclinationCurvature + curvature * inclinationSign;\n"
				"\t\t\tif (desiredTotalCurvature > maxInclinationCurvature || desiredTotalCurvature < minInclinationCurvature) curvature *= " CURVATURE_BOUNDARY_NUDGE ";\n"

				"\n\t\t\tcurvature *= inclinationSign;\n"
				"\t\t\tnproduce RotRel3f(curvature, 0, 0, 1);\n"

				"\n\t\t\ttotalInclinationCurvature += curvature;\n"
			"\t\t} else if (i->second == BranchType::NewAzimuthSegment) {\n"
					// Azimuth is currently not implemented for curves
			"\t\t} else if (i->second == BranchType::InclinationCurvatureSignChange) {\n"
				"\t\t\tinclinationSign *= -1;\n"
			"\t\t} else if (i->second == BranchType::AzimuthCurvatureSignChange) {\n"
				"\t\t\tazimuthSign *= -1;\n"
			"\t\t}\n"
		"\t}\n";

	// Output sympodial branch "buds"
	lpfgFile +=
		"\tfor (int i = 0; i < sympodialBranchCount; ++i) {\n"
			"\t\tnproduce Branch(nextLevel, endRadius, length, 1, 1.0f, isChildSympodial, 0, sympodialSubBranchGroup, i, nextInferLevel);\n"
		"\t}\n";

	// End the branch
	lpfgFile +=
		"\n\tproduce EB();\n"
		"}\n";
}

void appendLookupFunctions(const vector<BranchLevel> &branchLevels, string &lpfgFile) {
	lpfgFile +=
		"\nint getSubBranchGroupCount(int branchLevel, int isParentSympodial) {\n"
			"\tint subBranchGroupCount;\n";

	for (size_t i = 0; i < branchLevels.size(); ++i) {
		lpfgFile +=
			"\tif (branchLevel == " + toString(i) + ") {\n"
				"\t\tif (isParentSympodial) {\n" +
					constructPiecewiseConstantGenerator(branchLevels[i].numGroupSplits[1], "subBranchGroupCount", "\t\t\t") +
				"\t\t} else {\n" +
					constructPiecewiseConstantGenerator(branchLevels[i].numGroupSplits[0], "subBranchGroupCount", "\t\t\t") +
				"\t\t}\n"
				"\t\treturn subBranchGroupCount;\n"
			"\t}\n";
	}

	lpfgFile += "}\n";

	lpfgFile +=
		"\nint getChildBranchType(int branchLevel, int isParentSympodial) {\n"
			"\tint childBranchType;\n";

	for (size_t i = 0; i < branchLevels.size(); ++i) {
		lpfgFile +=
			"\tif (branchLevel == " + toString(i) + ") {\n"
				"\t\tif (isParentSympodial) {\n" +
					constructPiecewiseConstantGenerator(branchLevels[i].childBranchProbability[1], "childBranchType", "\t\t\t") +
				"\t\t} else {\n" +
					constructPiecewiseConstantGenerator(branchLevels[i].childBranchProbability[0], "childBranchType", "\t\t\t") +
				"\t\t}\n"
				"\t\treturn childBranchType;\n"
			"\t}\n";
	}

	lpfgFile += "}\n";
}

void appendLookupTables(const vector<BranchLevel> &branchLevels, string &lpfgFile) {
	static const vector<BranchLevel> *levels = &branchLevels;

	// General lookup tables to branch levels

	// Branch split start point
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();						// Level
				else return 2;												// Monopodial/sympodial parent?
			}
			static string get(const size_t *idx) { return "{" + constructNormalArgs((*levels)[idx[0]].splitStartPoint[idx[1]]) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "splitStartPoint", 3, 4, Lookup::size, Lookup::get);
	}

	// Branch child group distribution
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();						// Level
				else return 2;												// Monopodial/sympodial parent?
			}
			static string get(const size_t *idx) { return "{" + constructNormalArgs((*levels)[idx[0]].childGroupDistribution[idx[1]]) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "childGroupDistribution", 3, 4, Lookup::size, Lookup::get);
	}

	// Branch groups per unit length
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();						// Level
				else return 2;												// Monopodial/sympodial parent?
			}
			static string get(const size_t *idx) { return "{" + toString((*levels)[idx[0]].groupsPerUnitLength[idx[1]]) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "childGroupsPerUnitLength", 3, 1, Lookup::size, Lookup::get);
	}

	// These are lookup tables for curve information

	// Number of branching planes
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();									// Level
				else if (idxLen == 1) return 2;											// Monopodial/sympodial child?
				else return 2;															// Monopodial/sympodial parent?
			}
			static string get(const size_t *idx) { return "{" + toString((*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size()) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("int", "numBranchingPlanes", 4, 1, Lookup::size, Lookup::get);
	}

	// Number of groups in a branching plane
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();									// Level
				else if (idxLen == 1) return 2;											// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;											// Monopodial/sympodial parent?
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();	// Branching plane
			}
			static string get(const size_t *idx) { return "{" + toString((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size()) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("int", "numGroupsInBranchingPlane", 5, 1, Lookup::size, Lookup::get);
	}

	// Group sizes
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();													// Level
				else if (idxLen == 1) return 2;															// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;															// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();		// Branching plane
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();			// Branching group
			}
			static string get(const size_t *idx) { return "{" + toString((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size()) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("int", "numCurvesInGroup", 6, 1, Lookup::size, Lookup::get);
	}

	// For calculating branch angles (inclination/azimuth)
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																	// Level
				else if (idxLen == 1) return 2;																			// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																			// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();						// Branching plane
				else if (idxLen == 4) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();			// Branch angle groups
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size();					// Curve
			}
			static string get(const size_t *idx) { return "{" + constructNormalArgs((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]][idx[5]].angle.azimuth) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "azimuthBranchAngle", 7, 4, Lookup::size, Lookup::get);
	}
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																	// Level
				else if (idxLen == 1) return 2;																			// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																			// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();						// Branching plane
				else if (idxLen == 4) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();			// Branch angle groups
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size();					// Curve
			}
			static string get(const size_t *idx) { return "{" + constructNormalArgs((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]][idx[5]].angle.inclination) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "inclinationBranchAngle", 7, 4, Lookup::size, Lookup::get);
	}

	// For calculating branch start radius
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																	// Level
				else if (idxLen == 1) return 2;																			// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																			// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();						// Branching plane
				else if (idxLen == 4) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();			// Branch angle groups
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size();					// Curve
			}
			static string get(const size_t *idx) { return "{" + constructNormalArgs((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]][idx[5]].startRadius) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "branchStartRadius", 7, 4, Lookup::size, Lookup::get);
	}

	// For calculating branch end radius
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																	// Level
				else if (idxLen == 1) return 2;																			// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																			// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();						// Branching plane
				else if (idxLen == 4) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();			// Branch angle groups
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size();					// Curve
			}
			static string get(const size_t *idx) { return "{" + constructNormalArgs((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]][idx[5]].endRadius) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "branchEndRadius", 7, 4, Lookup::size, Lookup::get);
	}

	// For calculating branch length
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																	// Level
				else if (idxLen == 1) return 2;																			// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																			// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();						// Branching plane
				else if (idxLen == 4) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();			// Branch angle groups
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size();					// Curve
			}
			static string get(const size_t *idx) { return "{" + constructNormalArgs((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]][idx[5]].length) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "branchLength", 7, 4, Lookup::size, Lookup::get);
	}

	// For calculating branch original parent length
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																	// Level
				else if (idxLen == 1) return 2;																			// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																			// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();						// Branching plane
				else if (idxLen == 4) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();			// Branch angle groups
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size();					// Curve
			}
			static string get(const size_t *idx) { return "{" + constructNormalArgs((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]][idx[5]].originalParentLength) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "branchOriginalParentLength", 7, 4, Lookup::size, Lookup::get);
	}

	// For calculating branch original parent start radius
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																	// Level
				else if (idxLen == 1) return 2;																			// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																			// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();						// Branching plane
				else if (idxLen == 4) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();			// Branch angle groups
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size();					// Curve
			}
			static string get(const size_t *idx) { return "{" + constructNormalArgs((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]][idx[5]].originalParentStartRadius) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "branchOriginalParentStartRadius", 7, 4, Lookup::size, Lookup::get);
	}

	// For calculating branch original parent end radius
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																	// Level
				else if (idxLen == 1) return 2;																			// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																			// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();						// Branching plane
				else if (idxLen == 4) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();			// Branch angle groups
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size();					// Curve
			}
			static string get(const size_t *idx) { return "{" + constructNormalArgs((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]][idx[5]].originalParentEndRadius) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "branchOriginalParentEndRadius", 7, 4, Lookup::size, Lookup::get);
	}

	// For calculating the next segment length (inclination/azimuth)
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																	// Level
				else if (idxLen == 1) return 2;																			// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																			// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();						// Branching plane
				else if (idxLen == 4) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();			// Branch angle groups
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size();					// Curve
			}
			static string get(const size_t *idx) { return "{" + constructNormalArgs((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]][idx[5]].curveData.segmentLengths.first) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "inclinationSegmentLength", 7, 4, Lookup::size, Lookup::get);
	}
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																	// Level
				else if (idxLen == 1) return 2;																			// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																			// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();					// Branching plane
				else if (idxLen == 4) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();	// Branch angle groups
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size();				// Curve
			}
			static string get(const size_t *idx) { return "{" + constructNormalArgs((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]][idx[5]].curveData.segmentLengths.second) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "azimuthSegmentLength", 7, 4, Lookup::size, Lookup::get);
	}

	// For calculating the next curvature sign change (inclination/azimuth)
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																	// Level
				else if (idxLen == 1) return 2;																			// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																			// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();					// Branching plane
				else if (idxLen == 4) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();	// Branch angle groups
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size();				// Curve
			}
			static string get(const size_t *idx) { return "{" + constructNormalArgs((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]][idx[5]].curveData.curvatureSignChangeDist.first) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "inclinationCurvatureSignChange", 7, 4, Lookup::size, Lookup::get);
	}
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																	// Level
				else if (idxLen == 1) return 2;																			// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																			// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();					// Branching plane
				else if (idxLen == 4) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();	// Branch angle groups
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size();				// Curve
			}
			static string get(const size_t *idx) { return "{" + constructNormalArgs((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]][idx[5]].curveData.curvatureSignChangeDist.second) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "azimuthCurvatureSignChange", 7, 4, Lookup::size, Lookup::get);
	}

	// For calculating the initial curvature sign (inclination/azimuth)
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																	// Level
				else if (idxLen == 1) return 2;																			// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																			// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();					// Branching plane
				else if (idxLen == 4) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();	// Branch angle groups
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size();				// Curve
			}
			static string get(const size_t *idx) { return "{" + toString((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]][idx[5]].curveData.startCurvatureSign.first) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("int", "inclinationCurvatureSign", 7, 1, Lookup::size, Lookup::get);
	}
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																	// Level
				else if (idxLen == 1) return 2;																			// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																			// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();					// Branching plane
				else if (idxLen == 4) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();	// Branch angle groups
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size();				// Curve
			}
			static string get(const size_t *idx) { return "{" + toString((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]][idx[5]].curveData.startCurvatureSign.second) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("int", "azimuthCurvatureSign", 7, 1, Lookup::size, Lookup::get);
	}

	// For calculating the inclination curvature
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																	// Level
				else if (idxLen == 1) return 2;																			// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																			// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();					// Branching plane
				else if (idxLen == 4) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();	// Branch angle groups
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size();				// Curve
			}
			static string get(const size_t *idx) { return "{" + constructNormalArgs((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]][idx[5]].curveData.curvatureMagnitude.first) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "inclinationCurvature", 7, 4, Lookup::size, Lookup::get);
	}

	// For calculating the inclination total curvature (min/max)
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																	// Level
				else if (idxLen == 1) return 2;																			// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																			// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();					// Branching plane
				else if (idxLen == 4) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();	// Branch angle groups
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size();				// Curve
			}
			static string get(const size_t *idx) { return "{" + toString((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]][idx[5]].curveData.minTotalCurvature.first) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "inclinationMinTotalCurvature", 7, 1, Lookup::size, Lookup::get);
	}
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																	// Level
				else if (idxLen == 1) return 2;																			// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																			// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]].size();					// Branching plane
				else if (idxLen == 4) return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups.size();	// Branch angle groups
				else return (*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]].size();				// Curve
			}
			static string get(const size_t *idx) { return "{" + toString((*levels)[idx[0]].curveInfo[idx[1]][idx[2]][idx[3]].groups[idx[4]][idx[5]].curveData.maxTotalCurvature.first) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "inclinationMaxTotalCurvature", 7, 1, Lookup::size, Lookup::get);
	}

	// Lookup tables for specific example branches

	// Number of example branches at a level
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();									// Level
				else if (idxLen == 1) return 2;											// Monopodial/sympodial child?
				else return 2;															// Monopodial/sympodial parent?
			}
			static string get(const size_t *idx) { return "{" + toString((*levels)[idx[0]].exampleBranchInfo[idx[1]][idx[2]].size()) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("int", "numExampleBranches", 4, 1, Lookup::size, Lookup::get);
	}

	// Number of groups in an example branch
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();														// Level
				else if (idxLen == 1) return 2;																// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].exampleBranchInfo[idx[1]][idx[2]].size();	// Which example branch?
				else return 2;																				// Monopodial/sympodial child branch groups of the example branch?
			}
			static string get(const size_t *idx) { return "{" + toString((*levels)[idx[0]].exampleBranchInfo[idx[1]][idx[2]][idx[3]].childGroups[idx[4]].size()) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("int", "numExampleBranchGroups", 6, 1, Lookup::size, Lookup::get);
	}

	// Position of the example child branch group
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																// Level
				else if (idxLen == 1) return 2;																		// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																		// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].exampleBranchInfo[idx[1]][idx[2]].size();			// Which example branch?
				else if (idxLen == 4) return 2;															    		// Monopodial/sympodial child branch groups of the example branch?
				else return (*levels)[idx[0]].exampleBranchInfo[idx[1]][idx[2]][idx[3]].childGroups[idx[4]].size();	// Which branch group?
			}
			static string get(const size_t *idx) { return "{" + toString((*levels)[idx[0]].exampleBranchInfo[idx[1]][idx[2]][idx[3]].childGroups[idx[4]][idx[5]].position) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("float", "exampleBranchGroupPosition", 7, 1, Lookup::size, Lookup::get);
	}

	// Branching plane of the example child branch group
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																// Level
				else if (idxLen == 1) return 2;																		// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																		// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].exampleBranchInfo[idx[1]][idx[2]].size();			// Which example branch?
				else if (idxLen == 4) return 2;															    		// Monopodial/sympodial child branch groups of the example branch?
				else return (*levels)[idx[0]].exampleBranchInfo[idx[1]][idx[2]][idx[3]].childGroups[idx[4]].size();	// Which branch group?
			}
			static string get(const size_t *idx) { return "{" + toString((*levels)[idx[0]].exampleBranchInfo[idx[1]][idx[2]][idx[3]].childGroups[idx[4]][idx[5]].branchingPlane) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("int", "exampleBranchGroupBranchingPlane", 7, 1, Lookup::size, Lookup::get);
	}

	// Branching group of the example child branch group
	{
		class Lookup {
		public:
			static size_t size(const size_t *idx, size_t idxLen) {
				if (idxLen == 0) return levels->size();																// Level
				else if (idxLen == 1) return 2;																		// Monopodial/sympodial child?
				else if (idxLen == 2) return 2;																		// Monopodial/sympodial parent?
				else if (idxLen == 3) return (*levels)[idx[0]].exampleBranchInfo[idx[1]][idx[2]].size();			// Which example branch?
				else if (idxLen == 4) return 2;															    		// Monopodial/sympodial child branch groups of the example branch?
				else return (*levels)[idx[0]].exampleBranchInfo[idx[1]][idx[2]][idx[3]].childGroups[idx[4]].size();	// Which branch group?
			}
			static string get(const size_t *idx) { return "{" + toString((*levels)[idx[0]].exampleBranchInfo[idx[1]][idx[2]][idx[3]].childGroups[idx[4]][idx[5]].branchingGroup) + "}"; }
		};
		lpfgFile += "\n" + constructLookupTable("int", "exampleBranchGroupNumber", 7, 1, Lookup::size, Lookup::get);
	}
}

void appendRandomisationFunctions(string &lpfgFile) {
	lpfgFile += "\nboost::mt19937 rng;\n";

	lpfgFile += "\nfloat uniformDist(float minVal, float maxVal) {\n"
			"\tif (minVal > maxVal) return uniformDist(maxVal, minVal);\n"
			"\tboost::uniform_real<float> dist(minVal, maxVal);\n"
			"\treturn dist(rng);\n"
		"}\n";

	lpfgFile += "\nfloat uniformDist(const float *vals) {\n"
			"\treturn uniformDist(vals[0], vals[1]);\n"
		"}\n";

	lpfgFile += "\nfloat uniformDist(int minVal, int maxVal) {\n"
			"\tif (minVal > maxVal) return uniformDist(maxVal, minVal);\n"
			"\tboost::uniform_int<int> dist(minVal, maxVal);\n"
			"\treturn dist(rng);\n"
		"}\n";

	lpfgFile += "\nfloat uniformDist(const int *vals) {\n"
			"\treturn uniformDist(vals[0], vals[1]);\n"
		"}\n";

	lpfgFile += "\nptrdiff_t uniformDistExcl(ptrdiff_t maxVal) {\n"
			"\treturn uniformDist(0, maxVal - 1);\n"
		"}\n";

	lpfgFile += "\nfloat normalDist(float mean, float stddev, float minVal, float maxVal) {\n"
			"\tif (minVal > maxVal) return normalDist(mean, stddev, maxVal, minVal);\n"
			"\tboost::normal_distribution<float> dist(mean, stddev);\n"
			"\tboost::variate_generator<boost::mt19937&, boost::normal_distribution<float> > var(rng, dist);\n"
			"\tfloat val = var();\n"
			"\tif (val < minVal) return minVal;\n"
			"\tif (val > maxVal) return maxVal;\n"
			"\treturn val;\n"
		"}\n";

	lpfgFile += "\nfloat normalDist(const float *vals) {\n"
			"\treturn normalDist(vals[0], vals[1], vals[2], vals[3]);\n"
		"}\n";

	lpfgFile += "\nint piecewiseConstDist(const int *start, const int *end, const float *weights) {\n"
			"\tif (start == end) return 0.0f;\n"
			"\tfloat totalWeight = 0;\n"
			"\tfor (int i = 0; start + i != end; ++i) {\n"
				"\t\ttotalWeight += weights[i];\n"
			"\t}\n"
			"\tfloat rand = uniformDist(0.0f, totalWeight);\n"
			"\tfor (int i = 0; start + i != end; ++i) {\n"
				"\t\tif (rand < weights[i]) {\n"
					"\t\t\treturn *(start + i);\n"
				"\t\t}\n"
				"\t\trand -= weights[i];\n"
			"\t}\n"
		"}\n";
}

string constructPiecewiseConstantGenerator(const DiscreteDist &discreteDist, const string &varName, const string &lineStart) {
	int intervalCount = 0;
	string intervalString;
	string weightString;
	for (map<int, float>::const_iterator j = discreteDist.values.begin(); j != discreteDist.values.end(); ++j) {
		++intervalCount;

		if (intervalString != "") intervalString += ", ";
		intervalString += toString(j->first);

		if (weightString != "") weightString += ", ";
		weightString += toString(j->second);
	}

	return
		lineStart + "{\n" +
		lineStart + "\tstatic int intervals[] = {" + intervalString + "};\n" +
		lineStart + "\tstatic float weights[] = {" + weightString + "};\n" +
		lineStart + "\t" + varName + " = piecewiseConstDist(intervals, intervals + " + toString(intervalCount) + ", weights);\n" +
		lineStart + "}\n";
}

string constructNormalArgs(const NormalDist &norm) {
	return toString(norm.normalStat.getMean()) + ", " + toString(norm.normalStat.getStandardDeviation()) + ", " + toString(norm.min) + ", " + toString(norm.max);
}

// Construct a float lookup table.
//
// dims must be greater than 0, and gives the dimensions of the lookup table.
// size should return the size of the lookup table for index idx (with length
// given by idxLen). idxLen will always be less than dims.
// get should return a string representing a 1-dimensional float array
// at position idx, which has length (dims - 1).
//
// The reason for the size function pointer is to allow for jagged arrays.
//
// For example, consider the following definitions of size and get:
//   int size(const int *idx, int idxLen) {
//     switch (idxLen) {
//       case 0: return 2; break;
//       case 1: return 3; break;
//     }
//   }
//
//   string get(const int *idx) {
//     return "{1, 1}";
//   }
//
// Then calling constructLookupTable("float", "lookup", 3, 2, size, get) would return the
// following string:
//
//   "static const float lookup[][3][2] = {{{1, 1}, {1, 1}, {1, 1}}, {{1, 1}, {1, 1}, {1, 1}}};\n"
//
string constructLookupTable(const string &tableType, const string &tableName,
		size_t dims, size_t finalDimElements,
		size_t (*size)(const size_t *idx, size_t idxLen),
		string (*get)(const size_t *idx)) {
	string lookup = "static const " + tableType + " " + tableName;

	if (dims == 1 && finalDimElements == 0) return lookup + "[] = {};\n";

	size_t *idx = new size_t[dims - 1];

	// Calculate the maximum number of elements for each dimension
	for (size_t i = 0; i < dims - 1; ++i) {
		lookup += "[" + toString(lookupTableMaxElements(idx, 0, i, size))+ "]";
	}
	if ( dims > 1) lookup += "[" + toString(finalDimElements) + "]";

	// Calculate the actual elements
	lookup += " = ";
	constructLookupTable(lookup, idx, 0, dims, size, get);
	lookup += ";\n";

	delete [] idx;

	return lookup;
}

size_t lookupTableMaxElements(size_t *idx, size_t current, size_t depth,
		size_t (*size)(const size_t *idx, size_t idxLen)) {
	if (current == depth) return size(idx, current);

	size_t maxVal = 0;
	for (idx[current] = 0; idx[current] < size(idx, current); ++idx[current]) {
		maxVal = max(maxVal, lookupTableMaxElements(idx, current + 1, depth, size));
	}

	return maxVal;
}

void constructLookupTable(string &lookup, size_t *idx, size_t current, size_t dims,
		size_t (*size)(const size_t *idx, size_t idxLen),
		string (*get)(const size_t *idx)) {
	if (current == dims - 1) {
		lookup += get(idx);
		return;
	}

	lookup += "{";
	for (idx[current] = 0; idx[current] < size(idx, current); ++idx[current]) {
		constructLookupTable(lookup, idx, current + 1, dims, size, get);
		if (idx[current] < size(idx, current) - 1) lookup += ", ";
	}
	lookup += "}";
}
