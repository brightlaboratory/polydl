#include <ConfigProcessor.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;


void ReadCacheConfig(ifstream& inFile, Config* config);
void CheckIfConfigIsFullySpecified(Config* config);
void InitializeConfig(Config* config);
void ReadDataTypeConfig(ifstream& inFile, Config* config);
void ReadParams(ifstream& inFile, Config* config);
void PrintConfig(Config* config);
void ReadConfigFromFile(string configFile, Config* config);
void ReadConfigFromUserInput(UserInput *userInput, Config* config);
void ReadCacheConfig(string cachesizes, Config* config);
void ReadDataTypeConfig(string line, Config* config);
void ReadParameterValues(string line, vector<string> *paramNames, Config* config);
void ReadParams(string line, Config* config);
void ReadParallelLoops(string parallelLoops, Config* config);
void ReadSharedCacheConfig(string sharedcaches, Config* config);

void ReadConfig(UserInput *userInput, Config* config) {

	string configFile = userInput->configFile;

	/* Initialization */
	InitializeConfig(config);
	config->datatypeSize = -1;

	if (!userInput->configFile.empty()) {
		ReadConfigFromFile(userInput->configFile, config);
	}
	else {
		ReadConfigFromUserInput(userInput, config);
	}
}

void ReadConfigFromUserInput(UserInput *userInput, Config* config) {
	if (userInput->parameters.empty()) {
		cout << "Parameters not provided." << endl;
		exit(1);
	}

	if (userInput->cachesizes.empty()) {
		cout << "Cache sizes not provided." << endl;
		exit(1);
	}

	if (userInput->datatypesize.empty()) {
		cout << "Data type size not provided." << endl;
		exit(1);
	}

	ReadCacheConfig(userInput->cachesizes, config);
	ReadDataTypeConfig(userInput->datatypesize, config);
	ReadParams(userInput->parameters, config);
	ReadParallelLoops(userInput->parallelLoops, config);
	ReadSharedCacheConfig(userInput->sharedcaches, config);
}

void ReadConfigFromFile(string configFile, Config* config) {
	const string CACHE_HEADER = "cache";
	const string DATATYPE_SIZE_HEADER = "datatype_size";
	const string PARAMS_HEADER = "params";

	ifstream inFile;
	inFile.open(configFile);

	if (!inFile) {
		cout << "Unable to open config file: " << configFile << endl;
		exit(1);
	}

	string line;
	while (getline(inFile, line))
	{
		if (line == CACHE_HEADER) {
			ReadCacheConfig(inFile, config);
		}
		else if (line == DATATYPE_SIZE_HEADER) {
			ReadDataTypeConfig(inFile, config);
		}
		else if (line == PARAMS_HEADER) {
			ReadParams(inFile, config);
		}
	}

	CheckIfConfigIsFullySpecified(config);
	inFile.close();
}

void ReadCacheConfig(string cachesizes, Config* config) {
	istringstream iss(cachesizes);
	string cache;
	string size;

	while ((iss >> cache >> size)) {
		try {
			if (cache == "L1") {
				config->systemConfig->L1 = stol(size, nullptr, 10);
			}
			else if (cache == "L2") {
				config->systemConfig->L2 = stol(size, nullptr, 10);
			}
			else if (cache == "L3") {
				config->systemConfig->L3 = stol(size, nullptr, 10);
			}
			else {
				cout << "Cache in config file not known: " << cache << endl;
				exit(1);
			}
		}
		catch (const invalid_argument) {
			cerr << "Invalid cache size while reading the config file" << endl;
			exit(1);
		}
	}
}

void ReadSharedCacheConfig(string sharedcaches, Config* config) {
	istringstream iss(sharedcaches);
	string cache;

	while ((iss >> cache)) {
		try {
			if (cache == "L1") {
				config->systemConfig->L1Shared = true;
			}
			else if (cache == "L2") {
				config->systemConfig->L2Shared = true;
			}
			else if (cache == "L3") {
				config->systemConfig->L3Shared = true;
			}
			else {
				cout << "Cache in config file not known: " << cache << endl;
				exit(1);
			}
		}
		catch (const invalid_argument) {
			cerr << "Invalid cache size while reading the config file" << endl;
			exit(1);
		}
	}
}

void ReadParallelLoops(string parallelLoops, Config* config) {
	config->parallelLoops = NULL;
	if (!parallelLoops.empty()) {
		config->parallelLoops = new vector<string>();
		istringstream iss(parallelLoops);
		string loopName;
		while (iss >> loopName) {
			config->parallelLoops->push_back(loopName);
		}

		if (config->parallelLoops->size() > 1) {
			cout << "Multiple parallel loops have been declared."
				<< "Make the outermost loop, the parallel loop for equivalent analysis." << endl;
			exit(1);
		}
	}
}

void ReadCacheConfig(ifstream& inFile, Config* config) {
	string line;
	while (getline(inFile, line)) {
		if (line == "\n" || line.empty()) {
			break;
		}

		ReadCacheConfig(line, config);
	}
}

void ReadParameterNames(string line, vector<string> *paramNames) {
	if (line == "\n" || line.empty()) {
	}
	else {
		istringstream iss(line);

		string paramName;
		while (iss >> paramName) {
			paramNames->push_back(paramName);
		}
	}
}

void ReadParameterValues(string line, vector<string> *paramNames, Config* config) {
	unordered_map<std::string, int>* params = new unordered_map<std::string, int>();
	istringstream iss(line);
	string valueStr;
	int i = 0;

	while (iss >> valueStr) {
		try {
			int value = stoi(valueStr, nullptr, 10);
			params->insert({ paramNames->at(i), value });
		}
		catch (const invalid_argument) {
			cerr << "Invalid datatype size while reading the config file" << endl;
			exit(1);
		}

		i++;
	}

	if (i != paramNames->size()) {
		cout << "Parsing: " << line << endl;
		cout << "Expected param values: " << paramNames->size() << " actual: "
			<< i << endl;
		exit(1);
	}

	config->programParameterVector->push_back(params);
}

void ReadParams(string line, Config* config) {
	/*The input is of the form:
	M N K : 1000 2000 3000
	*/

	vector<string> *paramNames = new vector<string>();
	try {
		int pos = line.find(":");
		string paramNamesString = line.substr(0, pos);
		string paramValuesString = line.substr(pos + 1);

		ReadParameterNames(paramNamesString, paramNames);

		if (paramNames->size() > 0) {
			ReadParameterValues(paramValuesString, paramNames, config);
		}
	}
	catch (...) {
		cerr << "Exception reading the parameter string: " << line << endl;
	}

	delete paramNames;
}

void ReadParams(ifstream& inFile, Config* config) {
	string line;

	vector<string> *paramNames = new vector<string>();
	if (getline(inFile, line)) {
		ReadParameterNames(line, paramNames);
	}

	if (paramNames->size() > 0) {
		while (getline(inFile, line)) {
			if (line == "\n" || line.empty()) {
				break;
			}

			ReadParameterValues(line, paramNames, config);
		}
	}

	delete paramNames;
}

void ReadDataTypeConfig(string line, Config* config) {
	try {
		config->datatypeSize = stol(line, nullptr, 10);
	}
	catch (const invalid_argument) {
		cerr << "Datatype size line: " << line << endl;
		cerr << "Invalid datatype size while reading the config file" << endl;
		exit(1);
	}
}


void ReadDataTypeConfig(ifstream& inFile, Config* config) {
	string line;
	while (getline(inFile, line)) {
		if (line == "\n" || line.empty()) {
			break;
		}

		ReadDataTypeConfig(line, config);
	}
}

void InitializeConfig(Config* config) {
	config->systemConfig = new SystemConfig;
	config->programParameterVector = new vector<unordered_map<string, int>*>();
	config->datatypeSize = 0;
	config->systemConfig->L1 = 0;
	config->systemConfig->L2 = 0;
	config->systemConfig->L3 = 0;
	config->systemConfig->L1Shared = false;
	config->systemConfig->L2Shared = false;
	config->systemConfig->L3Shared = false;
}

void CheckIfConfigIsFullySpecified(Config* config) {
	if (config->systemConfig->L1 == 0) {
		cout << "L1 cache size not found in config file" << endl;
		exit(1);
	}

	if (config->systemConfig->L2 == 0) {
		cout << "L2 cache size not found in config file" << endl;
		exit(1);
	}

	if (config->systemConfig->L3 == 0) {
		cout << "L3 cache size not found in config file" << endl;
		exit(1);
	}

	if (config->datatypeSize == 0) {
		cout << "Data type size not found in config file" << endl;
		exit(1);
	}
}

void PrintConfig(Config* config) {
	cout << "Datatype size: " << config->datatypeSize << endl;
	cout << "L1 cache size: " << config->systemConfig->L1 << endl;
	cout << "L2 cache size: " << config->systemConfig->L2 << endl;
	cout << "L3 cache size: " << config->systemConfig->L3 << endl;

	cout << "Program parameters:" << endl;
	for (int i = 0; i < config->programParameterVector->size(); i++) {
		unordered_map<string, int>* params = config->programParameterVector->at(i);
		for (auto it = params->begin(); it != params->end(); it++) {
			cout << it->first << ": " << it->second << " ";
		}

		cout << endl;
	}

	cout << "Parallel loops" << endl;
	if (config->parallelLoops) {
		for (int i = 0; i < config->parallelLoops->size(); i++) {
			cout << config->parallelLoops->at(i) << " ";
		}

		cout << endl;
	}
	else {
		cout << "None" << endl;
	}
}

void FreeConfig(Config* config) {
	delete config->systemConfig;
	for (int i = 0; i < config->programParameterVector->size(); i++) {
		delete config->programParameterVector->at(i);
	}

	if (config->parallelLoops) {
		config->parallelLoops->clear();
		delete config->parallelLoops;
	}

	delete config->programParameterVector;
	delete config;
}