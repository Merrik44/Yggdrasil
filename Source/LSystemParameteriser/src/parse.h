#ifndef PARSE_H_
#define PARSE_H_

#include <rapidxml.hpp>

#include "Branch.h"

void parseTree(Branch &branch, const rapidxml::xml_document<> &doc);

#endif /* PARSE_H_ */
