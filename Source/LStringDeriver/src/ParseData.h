#ifndef PARSE_DATA_H_
#define PARSE_DATA_H_

#include <set>
#include <string>
#include <vector>

#include "Module.h"
#include "ModuleInstance.h"
#include "Production.h"

struct ParseData {
	std::vector<ModuleInstance> axiom;
	std::string derivationLength;
	std::string maximumDepthDecomposition;
	std::string maximumDepthInterpretation;
	std::set<Module> modules;
	std::vector<Production> productions;
	std::vector<Production> decompositions;
	std::string startBody;
	std::string startEachBody;
	std::string endBody;
	std::string endEachBody;
	std::string transformedString;
};

#endif /* PARSE_DATA_H_ */
