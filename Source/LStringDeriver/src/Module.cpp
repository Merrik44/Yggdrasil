#include "Module.h"

Module::Module() {
}

Module::Module(const Module &module) {
	this->name = module.name;
	this->parameterTypes = module.parameterTypes;
}

Module::~Module() {
}

bool Module::operator==(const Module &module) const {
	if (this->name != module.name) return false;

	return true;
}

bool Module::operator!=(const Module &module) const {
	return !(*this==module);
}

bool Module::operator<(const Module &module) const {
	return this->name < module.name;
}
