#ifndef EXTRACT_H_
#define EXTRACT_H_

#include <vector>

#include "Branch.h"
#include "BranchLevel.h"

void extractData(Branch &branch, std::vector<BranchLevel> &levels, size_t inferenceLevels, size_t propagation, bool storeRootBranchStructure);

#endif /* EXTRACT_H_ */
