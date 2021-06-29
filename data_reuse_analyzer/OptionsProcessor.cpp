#include <OptionsProcessor.hpp>
#include <stdlib.h>
#include <iostream>
using namespace std;

void ReadUserInput(int argc, char **argv, UserInput *userInput) {
	int i = 1;

	/*
	Example command line:
	./polyscientist --input conv2d.c --config conv2d_config
	*/
	string inputPrefix = "--input";
	string configPrefix = "--config";
	string diagnostic = "--diagnostic";
	string minimalOutput = "--minout";
	string perarray = "--perarray";
	string parameters = "--parameters";
	string cachesizes = "--cachesizes";
	string datatypesize = "--datatypesize";
	string parallelLoops = "--parallel_loops";
	string numProcs = "--numprocs";
	string sharedcaches = "--sharedcaches";

	userInput->interactive = false;
	userInput->minOutput = false;
	userInput->perarray = false;
	userInput->numProcs = 1;

	for (i = 1; i < argc;) {
		if (argv[i] == inputPrefix) {
			userInput->inputFile = argv[i + 1];
			i += 2;
		}
		else if (argv[i] == configPrefix) {
			userInput->configFile = argv[i + 1];
			i += 2;
		}
		else if (argv[i] == diagnostic) {
			userInput->interactive = true;
			i++;
		}
		else if (argv[i] == minimalOutput) {
			userInput->minOutput = true;
			i++;
		}
		else if (argv[i] == perarray) {
			userInput->perarray = true;
			i++;
		}
		else if (argv[i] == parameters) {
			userInput->parameters = argv[i + 1];
			i += 2;
		}
		else if (argv[i] == cachesizes) {
			userInput->cachesizes = argv[i + 1];
			i += 2;
		}
		else if (argv[i] == datatypesize) {
			userInput->datatypesize = argv[i + 1];
			i += 2;
		}
		else if (argv[i] == parallelLoops) {
			userInput->parallelLoops = argv[i + 1];
			i += 2;
		}
		else if (argv[i] == numProcs) {
			userInput->numProcs = atoi(argv[i + 1]);
			i += 2;

			if (userInput->numProcs <= 0) {
				cout << "The number of processors has to greater than zero. The entered value is: " <<
					userInput->numProcs << " Quitting. " << endl;
				exit(0);
			}
		}
		else if (argv[i] == sharedcaches) {
			userInput->sharedcaches = argv[i + 1];
			i += 2;
		}
		else {
			printf("Unexpected command line input: %s. Exiting\n", argv[i]);
			exit(1);
		}
	}

	if (userInput->inputFile.empty()) {
		printf("Input file not specified. Exiting\n");
		exit(1);
	}
	else {
		cout << "Input file: " << userInput->inputFile << endl;
	}

	if (userInput->configFile.empty()
		&& !userInput->interactive) {
		if (userInput->parameters.empty()
			|| userInput->cachesizes.empty() ||
			userInput->datatypesize.empty()) {
			printf("Config file not specified. Exiting\n");
			exit(1);
		}
	}
	else {
		cout << "Config file: " << userInput->configFile << endl;
	}

	if (!userInput->parallelLoops.empty() && userInput->numProcs == 1) {
		cout << "The parallel loops are specified. However the number of processors is 1. "
			<< "The number of processors must be greater than 1. Quitting" << endl;
		exit(1);
	}
}