#include <iostream>

#include "construct.h"
#include "parse.h"

using namespace std;

#define PARSE_ERROR 1
#define CONVERSION_ERROR 2

int main(int argc, char **argv) {
	string lpfgFile;
	ParseData parseData;

	if (!cin) return 1;

	string line;
	while (getline(cin, line)) {
		lpfgFile += line + '\n';
	}

	// Comments can be replaced with whitespace here

	if (!parseString(lpfgFile, parseData)) {
		return PARSE_ERROR;
	}

	string outputFile;
	if (!constructCpp(parseData, outputFile)) {
		return CONVERSION_ERROR;
	}

	cout << outputFile << endl;

	return 0;
}
