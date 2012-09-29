#ifndef MODULE_H_
#define MODULE_H_

#include <string>
#include <vector>

class Module {
public:
	Module();
	Module(const Module &module);
	virtual ~Module();

	bool operator==(const Module &module) const;
	bool operator!=(const Module &module) const;
	bool operator<(const Module &module) const;

	std::string name;
	std::vector<std::string> parameterTypes;
};

#endif /* MODULE_H_ */
