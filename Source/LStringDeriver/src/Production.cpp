#include "Production.h"

using namespace std;

bool equal(const vector<ModuleInstance> &a, const vector<ModuleInstance> &b);
bool lessThan(const vector<ModuleInstance> &a, const vector<ModuleInstance> &b);

Production::Production() {
}

Production::Production(const Production &production) {
	this->leftContext = production.leftContext;
	this->predecessor = production.predecessor;
	this->rightContext = production.rightContext;

	this->body = production.body;
	this->name = production.name;
}

Production::~Production() {
}

bool Production::operator==(const Production &production) const {
	return equal(this->leftContext, production.leftContext) &
	       equal(this->predecessor, production.predecessor) &
	       equal(this->rightContext, production.rightContext);
}

bool equal(const vector<ModuleInstance> &a, const vector<ModuleInstance> &b) {
	if (a.size() != b.size()) return false;
	for (size_t i = 0; i < a.size(); ++i) {
		if (a[i].arguments.size() != b[i].arguments.size()) return false;
		if (a[i].moduleName != b[i].moduleName) return false;
	}
	return true;
}

bool Production::operator!=(const Production &production) const {
	return !(*this==production);
}

bool Production::operator<(const Production &production) const {
	if (lessThan(this->leftContext, production.leftContext))
		return true;
	else if (lessThan(production.leftContext, this->leftContext))
		return false;

	if (lessThan(this->predecessor, production.predecessor))
		return true;
	else if (lessThan(production.predecessor, this->predecessor))
		return false;

	if (lessThan(this->rightContext, production.rightContext))
		return true;
	else if (lessThan(production.rightContext, this->rightContext))
		return false;

	return false;
}

bool lessThan(const vector<ModuleInstance> &a, const vector<ModuleInstance> &b) {
	if (a.size() < b.size()) return true;
	else if (b.size() < a.size()) return false;

	for (size_t i = 0; i < a.size(); ++i) {
		if (a[i].moduleName < b[i].moduleName) return true;
		else if (b[i].moduleName < a[i].moduleName) return false;

		if (a[i].arguments.size() < b[i].arguments.size()) return true;
		else if (b[i].arguments.size() < a[i].arguments.size()) return false;
	}

	return false;
}
