#ifndef MODULEINSTANCE_H_
#define MODULEINSTANCE_H_

#include <string>
#include <vector>

#include "Module.h"

class ModuleInstance {
public:
	ModuleInstance();
	ModuleInstance(const ModuleInstance &moduleInstance);
	virtual ~ModuleInstance();

	std::string moduleName;
	std::vector<std::string> arguments;

	Module module;
};

#endif /* MODULEINSTANCE_H_ */
