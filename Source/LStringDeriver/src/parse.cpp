#include <iostream>

#include <boost/regex.hpp>

#include "common.h"
#include "parse.h"

using namespace boost;
using namespace std;

namespace LabelTypes {
enum Enumerator {
	Production,
	Decomposition,
	Interpretation
};
}

// First element is the type of label, second is its position in the string
typedef vector<pair<LabelTypes::Enumerator, size_t> > LabelVec;

// Regex for the start of a statement: line start, followed by anything that
// isn't horizontal whitespace, followed by one of {, }, ), or ;, followed by
// whitespace
static const string statementStart = "";//"^(?:\\V*[\\{\\}\\);]+)*?\\s*";
// A valid C identifier
static const string identifier = "[_A-Za-z][_A-Za-z0-9]*";

// Forward declarations
template<class T>
bool isDuplicate(const vector<T> &vec, const T &obj);

bool extractBody(const string &text, size_t startBracePos, string &body, size_t &closingBracePos);
bool isWhitespace(const string &text);
bool labelComparator(pair<LabelTypes::Enumerator, size_t> a, pair<LabelTypes::Enumerator, size_t> b);
bool parseAxiom(ParseData &parseData);
bool parseDerivationLength(ParseData &parseData);
LabelVec parseLabels(ParseData &parseData);
void parseLabels(ParseData &parseData, LabelVec &labelPositions, LabelTypes::Enumerator type, const regex &exp);
bool parseMaximumDepths(ParseData &parseData, const LabelVec &labelPositions);
bool parseModules(ParseData &parseData);
bool parseModuleString(const string &expression, const set<Module> &modules, bool matchToModule, vector<ModuleInstance> &parsedList);
bool parseProductions(ParseData &parseData, const LabelVec &labelPositions);
bool parseProductionsProduce(string &body, const set<Module> &modules);
bool parseProductionsReturn(string &body, const string &cleanupString);
bool parseBlock(string &text, const string &blockLabel, string &body);

bool parseString(const string &lpfgString, ParseData &parseData) {
	parseData.transformedString = lpfgString;

	bool success = true;

	success &= parseDerivationLength(parseData);
	LabelVec labelPositions = parseLabels(parseData);
	success &= parseMaximumDepths(parseData, labelPositions);
	success &= parseModules(parseData);
	success &= parseAxiom(parseData);
	success &= parseProductions(parseData, labelPositions);
	success &= parseBlock(parseData.transformedString, "Start", parseData.startBody);
	success &= parseBlock(parseData.transformedString, "StartEach", parseData.startEachBody);
	success &= parseBlock(parseData.transformedString, "End", parseData.endBody);
	success &= parseBlock(parseData.transformedString, "EndEach", parseData.endEachBody);

	return success;
}

bool parseDerivationLength(ParseData &parseData) {
	// Anything of the form "derivation length: (expression);"
	// Group 1 is the entire statement
	// Group 2 is the derivation length expression
	static const regex exp(statementStart + "(derivation\\s+length\\s*:\\s*(.*?)\\s*;)");

	string::const_iterator start = parseData.transformedString.begin();
	string::const_iterator end = parseData.transformedString.end();
	match_results<string::const_iterator> what;
	match_flag_type flags = match_default;

	int count = 0;
	while (regex_search(start, end, what, exp, flags)) {
		++count;

		// Extract the derivation length expression
		parseData.derivationLength = parseData.transformedString.substr(
				what[2].first - parseData.transformedString.begin(),
				what[2].second - what[2].first);

		// Replace the matched statement with a blank string of the same length
		parseData.transformedString.replace(
				what[1].first - parseData.transformedString.begin(),
				what[1].second - what[1].first,
				what[1].second - what[1].first, ' ');

		start = what[1].second;

		flags |= match_prev_avail;
		flags |= match_not_bob;
	}

	if (count == 0) {
		cerr << "Error: no derivation length statement" << endl;
		return false;
	} else if (count == 1) {
		return true;
	} else {
		cerr << "Error: duplicate derivation length statements" << endl;
		return false;
	}
}

LabelVec parseLabels(ParseData &parseData) {
	// Expressions of one of the following forms:
	//    "production:"
	//    "decomposition:"
	//    "interpretation:"
	// Group 1 is the production/decomposition/interpretation statement
	static const regex productionExp(statementStart + "(production\\s*:)");
	static const regex decompositionExp(statementStart + "(decomposition\\s*:)");
	static const regex interpretationExp(statementStart + "(interpretation\\s*:)");

	LabelVec labelPositions;

	// Search for production labels
	parseLabels(parseData, labelPositions, LabelTypes::Production, productionExp);

	// Search for decomposition labels
	parseLabels(parseData, labelPositions, LabelTypes::Decomposition, decompositionExp);

	// Search for interpretation labels
	parseLabels(parseData, labelPositions, LabelTypes::Interpretation, interpretationExp);

	// Sort the labels by line number
	sort(labelPositions.begin(), labelPositions.end(), labelComparator);

	return labelPositions;
}

bool labelComparator(pair<LabelTypes::Enumerator, size_t> a, pair<LabelTypes::Enumerator, size_t> b) {
	return a.second < b.second;
}

void parseLabels(ParseData &parseData, LabelVec &labelPositions, LabelTypes::Enumerator type, const regex &exp) {
	string::const_iterator start = parseData.transformedString.begin();
	string::const_iterator end = parseData.transformedString.end();
	match_results<string::const_iterator> what;
	match_flag_type flags = match_default;

	while (regex_search(start, end, what, exp, flags)) {
		// Record the position of the label
		labelPositions.push_back(make_pair(type, what[1].first - parseData.transformedString.begin()));

		// Replace the matched statement with a blank string of the same length
		parseData.transformedString.replace(
				what[1].first - parseData.transformedString.begin(),
				what[1].second - what[1].first,
				what[1].second - what[1].first, ' ');

		start = what[1].second;

		flags |= match_prev_avail;
		flags |= match_not_bob;
	}
}


bool parseMaximumDepths(ParseData &parseData, const LabelVec &labelPositions) {
	// Anything of the form "maximum depth: (expression);"
	// Group 1 is the entire statement
	// Group 2 is the maximum depth expression
	static const regex exp(statementStart + "(maximum\\s+depth\\s*:\\s*(.*?)\\s*;)");

	int decompDepthCount = 0;
	int interpDepthCount = 0;

	string::const_iterator start = parseData.transformedString.begin();
	string::const_iterator end = parseData.transformedString.end();
	match_results<string::const_iterator> what;
	match_flag_type flags = match_default;

	LabelTypes::Enumerator label;
	size_t matchPosition;
	string expression;

	bool unmatchedStatement = false;

	while (regex_search(start, end, what, exp, flags)) {
		// Extract the expression
		expression = parseData.transformedString.substr(
				what[2].first - parseData.transformedString.begin(),
				what[2].second - what[2].first);

		// Replace the matched statement with a blank string of the same length
		parseData.transformedString.replace(
				what[1].first - parseData.transformedString.begin(),
				what[1].second - what[1].first,
				what[1].second - what[1].first, ' ');

		// Depending on the expression's position, it may be for decomposition or interpretation
		matchPosition = what[1].first - parseData.transformedString.begin();
		label = LabelTypes::Production;
		for (LabelVec::const_iterator i = labelPositions.begin(); i != labelPositions.end(); ++i) {
			if (i->second < matchPosition) {
				label = i->first;
			} else {
				break;
			}
		}

		switch (label) {
		case LabelTypes::Decomposition:
			++decompDepthCount;
			parseData.maximumDepthDecomposition = expression;
			break;
		case LabelTypes::Interpretation:
			++interpDepthCount;
			parseData.maximumDepthInterpretation = expression;
			break;
		default:
			unmatchedStatement = true;
			break;
		}

		start = what[1].second;

		flags |= match_prev_avail;
		flags |= match_not_bob;
	}

	// Set defaults if no expressions found
	if (decompDepthCount == 0) {
		parseData.maximumDepthDecomposition = "1";
	}

	if (interpDepthCount == 0) {
		parseData.maximumDepthInterpretation = "1";
	}

	// Check for errors
	bool error = false;
	if (unmatchedStatement) {
		cerr << "Error: maximum depth statement not matched to decomposition or interpretation label" << endl;
		error = true;
	}

	if (decompDepthCount > 1) {
		cerr << "Error: multiple maximum decomposition depth statements" << endl;
		error = true;
	}

	if (interpDepthCount > 1) {
		cerr << "Error: multiple maximum interpretation depth statements" << endl;
		error = true;
	}

	return !error;
}

bool parseModules(ParseData &parseData) {
	// Anything of the form: "module identifier(type1, type2, ..., typen);"
	// Group 1 is the statement
	// Group 2 is the module identifier and arguments
	static const regex exp(statementStart + "(module\\s+(" + identifier + "\\s*\\(\\s*(?:" + identifier + "\\s*(?:,\\s*" + identifier + "\\s*)*)?\\))\\s*;)");

	string::const_iterator start = parseData.transformedString.begin();
	string::const_iterator end = parseData.transformedString.end();
	match_results<string::const_iterator> what;
	match_flag_type flags = match_default;

	bool duplicateModuleExists = false;

	while (regex_search(start, end, what, exp, flags)) {
		// Add the module to the stored list
		vector<ModuleInstance> parsedList;
		if (parseModuleString(
				parseData.transformedString.substr(what[2].first - parseData.transformedString.begin(),
						what[2].second - what[2].first),
				set<Module>(), false, parsedList)) {

			Module module;
			module.name = parsedList[0].moduleName;
			module.parameterTypes = parsedList[0].arguments;

			duplicateModuleExists |= !parseData.modules.insert(module).second;
		}

		// Replace the matched statement with a blank string of the same length
		parseData.transformedString.replace(
				what[1].first - parseData.transformedString.begin(),
				what[1].second - what[1].first,
				what[1].second - what[1].first, ' ');

		start = what[1].second;

		flags |= match_prev_avail;
		flags |= match_not_bob;
	}

	if (duplicateModuleExists) {
		cerr << "Error: duplicate module definitions present" << endl;
		return false;
	}

	return true;
}

bool parseAxiom(ParseData &parseData) {
	// Anything of the form "axiom expression;"
	// Group 1 is the whole statement
	// Group 2 is the axiom expression
	static const regex exp(statementStart + "(axiom\\s*:\\s*(.*?)\\s*;)");

	string::const_iterator start = parseData.transformedString.begin();
	string::const_iterator end = parseData.transformedString.end();
	match_results<string::const_iterator> what;
	match_flag_type flags = match_default;

	bool success = true;
	int count = 0;
	string expression;

	while (regex_search(start, end, what, exp, flags)) {
		++count;

		// Process the expression
		expression = parseData.transformedString.substr(
				what[2].first - parseData.transformedString.begin(),
				what[2].second - what[2].first);
		parseData.axiom.clear();
		if (!parseModuleString(expression, parseData.modules, true, parseData.axiom)) {
			success = false;
			cerr << "Error: parsing axiom expression: " << expression << endl;
		}

		// Replace the matched statement with a blank string of the same length
		parseData.transformedString.replace(
				what[1].first - parseData.transformedString.begin(),
				what[1].second - what[1].first,
				what[1].second - what[1].first, ' ');

		start = what[1].second;

		flags |= match_prev_avail;
		flags |= match_not_bob;
	}

	if (count == 0) {
		cerr << "Error: no axiom statement found" << endl;
		return false;
	} else if (count == 1) {
		return success;
	} else {
		cerr << "Error: duplicate axiom statements present" << endl;
		return false;
	}
}

bool parseModuleString(const string &expression, const set<Module> &modules, bool matchToModule, vector<ModuleInstance> &parsedList) {
	static const regex identifierExp("\\s*(" + identifier + ")\\s*");
	static const regex whitespaceExp("\\s*");
	// Of the form module1(arg1, arg2, ..., argm) ... modulek(arg1, arg2, ..., argn)
	match_results<string::const_iterator> what;
	int depth = 0;
	size_t pos = 0;
	string moduleName;
	ModuleInstance moduleInstance;

	for (size_t i = 0; i < expression.size(); ++i) {
		if (expression[i] == '(') {
			++depth;
			if (depth == 1) {
				moduleInstance = ModuleInstance();

				moduleName = expression.substr(pos, i - pos);
				if (regex_match(moduleName, what, identifierExp)) {
					moduleInstance.moduleName.assign(what[1].first, what[1].second);
				} else {
					return false;
				}
				pos = i + 1;
			}
		} else if (expression[i] == ')') {
			--depth;
			if (depth == 0) {
				string argument = expression.substr(pos, i - pos);
				if (!isWhitespace(argument)) {
					moduleInstance.arguments.push_back(argument);
				}
				pos = i + 1;

				// Check that the module exists, with the given argument count
				if (matchToModule) {
					Module module;
					module.name = moduleInstance.moduleName;
					set<Module>::iterator j = modules.find(module);
					if (j == modules.end()) {
						cerr << "Error: module " << moduleInstance.moduleName << " in module string " << expression << " is not defined" << endl;
						return false;
					} else if (j->parameterTypes.size() != moduleInstance.arguments.size()) {
						cerr << "Error: module " << moduleInstance.moduleName << " with " << moduleInstance.arguments.size() <<
						" arguments in module string " << expression << " does not match defined module " << j->name <<
						" with " << j->parameterTypes.size() << " arguments" << endl;
						return false;
					} else {
						moduleInstance.module = *j;
						parsedList.push_back(moduleInstance);
					}
				} else {
					parsedList.push_back(moduleInstance);
				}
			} else if (depth < 0) {
				return false;
			}
		} else if (expression[i] == ',') {
			if (depth == 1) {
				moduleInstance.arguments.push_back(expression.substr(pos, i - pos));
				pos = i + 1;
			}
		}
	}

	if (!regex_match(expression.substr(pos, expression.size() - pos), what, whitespaceExp)) {
		return false;
	}

	if (depth != 0) {
		return false;
	} else {
		return true;
	}
}

bool parseProductions(ParseData &parseData, const LabelVec &labelPositions) {
	// Expressions of the following form:
	//   predecessor:
	//   {
	//     production body
	//   }
	// Or:
	//   left context < predecessor > right context:
	//   {...}
	// Where predecessor, left context, and right context are module strings
	// Group 1 is the entire statement, up to and including the opening brace
	// Group 2 is the left context, group 3 is the predecessor, and group 4 is
	// the right context

	static const string module = identifier + "\\s*\\(\\s*(?:" + identifier + "(?:\\s*,\\s*" + identifier + ")*)?\\s*\\)";
	static const regex exp(statementStart + "((?:((?:" + module + "\\s*))+<\\s*)?((?:" + module + "\\s*)+)(?:\\s*>((?:\\s*" + module + ")+))?\\s*:\\s*\\{)");

	string::const_iterator start = parseData.transformedString.begin();
	string::const_iterator end = parseData.transformedString.end();
	match_results<string::const_iterator> what;
	match_flag_type flags = match_default;

	bool duplicateProductionExists = false;
	LabelTypes::Enumerator label;
	size_t matchPosition;

	bool success = true;

	while (regex_search(start, end, what, exp, flags)) {
		flags |= match_prev_avail;
		flags |= match_not_bob;

		matchPosition = what[1].first - parseData.transformedString.begin();

		// Extract the left context, predecessor, right context, and body
		Production production;

		parseModuleString(
				parseData.transformedString.substr(
						what[2].first - parseData.transformedString.begin(),
						what[2].second - what[2].first),
				parseData.modules, true,
				production.leftContext);

		parseModuleString(
				parseData.transformedString.substr(
						what[3].first - parseData.transformedString.begin(),
						what[3].second - what[3].first),
				parseData.modules, true,
				production.predecessor);

		parseModuleString(
				parseData.transformedString.substr(
						what[4].first - parseData.transformedString.begin(),
						what[4].second - what[4].first),
				parseData.modules, true,
				production.rightContext);

		size_t pos;
		if (!extractBody(parseData.transformedString,
				what[1].second - parseData.transformedString.begin() - 1,
				production.body, pos)) {
			success = false;
			continue;
		}

		// The start position of our next search
		// The position after the closing brace
		start = parseData.transformedString.begin() + pos + 1;

		string cleanupString = "\n\tfor (std::vector<" + MODULE_CLASS + " *>::iterator " + PROD_ITER_VAR + " = " + PROD_STRING + "->begin(); " + PROD_ITER_VAR + " != " + PROD_STRING + "->end(); ++" + PROD_ITER_VAR + ") {\n"
				"\t\tdelete *" + PROD_ITER_VAR + ";\n"
				"\t}\n"
				"\tdelete " + PROD_STRING + ";\n\treturn 0;\n";

		success &= parseProductionsReturn(production.body, cleanupString);
		parseProductionsProduce(production.body, parseData.modules);

		// Add statements to the production body that allow the returning of
		// production information
		production.body = "\tstd::vector<" + MODULE_CLASS + " *> *" + PROD_STRING + " = new std::vector<" + MODULE_CLASS + " *>();\n" +
		                  production.body +
		                  cleanupString;

		label = LabelTypes::Production;
		for (LabelVec::const_iterator i = labelPositions.begin(); i != labelPositions.end(); ++i) {
			if (i->second < matchPosition) {
				label = i->first;
			} else {
				break;
			}
		}

		if (label == LabelTypes::Production) {
			duplicateProductionExists |= isDuplicate(parseData.productions, production);
			parseData.productions.push_back(production);
		} else if (label == LabelTypes::Decomposition) {
			if (production.leftContext.size() > 0 || production.leftContext.size() > 0) {
				success = false;
				cerr << "Error: decompositions must be context-free" << endl;
			}
			if (production.predecessor.size() > 1) {
				success = false;
				cerr << "Error: decompositions cannot contain more than one module in their strict predecessor" << endl;
			}
			duplicateProductionExists |= isDuplicate(parseData.decompositions, production);
			parseData.decompositions.push_back(production);
		}

		// Replace the matched statement with a blank string of the same length
		parseData.transformedString.replace(
				what[1].first - parseData.transformedString.begin(),
				start - what[1].first,
				start - what[1].first, ' ');
	}

	if (duplicateProductionExists) {
		success = false;
		cerr << "Error: duplicate production/decomposition present" << endl;
	}

	return success;
}

bool parseProductionsProduce(string &body, const set<Module> &modules) {
	// Expressions of the following form:
	//   produce expression;
	// Group 1 is the produce statement, group 2 is the word produce or nproduce, group 3 is the expression
	static const regex exp(statementStart + "((produce|nproduce)(?:\\s(.*?))??\\s*;)");

	string::const_iterator start = body.begin();
	string::const_iterator end = body.end();
	match_results<string::const_iterator> what;
	match_flag_type flags = match_default;

	bool success = true;
	string expression;
	string replacementText;

	while (regex_search(start, end, what, exp, flags)) {
		// Produce statements can have empty expressions
		vector<ModuleInstance> parsedList;
		expression = body.substr(
				what[3].first - body.begin(),
				what[3].second - what[3].first);

		if (!parseModuleString(expression, modules, true, parsedList)) {
			cerr << "Error: parsing produce expression: " << expression << endl;
			success = false;
		}

		// Generate the text to replace the produce statement with
		// The following string:
		//   produce SB() A(a, b) EB();
		// will produce:
		//   PROD_STRING->push_back(SB());
		//   PROD_STRING->push_back(A(a, b));
		//   PROD_STRING->push_back(EB());
		//   return PROD_STRING;
		replacementText = "\n";
		for (vector<ModuleInstance>::const_iterator i = parsedList.begin(); i != parsedList.end(); ++i) {
			replacementText += "\t" + PROD_STRING + "->push_back(new " + i->moduleName + "(";
			for (size_t j = 0; j < i->arguments.size(); ++j) {
				if (j > 0) {
					replacementText += ", ";
				}
				replacementText += i->arguments[j];
			}
			replacementText += "));\n";
		}

		if (body.substr(
				what[2].first - body.begin(),
				what[2].second - what[2].first) == "produce") {
			replacementText += "\treturn " + PROD_STRING + ";\n";
		}

		// Finally, actually replace the produce statement, paying attention to bounds
		size_t sizeToReplace = what[1].second - what[1].first;
		if (replacementText.size() > sizeToReplace) {
			size_t newStart = what[1].first - body.begin();
			newStart += replacementText.size();

			body.erase(what[1].first - body.begin(),
					what[1].second - what[1].first);
			body.insert(what[1].first - body.begin(),
					replacementText);

			start = body.begin() + newStart;
			end = body.end();
		} else {
			body.replace(
					what[1].first - body.begin(),
					what[1].second - what[1].first,
					what[1].second - what[1].first, ' ');
			body.replace(what[1].first - body.begin(),
					replacementText.size(),
					replacementText);
			start = what[1].second;
		}

		flags |= match_prev_avail;
		flags |= match_not_bob;
	}

	return success;
}

bool parseProductionsReturn(string &body, const string &cleanupString) {
	// Expressions of the following form:
	//   return expression;
	// Group 1 is the return statement, group 2 is the expression
	static const regex exp(statementStart + "(return(?:\\s(.*?))??\\s*;)");

	string::const_iterator start = body.begin();
	string::const_iterator end = body.end();
	match_results<string::const_iterator> what;
	match_flag_type flags = match_default;

	bool success = true;

	while (regex_search(start, end, what, exp, flags)) {
		// Return statements shouldn't have expressions
		if (what[2].second - what[2].first > 0) {
			success = false;
			cerr << "Error: return statements with expressions not allowed in productions" << endl;
			start = what[1].second;
		} else {
			// Replace the return statement with the appropriate string, taking
			// into account endpoints -- check if the string is getting bigger,
			// smaller, or staying the same
			size_t sizeToReplace = what[1].second - what[1].first;
			if (cleanupString.size() > sizeToReplace) {
				size_t newStart = what[1].first - body.begin();
				newStart += cleanupString.size();

				body.erase(what[1].first - body.begin(),
						what[1].second - what[1].first);
				body.insert(what[1].first - body.begin(),
						cleanupString);

				start = body.begin() + newStart;
				end = body.end();
			} else if (cleanupString.size() <= sizeToReplace) {
				body.replace(
						what[1].first - body.begin(),
						what[1].second - what[1].first,
						what[1].second - what[1].first, ' ');
				body.replace(what[1].first - body.begin(),
						cleanupString.size(),
						cleanupString);
				start = what[1].second;
			}
		}

		flags |= match_prev_avail;
		flags |= match_not_bob;
	}

	return success;
}

bool parseBlock(string &text, const string &blockLabel, string &body) {
	// Of the form:
	//   blockLabel: { /*body*/ }
	const regex exp(statementStart + "(" + blockLabel + "\\s*:\\s*\\{)");

	body = "";

	string::const_iterator start = text.begin();
	string::const_iterator end = text.end();
	match_results<string::const_iterator> what;
	match_flag_type flags = match_default;

	int count = 0;
	bool success = true;

	while (regex_search(start, end, what, exp, flags)) {
		++count;

		size_t pos;
		if (!extractBody(text, what[1].second - text.begin() - 1, body, pos)) {
			success = false;
			continue;
		}

		// Replace the matched statement with a blank string of the same length
		text.replace(
				what[1].first - text.begin(),
				pos - (what[1].first - text.begin()) + 1,
				pos - (what[1].first - text.begin()) + 1, ' ');

		flags |= match_prev_avail;
		flags |= match_not_bob;
	}

	if (count > 1) {
		cerr << "Error: duplicate " << blockLabel << " blocks" << endl;
		success = false;
	}

	return success;
}

bool extractBody(const string &text, size_t startBracePos, string &body, size_t &closingBracePos) {
	int depth = 0;
	size_t pos = startBracePos;
	for (; pos < text.size(); ++pos) {
		if (text[pos] == '{') {
			++depth;
		} else if (text[pos] == '}') {
			--depth;
			if (depth == 0) {
				break;
			}
		}
	}

	if (depth != 0) {
		return false;
	}

	body = text.substr(startBracePos + 1, pos - startBracePos - 1);
	closingBracePos = pos;
	return true;
}

template<class T>
bool isDuplicate(const vector<T> &vec, const T &obj) {
	typename vector<T>::const_iterator it;
	for (it = vec.begin(); it != vec.end(); ++it) {
		if (obj == *it) return true;
	}
	return false;
}

bool isWhitespace(const string &text) {
	static const regex whitespaceExp("\\s*");

	match_results<string::const_iterator> what;

	return regex_match(text, what, whitespaceExp);
}
