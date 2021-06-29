#ifndef OPTIONS_PROCESSOR_HPP
#define OPTIONS_PROCESSOR_HPP

#include <string>

struct UserInput {
	std::string inputFile;
	std::string configFile;
	std::string parameters;
	std::string cachesizes;
	std::string datatypesize;
	std::string parallelLoops;
	std::string sharedcaches;
	int numProcs;
	bool interactive;
	bool minOutput;
	bool perarray;
};

typedef struct UserInput UserInput;
void ReadUserInput(int argc, char **argv, UserInput *userInput);


#endif