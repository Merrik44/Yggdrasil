#include "ModuleInstance.h"

ModuleInstance::ModuleInstance() {
}

ModuleInstance::ModuleInstance(const ModuleInstance &moduleInstance) {
	this->moduleName = moduleInstance.moduleName;
	this->arguments = moduleInstance.arguments;
	this->module = moduleInstance.module;
}

ModuleInstance::~ModuleInstance() {
}
