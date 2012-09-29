#ifndef CONSTRUCT_H_
#define CONSTRUCT_H_

#include <string>
#include <vector>

#include "BranchLevel.h"

void constructLpfgFile(const std::vector<BranchLevel> &branchLevels, std::string &lpfgFile,
		int derivationLength,
		float monopodialScalingFactor, float sympodialScalingFactor,
		float minRadius, float minLength, int branchLimit);

#endif /* CONSTRUCT_H_ */
