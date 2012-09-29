#include <cstring>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include <rapidxml.hpp>

#include "Branch.h"
#include "BranchLevel.h"
#include "construct.h"
#include "extract.h"
#include "parse.h"
#include "Random.h"

using namespace std;

int main(int argc, char **argv) {
	Random::init();

	bool storeRootBranchStructure;
	int derivationLength, branchLimit;
	size_t propagation, inferenceLevels;
	float monopodialScalingFactor, sympodialScalingFactor, minRadius, minLength;

	// Program options
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("derivation,d", boost::program_options::value<int>(&derivationLength)->default_value(0), "set derivation length beyond the levels in the input file")
		("scalemono", boost::program_options::value<float>(&monopodialScalingFactor)->default_value(1.0f / sqrtf(2.0f)), "set the scaling factor for inferred monopodial child branches")
		("scalesymp", boost::program_options::value<float>(&sympodialScalingFactor)->default_value(1.0f / sqrtf(2.0f)), "set the scaling factor for inferred sympodial child branches")
		("mradius,r", boost::program_options::value<float>(&minRadius)->default_value(0.1f), "set the minimum starting radius of a branch that will be produced")
		("mlength,l", boost::program_options::value<float>(&minLength)->default_value(0.1f), "set the minimum length of a branch that will be produced")
		("propagation,p", boost::program_options::value<size_t>(&propagation)->default_value(1), "set the number of levels to propagate inferred parameters to; if <= 0, will infer a propagation level")
		("branchlimit,b", boost::program_options::value<int>(&branchLimit)->default_value(0), "set the overall limit on the number of branches produced")
		("inferencelevels,i", boost::program_options::value<size_t>(&inferenceLevels)->default_value(2), "set the number of levels to use for inference information")
		("storeroot", boost::program_options::value<bool>(&storeRootBranchStructure)->default_value(true), "recreate the trunk of the tree almost exactly as it appears");
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);

	if (vm.count("help")) {
		cout << "Usage " << argv[0] << " [options]" << endl;
		cout << desc << endl;
		return 1;
	}

	if (!cin) return 1;

	// Read the XML from stdin
	string *file = new string();
	string line;
	while (getline(cin, line)) {
		*file += line + '\n';
	}

	char *xmlSource = new char[file->size() + 1];
	memcpy(xmlSource, file->c_str(), file->size() + 1);

	delete file;

	rapidxml::xml_document<> *doc = new rapidxml::xml_document<>();
	doc->parse<0>(xmlSource);

	// Parse incoming data
	Branch root;
	parseTree(root, *doc);

	delete doc;
	delete xmlSource;

	// Derive L-system from data
	vector<BranchLevel> branchLevels;
	extractData(root, branchLevels, inferenceLevels, propagation, storeRootBranchStructure);
	if (branchLevels.size() == 0) {
		cerr << "Error: no branch data present, cannot generate lpfg file" << endl;
	}

	// Construct lpfg file
	string lpfgFile;
	constructLpfgFile(branchLevels, lpfgFile, derivationLength,
			monopodialScalingFactor, sympodialScalingFactor,
			minRadius, minLength,
			branchLimit);

	cout << lpfgFile << endl;

	return 0;
}
