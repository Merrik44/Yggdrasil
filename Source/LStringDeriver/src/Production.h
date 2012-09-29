#ifndef PRODUCTION_H_
#define PRODUCTION_H_

#include <string>
#include <vector>

#include "ModuleInstance.h"

class Production {
public:
	Production();
	Production(const Production &production);
	virtual ~Production();

	bool operator==(const Production &production) const;
	bool operator!=(const Production &production) const;
	bool operator<(const Production &production) const;

	std::vector<ModuleInstance> leftContext;
	std::vector<ModuleInstance> predecessor;
	std::vector<ModuleInstance> rightContext;

	std::string name;
	std::string body;
};

#endif /* PRODUCTION_H_ */
