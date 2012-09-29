#include <set>
#include <sstream>
#include <vector>

#include "common.h"
#include "construct.h"

using namespace std;

#define START_FUNC string(CONSTRUCT_PREFIX "start")
#define START_EACH_FUNC string(CONSTRUCT_PREFIX "startEach")
#define END_FUNC string(CONSTRUCT_PREFIX "end")
#define END_EACH_FUNC string(CONSTRUCT_PREFIX "endEach")
#define PRODUCTION_CLASS string(CONSTRUCT_PREFIX "Production")
#define PROD_FUNCTION string(CONSTRUCT_PREFIX "prod")
#define DECOMP_FUNCTION string(CONSTRUCT_PREFIX "decomp")
#define MODULE_STRING string(CONSTRUCT_PREFIX "moduleString")
#define DERIVATION_STEP_COUNTER string(CONSTRUCT_PREFIX "derivationStep")
#define INDENT_VAR string(CONSTRUCT_PREFIX "indentLevel")
#define APPLY_PRODUCTIONS_FUNC string(CONSTRUCT_PREFIX "applyProductions")
#define APPLY_DECOMPOSITIONS_FUNC string(CONSTRUCT_PREFIX "applyDecompositions")
#define MATCH_PRODUCTION_FUNC string(CONSTRUCT_PREFIX "matchProduction")
#define MATCH_RESULTS_CLASS string(CONSTRUCT_PREFIX "MatchResults")
#define CALL_PRODUCTION_FUNC string(CONSTRUCT_PREFIX "callProduction")
#define CALL_DECOMPOSITION_FUNC string(CONSTRUCT_PREFIX "callDecomposition")
#define TO_STRING_FUNC string(CONSTRUCT_PREFIX "toString")

void appendApplyDecompositionsFunc(const vector<Production> &decompositions, string &output);
void appendApplyProductionsFunc(const vector<Production> &productions, string &output);
void appendFunction(const string &name, const string &body, string &output);
void appendIncludes(string &output);
void appendMainFunction(const ParseData &parseData, string &output);
void appendMatchResultsClass(string &output);
void appendModuleInstanceArguments(const vector<ModuleInstance> &moduleInstances, bool continuingList, string &output);
void appendModules(const set<Module> &modules, string &output);
void appendProductionClass(string &output);
void appendProductionFunctionChooser(const vector<Production> &productions, const string &chooserName, string &output);
void appendProductionMatcherFunc(string &output);
void appendProductions(vector<Production> &productions, const string &functionIdentifier, string &output);
void appendToStringFunction(const set<Module> &modules, string &output);

template<class T>
std::string toString(const T &t) {
	stringstream ss;
	ss << t;
	return ss.str();
}

bool constructCpp(ParseData &parseData, string &outputFile) {
	outputFile = "";

	bool success = true;

	outputFile += parseData.transformedString;

	appendIncludes(outputFile);

	appendFunction(START_FUNC, parseData.startBody, outputFile);
	appendFunction(START_EACH_FUNC, parseData.startEachBody, outputFile);
	appendFunction(END_FUNC, parseData.endBody, outputFile);
	appendFunction(END_EACH_FUNC, parseData.endEachBody, outputFile);

	appendToStringFunction(parseData.modules, outputFile);

	appendMatchResultsClass(outputFile);
	appendModules(parseData.modules, outputFile);
	appendProductionClass(outputFile);
	appendProductions(parseData.productions, PROD_FUNCTION, outputFile);
	appendProductions(parseData.decompositions, DECOMP_FUNCTION, outputFile);
	appendProductionFunctionChooser(parseData.productions, CALL_PRODUCTION_FUNC, outputFile);
	appendProductionFunctionChooser(parseData.decompositions, CALL_DECOMPOSITION_FUNC, outputFile);

	appendProductionMatcherFunc(outputFile);
	appendApplyProductionsFunc(parseData.productions, outputFile);
	appendApplyDecompositionsFunc(parseData.decompositions, outputFile);

	appendMainFunction(parseData, outputFile);

	return success;
}

void appendFunction(const string &name, const string &body, string &output) {
	output += "\nvoid " + name + "() {\n" +
			body +
			"\n}\n";
}

void appendModules(const set<Module> &modules, string &output) {
	// Define the base module class
	output += "\nclass " + MODULE_CLASS + "{\n"
		"public:\n"
		"\t" + MODULE_CLASS + "() { position = 0; }\n"
		"\n\tvirtual std::string toString() const { return name + \"()\"; }\n"
		"\tfriend std::ostream &operator<<(std::ostream &out, const " + MODULE_CLASS + " &m) { return out << m.toString(); }\n"
		"\tstd::string name;\n"
		"\tint matchCount;\n"
		"\tsize_t position;\n"
		"};\n";

	int counter;
	string varName;

	// Extend the module class for each specific module
	for (set<Module>::const_iterator i = modules.begin(); i != modules.end(); ++i) {
		output += "\nclass " + i->name + " : public " + MODULE_CLASS + " {\n"
			"public:\n";

		// Create variables
		counter = 0;
		for (vector<string>::const_iterator j = i->parameterTypes.begin(); j != i->parameterTypes.end(); ++j) {
			varName = "_" + toString(counter++);
			output += "\t" + *j + " " + varName + ";\n";
		}

		// Create a constructor
		output += "\n\t" + i->name + "(";
		counter = 0;
		for (vector<string>::const_iterator j = i->parameterTypes.begin(); j != i->parameterTypes.end(); ++j) {
			if (counter != 0) {
				output += ", ";
			}
			varName = "_t_" + toString(counter++);
			output += *j + " " + varName;
		}
		output += ")";
		if (i->parameterTypes.size() > 0) {
			output += " : ";
			counter = 0;
			for (vector<string>::const_iterator j = i->parameterTypes.begin(); j != i->parameterTypes.end(); ++j) {
				if (counter != 0) {
					output += ", ";
				}
				varName = "_" + toString(counter);
				string varName2 = "_t_" + toString(counter);
				output += varName + "(" + varName2 + ")";
				++counter;
			}
		}
		output += " { name = \"" + i->name + "\"; }\n";

		// Create a toString method
		output += "\n\tstd::string toString() const {\n";
		output += "\t\tstd::string moduleString = name + \"(\";\n";
		for (size_t j = 0; j < i->parameterTypes.size(); ++j) {
			if (j > 0) {
				output += "\t\tmoduleString += \", \";\n";
			}
			output += "\t\tmoduleString += " + TO_STRING_FUNC + "(_" + toString(j) + ");\n";
		}
		output += "\t\tmoduleString += \")\";\n"
			"\t\treturn moduleString;\n"
			"\t}\n";

		output += "};\n";
	}
}

void appendProductions(vector<Production> &productions, const string &functionIdentifier, string &output) {
	// Assign names to each production and create the function code
	size_t counter = 0;
	vector<Production> newProductions;

	for (vector<Production>::iterator i = productions.begin(); i != productions.end(); ++i) {
		Production production = *i;
		production.name = functionIdentifier + toString(counter++);

		output += "\nstd::vector<" + MODULE_CLASS + " *> *" + production.name + "(";

		// Output the types and names of the parameters
		appendModuleInstanceArguments(production.leftContext, false, output);
		appendModuleInstanceArguments(production.predecessor, (production.leftContext.size() > 0), output);
		appendModuleInstanceArguments(production.rightContext,(production.leftContext.size() > 0 || production.predecessor.size() > 0), output);

		output += ") {\n" + production.body + "\n}\n";

		newProductions.push_back(production);
	}

	productions.clear();
	productions.insert(productions.begin(), newProductions.begin(), newProductions.end());
}

void appendModuleInstanceArguments(const vector<ModuleInstance> &moduleInstances, bool continuingList, string &output) {
	for (vector<ModuleInstance>::const_iterator i = moduleInstances.begin(); i != moduleInstances.end(); ++i) {
		for (size_t j = 0; j < i->arguments.size(); ++j) {
			if (continuingList) {
				output += ", ";
			} else {
				continuingList = true;
			}
			output += i->module.parameterTypes[j] + " " + i->arguments[j];
		}
	}
}

void appendMainFunction(const ParseData &parseData, string &output) {
	output += "\nint main() {\n";

	// Create and populate production/decomposition vectors
	output += "\tstd::vector<" + PRODUCTION_CLASS + "> productions;\n";
	for (vector<Production>::const_iterator i = parseData.productions.begin(); i != parseData.productions.end(); ++i) {
		output += "\n\t" + PRODUCTION_CLASS + " " + i->name + ";\n";
		output += "\t" + i->name + ".name = \"" + i->name + "\";\n";
		for (vector<ModuleInstance>::const_iterator j = i->leftContext.begin(); j != i->leftContext.end(); ++j) {
			output += "\t" + i->name + ".leftContext.push_back(\"" + j->moduleName +"\");\n";
		}
		for (vector<ModuleInstance>::const_iterator j = i->predecessor.begin(); j != i->predecessor.end(); ++j) {
			output += "\t" + i->name + ".predecessor.push_back(\"" + j->moduleName +"\");\n";
		}
		for (vector<ModuleInstance>::const_iterator j = i->rightContext.begin(); j != i->rightContext.end(); ++j) {
			output += "\t" + i->name + ".rightContext.push_back(\"" + j->moduleName +"\");\n";
		}
		output += "\tproductions.push_back(" + i->name + ");\n";
	}

	output += "\n\tstd::vector<" + PRODUCTION_CLASS + "> decompositions;\n";
	for (vector<Production>::const_iterator i = parseData.decompositions.begin(); i != parseData.decompositions.end(); ++i) {
		output += "\n\t" + PRODUCTION_CLASS + " " + i->name + ";\n";
		output += "\t" + i->name + ".name = \"" + i->name + "\";\n";
		for (vector<ModuleInstance>::const_iterator j = i->predecessor.begin(); j != i->predecessor.end(); ++j) {
			output += "\t" + i->name + ".predecessor.push_back(\"" + j->moduleName +"\");\n";
		}
		output += "\tdecompositions.push_back(" + i->name + ");\n";
	}

	// The module string which is modified/matched as derivation progresses
	output += "\n\tstd::vector<" + MODULE_CLASS + " *> modules;\n";

	// Call start function
	output += "\n\t" + START_FUNC + "();\n\n";

	// Instantiate the module string with the axiom
	for (vector<ModuleInstance>::const_iterator i = parseData.axiom.begin(); i != parseData.axiom.end(); ++i) {
		output += "\tmodules.push_back(new " + i->moduleName + "(";
		int counter = 0;
		for (vector<string>::const_iterator j = i->arguments.begin(); j != i->arguments.end(); ++j) {
			if (counter++ != 0) {
				output += ", ";
			}
			output += *j;
		}
		output += "));\n";
	}

	// Perform however many derivation steps are required
	output += "\tfor (int " + DERIVATION_STEP_COUNTER + " = 0; " + DERIVATION_STEP_COUNTER + " < " + parseData.derivationLength + "; ++" + DERIVATION_STEP_COUNTER + ") {\n";

	// Call starteach function
	output += "\t\t" + START_EACH_FUNC + "();\n";

	// Apply production rules
	output += "\t\t" + APPLY_PRODUCTIONS_FUNC + "(modules, productions, false);\n";

	// Perform decomposition
	output += "\t\t" + APPLY_DECOMPOSITIONS_FUNC + "(modules, decompositions, " + parseData.maximumDepthDecomposition + ");\n";

	// Call endeach function
	output += "\t\t" + END_EACH_FUNC + "();\n";

	// Call end function
	output += "\t}\n";
	output += "\n\t" + END_FUNC + "();\n";

	// Output the module string to standard output
	output += "\n\tstd::cout << \"LST " + LST_VERSION + "\" << std::endl << std::endl;\n"
		"\tint " + INDENT_VAR + " = 0;\n"
		"\tfor (std::vector<" + MODULE_CLASS + " *>::const_iterator i = modules.begin(); i != modules.end(); ++i) {\n"
		"\t\tif ((*i)->name == \"" + END_BRANCH_MODULE + "\") {\n"
				"\t\t\t--" + INDENT_VAR + ";\n"
		"\t\t}\n"
		"\t\tfor (int j = 0; j < " + INDENT_VAR + "; ++j) {\n"
		"\t\t\tstd::cout << \"\\t\";\n"
		"\t\t}\n"
		"\t\tstd::cout << (**i) << std::endl;\n"
		"\t\tif ((*i)->name == \"" + START_BRANCH_MODULE + "\") {\n"
		"\t\t\t++" + INDENT_VAR + ";\n"
		"\t\t}\n"
		"\t}\n";

	output += "\n\twhile (modules.size() > 0) {\n"
		"\t\tdelete modules[0];\n"
		"\t\tmodules.erase(modules.begin());\n"
		"\t}\n";

	output += "\n\treturn 0;\n"
		"}\n";
}

void appendApplyProductionsFunc(const vector<Production> &productions, string &output) {
	// Function header
	output += "\nvoid " + APPLY_PRODUCTIONS_FUNC + "(std::vector<" + MODULE_CLASS + " *> &modules, const std::vector<" + PRODUCTION_CLASS + "> &productions, bool isDecomp) {\n";

	// Reset information on modules for processing
	output += "\tfor (size_t i = 0; i < modules.size(); ++i) {\n"
		"\t\tmodules[i]->matchCount = 0;\n"
		"\t\tmodules[i]->position = i;\n"
		"\t}\n";

	// Match against oldString, write to newString
	output += "\tstd::vector<" + MODULE_CLASS + " *> oldString = modules;\n";
	output += "\tstd::vector<" + MODULE_CLASS + " *> newString = modules;\n";

	// Perform the matching production-by-production
	output += "\tfor (std::vector<" + PRODUCTION_CLASS + ">::const_iterator i = productions.begin(); i != productions.end(); ++i) {\n";

	// Perform the matching starting at each module in the module string (naive)
	output += "\t\tfor (size_t j = 0; j < oldString.size(); ++j) {\n";

	// Get the match results starting from position j
	output += "\t\t\t" + MATCH_RESULTS_CLASS + " match = " + MATCH_PRODUCTION_FUNC + "(oldString, *i, j);\n";

	// If no match, continue
	output += "\t\t\tif (!match.isMatch) continue;\n";

	// Otherwise, call the production function, and continue based on the results
	output += "\t\t\tstd::vector<" + MODULE_CLASS + " *> *results;\n"
		"\t\t\tif (isDecomp) {\n"
		"\t\t\t\tresults = " + CALL_DECOMPOSITION_FUNC + "(oldString, match, i->name);\n"
		"\t\t\t} else {\n"
		"\t\t\t\tresults = " + CALL_PRODUCTION_FUNC + "(oldString, match, i->name);\n"
		"\t\t\t}\n";

	// If the production function didn't match, carry on looking
	output += "\t\t\tif (!results) continue;\n";

	// If the production did do something, mark the predecessor as matched, and perform the replacement
	output += "\t\t\tfor (size_t k = match.predecessor.first; k < match.predecessor.second; ++k) {\n"
		"\t\t\t\t++oldString[k]->matchCount;\n"
		"\t\t\t}\n";
	output += "\t\t\tsize_t toRemove = match.predecessor.first;\n"
		"\t\t\tfor (size_t k = 0; k < newString.size(); ++k) {\n"
		"\t\t\t\tif (newString[k]->position == toRemove) {\n"
		"\t\t\t\t\tnewString.erase(newString.begin() + k);\n"
		"\t\t\t\t\tif (++toRemove == match.predecessor.second) {\n"
		"\t\t\t\t\t\tnewString.insert(newString.begin() + k, results->begin(), results->end());\n"
		"\t\t\t\t\t\tbreak;\n"
		"\t\t\t\t\t}\n"
		"\t\t\t\t\t--k;\n"
		"\t\t\t\t}\n"
		"\t\t\t}\n";

	output += "\t\t\tdelete results;\n";

	output += "\t\t}\n";
	output += "\t}\n";

	// Finally, replace modules with newString
	output += "\tfor (std::vector<" + MODULE_CLASS + " *>::iterator i = modules.begin(); i != modules.end(); ++i) {\n"
		"\t\tif ((*i)->matchCount > 0) {\n"
		"\t\t\tdelete *i;\n"
		"\t\t}\n"
		"\t}\n";

	output += "\tmodules.clear();\n";
	output += "\tmodules.insert(modules.begin(), newString.begin(), newString.end());\n";

	output += "}\n";
}

void appendApplyDecompositionsFunc(const vector<Production> &decompositions, string &output) {
	// Highly naive implementation
	// Just applies the decompositions as productions, as many times as required
	output += "\nvoid " + APPLY_DECOMPOSITIONS_FUNC + "(std::vector<" + MODULE_CLASS + " *> &modules, const std::vector<" + PRODUCTION_CLASS + "> &decompositions, int depth) {\n"
		"\tfor (int i = 0; i < depth; ++i) {\n"
		"\t\t" + APPLY_PRODUCTIONS_FUNC + "(modules, decompositions, true);\n"
		"\t}\n"
		"}\n";
}

void appendIncludes(string &output) {
	output += "\n#include <iostream>\n"
		"#include <sstream>\n"
		"#include <string>\n"
		"#include <vector>\n";
}

void appendProductionClass(string &output) {
	output += "\nclass " + PRODUCTION_CLASS + " {\n"
		"public:\n"
		"\tstd::string name;\n"
		"\tstd::vector<std::string> leftContext;\n"
		"\tstd::vector<std::string> predecessor;\n"
		"\tstd::vector<std::string> rightContext;\n"
		"};\n";
}

void appendProductionFunctionChooser(const vector<Production> &productions, const string &chooserName, string &output) {
	output += "\nstd::vector<" + MODULE_CLASS + " *> *" + chooserName + "(const std::vector<" + MODULE_CLASS + " *> &modules, " + MATCH_RESULTS_CLASS + " matchResults, std::string prodName) {\n";
	for (vector<Production>::const_iterator i = productions.begin(); i != productions.end(); ++i) {
		output += "\tif (prodName == \"" + i->name + "\") {\n";
		output += "\t\treturn " + i->name + "(";
		bool first = true;

		// Left context
		size_t moduleOffset = 0;
		for (vector<ModuleInstance>::const_iterator j = i->leftContext.begin(); j != i->leftContext.end(); ++j) {
			for (size_t k = 0; k < j->arguments.size(); ++k) {
				if (first) {
					first = false;
				} else {
					output += ", ";
				}
				output += "(dynamic_cast<const " + j->moduleName + " *>(modules[matchResults.leftContext[" + toString(moduleOffset) + "]]))->_" + toString(k);
			}
			++moduleOffset;
		}

		// Predecessor
		moduleOffset = 0;
		for (vector<ModuleInstance>::const_iterator j = i->predecessor.begin(); j != i->predecessor.end(); ++j) {
			for (size_t k = 0; k < j->arguments.size(); ++k) {
				if (first) {
					first = false;
				} else {
					output += ", ";
				}
				output += "(dynamic_cast<const " + j->moduleName + " *>(modules[matchResults.predecessor.first + " + toString(moduleOffset) + "]))->_" + toString(k);
			}
			++moduleOffset;
		}

		// Right context
		moduleOffset = 0;
		for (vector<ModuleInstance>::const_iterator j = i->rightContext.begin(); j != i->rightContext.end(); ++j) {
			for (size_t k = 0; k < j->arguments.size(); ++k) {
				if (first) {
					first = false;
				} else {
					output += ", ";
				}
				output += "(dynamic_cast<const " + j->moduleName + " *>(modules[(matchResults.rightContext[" + toString(moduleOffset) + "]]))->_" + toString(k);
			}
			++moduleOffset;
		}

		output += ");\n";
		output += "\t}\n";
	}
	output += "\treturn 0;\n";
	output += "}\n";
}

void appendMatchResultsClass(string &output) {
	output += "\nclass " + MATCH_RESULTS_CLASS + " {\n"
		"public:\n"
		"\tbool isMatch;\n"
		"\tstd::vector<size_t> leftContext;\n"
		"\tstd::pair<size_t, size_t> predecessor;\n"
		"\tstd::vector<size_t> rightContext;\n"
		"};\n";
}

void appendProductionMatcherFunc(string &output) {
	// Left context advanced matching rules currently unimplemented
	output += "\n" + MATCH_RESULTS_CLASS + " " + MATCH_PRODUCTION_FUNC + "(const std::vector<" + MODULE_CLASS + " *> &modules, const " + PRODUCTION_CLASS + " &production, size_t startPosition) {\n"
		"\t" + MATCH_RESULTS_CLASS + " results;\n"
		"\tresults.isMatch = false;\n"
		// Left context
		"\tfor (std::vector<std::string>::const_iterator i = production.leftContext.begin(); i != production.leftContext.end(); ++i) {\n"
			"\t\tif (startPosition >= modules.size()) return results;\n"
			"\t\tif (*i == modules[startPosition]->name) {\n"
				"\t\t\tresults.leftContext.push_back(startPosition++);\n"
			"\t\t} else {\n"
				"\t\t\treturn results;\n"
			"\t\t}\n"
		"\t}\n"
		// Strict predecessor
		"\tresults.predecessor.first = startPosition;\n"
		"\tfor (std::vector<std::string>::const_iterator i = production.predecessor.begin(); i != production.predecessor.end(); ++i) {\n"
			"\t\tif (startPosition >= modules.size()) return results;\n"
			"\t\tif (modules[startPosition]->matchCount == 0 && *i == modules[startPosition]->name) {\n"
				"\t\t\t++startPosition;\n"
			"\t\t} else {\n"
				"\t\t\treturn results;\n"
			"\t\t}\n"
		"\t}\n"
		"\tresults.predecessor.second = startPosition;\n"
		// Right context
		"\tfor (std::vector<std::string>::const_iterator i = production.rightContext.begin(); i != production.rightContext.end(); ++i) {\n"
			"\t\tif (startPosition >= modules.size()) return results;\n"
			"\t\tif (*i == modules[startPosition]->name) {\n"
				"\t\t\tresults.rightContext.push_back(startPosition++);\n"
			"\t\t} else {\n"
				"\t\t\tbool skipToEndBranch = false;\n"
				"\t\t\tint depth = 1;\n"
				"\t\t\tif (startPosition >= modules.size()) return results;\n"
				"\t\t\tif (modules[startPosition]->name == \"" + START_BRANCH_MODULE + "\") {\n"
					"\t\t\t\tskipToEndBranch = true;\n"
				"\t\t\t} else if (*i == \"" + END_BRANCH_MODULE + "\") {\n"
					"\t\t\t\tskipToEndBranch = true;\n"
				"\t\t\t} else {\n"
					"\t\t\t\treturn results;\n"
				"\t\t\t}\n"
				"\t\t\tif (skipToEndBranch) {\n"
					"\t\t\t\t++startPosition;\n"
					"\t\t\t\twhile (depth > 0) {\n"
							"\t\t\t\t\tif (startPosition >= modules.size()) return results;\n"
							"\t\t\t\t\tif (modules[startPosition]->name == \"" + START_BRANCH_MODULE + "\") {\n"
								"\t\t\t\t\t\t++depth;\n"
							"\t\t\t\t\t} else if (modules[startPosition]->name == \"" + END_BRANCH_MODULE + "\") {\n"
								"\t\t\t\t\t\t--depth;\n"
							"\t\t\t\t\t}\n"
							"\t\t\t\t\t++startPosition;\n"
					"\t\t\t\t}\n"
				"\t\t\t}\n"
			"\t\t}\n"
		"\t}\n"
		"\tresults.isMatch = true;\n"
		"\treturn results;\n"
	"}\n";
}

void appendToStringFunction(const set<Module> &modules, string &output) {
	output += "\ntemplate<class T>\n"
		"std::string " + TO_STRING_FUNC + "(const T &t) {\n"
		"\tstd::stringstream ss;\n"
		"\tss << t;\n"
		"\treturn ss.str();\n"
		"}\n";
}
