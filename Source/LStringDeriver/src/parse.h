#ifndef PARSE_H_
#define PARSE_H_

#include <string>
#include <set>
#include <vector>

#include "Module.h"
#include "ModuleInstance.h"
#include "ParseData.h"
#include "Production.h"

bool parseString(const std::string &lpfgString, ParseData &parseData);

#endif /* PARSE_H_ */
