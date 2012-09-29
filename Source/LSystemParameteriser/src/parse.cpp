#include <algorithm>
#include <iostream>
#include <math.h>
#include <utility>

#include "BranchSegment.h"
#include "parse.h"
#include "util.h"

using namespace rapidxml;
using namespace std;

#define APPROX_EQUAL_AMOUNT 0.05f
#define RADIUS_SEGMENT_LEEWAY 0.2f

void parseTree(Branch &branch, const xml_node<> *node);
void moveStartBranchesToParent(Branch &branch, vector<Branch> *newChildren);
void calculateSegments(Branch &branch);
void calculateSympodialChildren(Branch &branch);

float parseFloat(const char *value, bool positive);
pair<float, float> parseMinMaxPair(const xml_node<> *node, bool positive);

void parseTree(Branch &branch, const xml_document<> &doc) {
	parseTree(branch, doc.first_node("branch", 0, false));
	moveStartBranchesToParent(branch, 0);
	calculateSegments(branch);
	calculateSympodialChildren(branch);
}

void parseTree(Branch &branch, const xml_node<> *node) {
	branch.segments.clear();
	branch.children.clear();
	branch.sympodialChildren.clear();

	// Position
	xml_node<> *positionNode = node->first_node("positionOnParent", 0, false);
	branch.position = parseFloat(positionNode->value(), true);

	// Whether or not this branch is a sub-segment
	xml_node<> *isSubSegmentNode = node->first_node("isSubSegment", 0, false);
	if (isSubSegmentNode) branch.isSubSegment = atob(isSubSegmentNode->value());

	BranchSegment rootSegment;

	// Angle
	xml_node<> *angleNode = node->first_node("angleToParent", 0, false);
	xml_node<> *inclinationNode = angleNode->first_node("inclination", 0, false);
	rootSegment.inclination = parseMinMaxPair(inclinationNode, false);
	xml_node<> *azimuthNode = angleNode->first_node("azimuth", 0, false);
	rootSegment.azimuth = parseMinMaxPair(azimuthNode, false);

	// Start radius
	xml_node<> *startRadiusNode = node->first_node("thickness", 0, false)->first_node("start", 0, false);
	rootSegment.startRadius = parseMinMaxPair(startRadiusNode, true);

	// End radius
	xml_node<> *endRadiusNode = node->first_node("thickness", 0, false)->first_node("end", 0, false);
	rootSegment.endRadius = parseMinMaxPair(endRadiusNode, true);

	// Length
	xml_node<> *lengthNode = node->first_node("length", 0, false);
	rootSegment.length = parseMinMaxPair(lengthNode, true);

	branch.segments.push_back(rootSegment);

	// Read children
	xml_node<> *childrenNode = node->first_node("children", 0, false);
	for (xml_node<> *childNode = childrenNode->first_node("branch"); childNode; childNode = childNode->next_sibling("branch")) {
		Branch child;
		parseTree(child, childNode);
		branch.children.push_back(child);
	}

	// Remove any duplicate children
	for (size_t i = 0; i < branch.children.size(); ++i) {
		for (size_t j = i + 1; j < branch.children.size(); ++j) {
			if (branch.children[i] == branch.children[j]) {
				branch.children.erase(branch.children.begin() + j);
				--j;
			}
		}
	}
}

void moveStartBranchesToParent(Branch &branch, vector<Branch> *newChildren) {
	vector<Branch> children = branch.children;
	branch.children.clear();
	while (children.size() > 0) {
		Branch child = children[0];
		children.erase(children.begin());

		moveStartBranchesToParent(child, &children);
		branch.children.push_back(child);
	}
	// If we have a parent, check for branches near the start of this branch.
	// If there are any, move them onto the parent at our position on the
	// parent.
	if (newChildren) {
		for (size_t i = 0; i < branch.children.size(); ++i) {
			if (approxEqual(branch.children[i].position, 0.0f, APPROX_EQUAL_AMOUNT)) {
				Branch child = branch.children[i];
				branch.children.erase(branch.children.begin() + i);

				child.position = branch.position;

				/*child.segments[0].inclination.first += branch.segments[0].inclination.first;
				child.segments[0].inclination.second += branch.segments[0].inclination.second;
				child.segments[0].azimuth.first += branch.segments[0].azimuth.first;
				child.segments[0].azimuth.second += branch.segments[0].azimuth.second;

				anglesInRange(child.segments[0].inclination.first, child.segments[0].azimuth.first);
				anglesInRange(child.segments[0].inclination.second, child.segments[0].azimuth.second);*/

				newChildren->push_back(child);
				--i;
			}
		}
	}
}

void calculateSegments(Branch &branch) {
	for (size_t i = 0; i < branch.children.size(); ++i) {
		calculateSegments(branch.children[i]);
	}

	// Go through the children, and establish candidates for further segments in the current branch
	vector<Branch>::iterator nextSegment = branch.children.end();
	int matches = 0;

	for (vector<Branch>::iterator i = branch.children.begin(); i != branch.children.end(); ++i) {
		// We define another segment in the branch as a sympodial child that
		// has position close to 1 (i.e. it appears near the end of the current
		// branch), and the child's start radius is close to the parent's end radius.
		// If, however, the branch is marked as the next sub-segment, we take
		// it as the next sub-segment, regardless.
		float endRadiusAverage = average(branch.segments[0].endRadius);
		float startRadiusAverage = average(i->segments[0].startRadius);
		if (i->isSubSegment) {
			nextSegment = i;
			matches = 1;
			break;
		} else if (approxEqual(i->position, 1.0f, APPROX_EQUAL_AMOUNT) &&
				(endRadiusAverage < startRadiusAverage ||
						approxEqualRatio(startRadiusAverage, endRadiusAverage, RADIUS_SEGMENT_LEEWAY))) {
			nextSegment = i;
			++matches;
		}
	}

	// If there is a single valid candidate for the next branch segment,
	// merge it with the current branch.
	// If there is still more than a single candidate, ignore them all.
	if (matches == 1) {
		Branch nextBranch = *nextSegment;
		branch.children.erase(nextSegment);

		vector<Branch> newChildren = nextBranch.children;
		newChildren.insert(newChildren.end(), nextBranch.sympodialChildren.begin(), nextBranch.sympodialChildren.end());
		vector<BranchSegment> newSegments = nextBranch.segments;

		nextBranch.children.clear();
		nextBranch.sympodialChildren.clear();
		nextBranch.segments.clear();

		// Recalculate position of all children, both for the current branch, and
		// for the segment we're adding
		pair<float, float> branchLen, nextBranchLen, totalLen;

		branchLen = branch.segments[0].length;

		// Calculate the new length
		nextBranchLen = make_pair(0.0f, 0.0f);
		for (size_t i = 0; i < newSegments.size(); ++i) {
			nextBranchLen.first += newSegments[i].length.first;
			nextBranchLen.second += newSegments[i].length.second;
		}

		totalLen = make_pair(branchLen.first + nextBranchLen.first, branchLen.second + nextBranchLen.second);
		if (totalLen.first == 0.0f) totalLen.first = 1.0f;
		if (totalLen.second == 0.0f) totalLen.second = 1.0f;

		// Modify all child positions to reflect that they are now on a larger branch
		for (size_t i = 0; i < branch.children.size(); ++i) {
			branch.children[i].position = average(
					branch.children[i].position * branchLen.first / totalLen.first,
					branch.children[i].position * branchLen.second / totalLen.second);

		}

		for (size_t i = 0; i < newChildren.size(); ++i) {
			newChildren[i].position = average(
					(branchLen.first + newChildren[i].position * nextBranchLen.first) / totalLen.first,
					(branchLen.second + newChildren[i].position * nextBranchLen.second) / totalLen.second);
		}

		// Add the child's branches to ours
		branch.children.insert(branch.children.end(), newChildren.begin(), newChildren.end());
		branch.segments.insert(branch.segments.end(), newSegments.begin(), newSegments.end());
	}
}

void calculateSympodialChildren(Branch &branch) {
	for (size_t i = 0; i < branch.children.size(); ++i) {
		calculateSympodialChildren(branch.children[i]);
	}

	size_t numCandidates = 0;
	for (size_t i = 0; i < branch.children.size(); ++i) {
		if (approxEqual(branch.children[i].position, 1.0f, APPROX_EQUAL_AMOUNT)) {
			++numCandidates;
		}
	}

	if (numCandidates > 1) {
		// Calculate sympodial children
		for (size_t i = 0; i < branch.children.size(); ++i) {
			if (approxEqual(branch.children[i].position, 1.0f, APPROX_EQUAL_AMOUNT)) {
				branch.sympodialChildren.push_back(branch.children[i]);
				branch.children.erase(branch.children.begin() + i--);
			}
		}
	}

	// Finally, mark child/sympodial branches correctly
	for (vector<Branch>::iterator i = branch.children.begin(); i != branch.children.end(); ++i) {
		i->isSympodialChild = false;
	}
	for (vector<Branch>::iterator i = branch.sympodialChildren.begin(); i != branch.sympodialChildren.end(); ++i) {
		i->isSympodialChild = true;
	}
}

pair<float, float> parseMinMaxPair(const xml_node<> *node, bool positive) {
	float first = parseFloat(node->first_node("min", 0, false)->value(), positive);
	float second = parseFloat(node->first_node("max", 0, false)->value(), positive);

	return make_pair(first, second);
}

float parseFloat(const char *value, bool positive) {
	float num = atof(value);

	if (isnan(num)) error("expected number, received nan", true);
	if (positive && num < 0) error("unexpected negative value", true);

	return num;
}
