#ifndef CONFIG_PROCESSOR_HPP
#define CONFIG_PROCESSOR_HPP

#include<string.h>
#include<vector>
#include <unordered_map>
#include <OptionsProcessor.hpp>

struct SystemConfig {
	long L1; // in bytes
	long L2; // in bytes
	long L3; // in bytes
	bool L1Shared;
	bool L2Shared;
	bool L3Shared;
};

typedef struct SystemConfig SystemConfig;

struct Config {
	SystemConfig *systemConfig;
	std::vector<std::unordered_map<std::string, int>*> *programParameterVector;
	int datatypeSize;
	std::vector<std::string> *parallelLoops;
};

typedef struct Config Config;

void ReadConfig(UserInput *userInput, Config* config);
void FreeConfig(Config* config);
void PrintConfig(Config* config);
#endif