#include <pet.h>
#include <iostream>
#include <isl/union_set.h>
#include <isl/flow.h>
#include <isl/id.h>
#include <stdlib.h>
#include <barvinok/isl.h>
#include <string.h>
#include <vector>
#include <isl/space.h>
#include <unordered_map>
#include <bits/stdc++.h>
#include <fstream>
#include <ConfigProcessor.hpp>
#include <OptionsProcessor.hpp>
#include <Utility.hpp>
#include <algorithm>
using namespace std;


#define IGNORE_WS_SIZE_ONE 1
#define DEBUG 0
#define SCALEDATASETSIZEATL3 0

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#define max(X, Y) (((X) > (Y)) ? (X) : (Y))


/* Function header declarations begin */
struct WorkingSetSize {
	isl_basic_map* dependence;
	isl_set* source;
	isl_set* target;
	isl_set *minTarget;
	isl_set *maxTarget;
	isl_union_pw_qpolynomial* minSize;
	isl_union_pw_qpolynomial* maxSize;
	bool parallelLoop;
	long size;
	long dataSetUnionCardInt;
	long dataSetCommonCardInt;
};

typedef struct WorkingSetSize WorkingSetSize;

struct ProgramCharacteristics {
	int L1Fit; // #working sets that fit in L1 cache
	int L2Fit; // #working sets that fit in L2 cache
	int L3Fit; // #working sets that fit in L3 cache
	int MemFit;
	int datatypeSize; // size of datatype of arrays
	long L1DataSetSize;
	long L2DataSetSize;
	long L3DataSetSize;
	long MemDataSetSize;
	long PessiL1DataSetSize;
	long PessiL2DataSetSize;
	long PessiL3DataSetSize;
	long PessiMemDataSetSize;
};

typedef struct ProgramCharacteristics ProgramCharacteristics;

struct MinMaxTuple {
	long min;
	long max;
	bool isParallelLoopEncountered;
	long dataSetUnionCardInt;
	long dataSetCommonCardInt;
};

typedef struct MinMaxTuple MinMaxTuple;

void GetSystemAndProgramCharacteristics(SystemConfig* systemConfig,
	ProgramCharacteristics* programChar);
void InitializeProgramCharacteristics(ProgramCharacteristics* programChar);
void UpdateProgramCharacteristics(long size, SystemConfig* systemConfig,
	ProgramCharacteristics* programChar);

struct ArgComputeWorkingSetSizesForDependence {
	pet_scop *scop;
	vector<WorkingSetSize*>* workingSetSizes;
	isl_union_map* may_reads;
	isl_union_map* may_writes;
	Config* config;
};

typedef struct ArgComputeWorkingSetSizesForDependence  ArgComputeWorkingSetSizesForDependence;

struct ArrayDataAccesses {
	isl_union_map* may_reads;
	isl_union_map* may_writes;
	isl_union_map* dependences;
};

typedef struct ArrayDataAccesses ArrayDataAccesses;

struct DimPositions {
	int input;
	int output;
	int inputDim;
	int outputDim;
};

typedef struct DimPositions DimPositions;


struct ParallelDependenceDetectionData {
	std::vector<std::string> *parallelLoops;
	bool parallelDependence;
	string parallelLoop;
};

typedef struct ParallelDependenceDetectionData ParallelDependenceDetectionData;

void ComputeDataReuseWorkingSets(UserInput *userInput, Config *config);
pet_scop* ParseScop(isl_ctx* ctx, const char *fileName);
isl_stat ComputeWorkingSetSizesForDependence(isl_map* dep, void *user);
isl_stat ComputeWorkingSetSizesForDependenceBasicMap(isl_basic_map* dep,
	void *user);
isl_union_pw_qpolynomial* ComputeDataSetSize(isl_union_set* WS,
	isl_union_map *may_reads, isl_union_map *may_writes);
void FreeWorkingSetSizes(vector<WorkingSetSize*>* workingSetSizes);
void PrintWorkingSetSizes(vector<WorkingSetSize*>* workingSetSizes);
void SimplifyWorkingSetSizesInteractively(vector<WorkingSetSize*>* workingSetSizes,
	UserInput *userInput, Config *config);
void SimplifyWorkingSetSizes(vector<WorkingSetSize*>* workingSetSizes,
	UserInput *userInput, Config *config, pet_scop *scop);
string SimplifyUnionPwQpolynomial(isl_union_pw_qpolynomial* size,
	unordered_map<string, int>* paramValues);
unordered_map<string, int>* GetParameterValues(vector<WorkingSetSize*>* workingSetSizes);
int findInParamsMap(unordered_map<string, int>* map, string key);
long ExtractIntegerFromUnionPwQpolynomial(isl_union_pw_qpolynomial* poly);
string GetParameterValuesString(unordered_map<string, int>* paramValues);
isl_union_map* ComputeDataDependences(isl_union_map *source,
	isl_union_map *target, isl_schedule* schedule);
void OrchestrateDataReuseComputation(int argc, char **argv);
string ExtractFileName(string fileName);
MinMaxTuple* AddToVectorIfUniqueDependence(vector<MinMaxTuple*> *minMaxTupleVector,
	long min, long max, bool isParallelLoopEncountered);
void FreeMinMaxTupleVector(vector<MinMaxTuple*> *minMaxTupleVector);
long ConvertStringToLong(string sizeStr);
isl_set* ConstructContextEquatingParametersToConstants(
	isl_space* space, unordered_map<string, int>* paramValues);
isl_union_map* SimplifyUnionMap(isl_union_map* map,
	unordered_map<string, int>* paramValues);
void UpdatePessimisticProgramCharacteristics(long minSize, long maxSize,
	bool isParallelLoopEncountered,
	bool doesParallelLoopExist,
	SystemConfig* systemConfig,
	ProgramCharacteristics* programChar,
	int numProcs, long totalDataSetSize, long dataSetUnionCardInt, long dataSetCommonCardInt);
bool compareByMinMaxSize(const MinMaxTuple* a, const MinMaxTuple* b);
vector<WorkingSetSize*>* ComputeWorkingSetSizesForDependences(
	UserInput *userInput,
	unordered_map<int, ArrayDataAccesses*>* dependenceMap,
	pet_scop *scop, Config *config);
ArrayDataAccesses* ComputeAllDataDependences(isl_union_map* may_reads,
	isl_union_map* may_writes, isl_schedule* schedule);
unordered_map<int, ArrayDataAccesses*>* ComputeDataDependences(
	UserInput *userInput,
	isl_ctx* ctx, pet_scop* scop,
	Config *config);
void FreeDependenceMap(
	unordered_map<int, ArrayDataAccesses*>* dependenceMap);
isl_union_pw_qpolynomial* ComputeDataSetSize(isl_basic_set* sourceDomain,
	isl_set* source, isl_set* target, isl_union_map* may_reads,
	isl_union_map* may_writes);
void RecognizeParallelIterationSpanningDependences(
	unordered_map<int, ArrayDataAccesses*>* dependenceMap,
	Config *config);
isl_stat RecognizeParallelIterationSpanningDependenceMap(isl_map* dep,
	void *user);
isl_stat RecognizeParallelIterationSpanningDependenceBasicMap(
	isl_basic_map* dep,
	void *user);
DimPositions* FindPositionOfParallelLoop(string var,
	isl_basic_map *map);
isl_basic_map* EliminateAllConstraintsInnerToParallelLoop(DimPositions* dimPositions,
	isl_basic_map *deps);
isl_basic_map* EliminateInnerConstraintsIncludingParallelLoop(
	DimPositions* dimPositions,
	isl_basic_map *deps);
isl_basic_map* RetailOnlyParallelLoopConstraints(
	DimPositions* dimPositions,
	isl_basic_map *deps);
bool AreInequalitiesPresent(isl_basic_map* deps);
bool IsDependenceProcessorCrossing(string parallelLoop, isl_basic_map *deps);
bool AreInequalitiesPresentOuterToParallelLoop(
	DimPositions* dimPositions,
	isl_basic_map *deps, string parallelLoop);
bool AreInequalitiesPresentInTheParallelLoop(
	DimPositions* dimPositions,
	isl_basic_map *deps, string parallelLoop);
vector<string>* GetLoopVariables(isl_basic_map *deps, int firstIn, int nIn, int firstOut, int nOut);
bool DoesLoopVariableOccurInInqualityConstraints(isl_basic_map *deps, const char* loopVar);
int FindThePositionOfTheLoopVariable(isl_basic_set *bset,
	vector<string> *parallelLoops);
isl_basic_set* ProjectBSetToLexExtreme(isl_basic_set* sourceDomain,
	isl_set* sourceDomainLexExtreme, int pos);
void ComputeWorkingSetSize(isl_set*  min, isl_set* max,
	isl_union_map* may_reads,
	isl_union_map* may_writes, Config* config, isl_basic_set* domain, int pos,
	WorkingSetSize* workingSetSize);
isl_union_set* SimplifyUnionSet(isl_union_set* set,
	unordered_map<string, int>* paramValues);
long ComputeNumberOfItersInParallelLoop(isl_basic_set* bset, int pos,
	unordered_map<string, int>* paramValues);
isl_basic_set* SimplifyBasicSet(isl_basic_set* bset,
	unordered_map<string, int>* paramValues);
isl_union_pw_qpolynomial* ComputeTotalDataSetSize(pet_scop *scop);
void PrintWorkingSetSize(WorkingSetSize* wss);
/* Function header declarations end */

int main(int argc, char **argv) {
	OrchestrateDataReuseComputation(argc, argv);
	return 0;
}

void OrchestrateDataReuseComputation(int argc, char **argv) {
	string fileName = "../apps/padded_conv_fp_stride_1_libxsmm_core2.c";

	UserInput *userInput = new UserInput;
	ReadUserInput(argc, argv, userInput);

	Config *config = NULL;

	if (!userInput->configFile.empty() ||
		!userInput->interactive) {
		config = new Config;
		ReadConfig(userInput, config);
		if (DEBUG) {
			PrintConfig(config);
		}
	}

	ComputeDataReuseWorkingSets(userInput, config);

	if (!userInput->interactive) {
		FreeConfig(config);
	}

	delete userInput;
}


void ComputeDataReuseWorkingSets(UserInput *userInput, Config *config) {
	isl_ctx* ctx = isl_ctx_alloc_with_pet_options();
	pet_scop *scop = ParseScop(ctx, userInput->inputFile.c_str());
	unordered_map<int, ArrayDataAccesses*>* dependenceMap = ComputeDataDependences(userInput, ctx, scop, config);

	if (dependenceMap->size() == 0) {
		cout << "No depdendences found. Quitting" << endl;
		exit(1);
	}

	vector<WorkingSetSize*>* workingSetSizes =
		ComputeWorkingSetSizesForDependences(userInput,
			dependenceMap, scop, config);

	if (DEBUG) {
		PrintWorkingSetSizes(workingSetSizes);
	}

	/*TODO: Deduplicate results*/
	if (userInput->interactive) {
		SimplifyWorkingSetSizesInteractively(workingSetSizes,
			userInput, config);
	}
	else {
		SimplifyWorkingSetSizes(workingSetSizes, userInput, config, scop);
	}

	FreeWorkingSetSizes(workingSetSizes);
	FreeDependenceMap(dependenceMap);
	pet_scop_free(scop);
	isl_ctx_free(ctx);
}

void FreeDependenceMap(
	unordered_map<int, ArrayDataAccesses*>* dependenceMap) {

	for (auto i : *dependenceMap) {
		isl_union_map_free(i.second->may_reads);
		isl_union_map_free(i.second->may_writes);
		isl_union_map_free(i.second->dependences);
		delete i.second;
	}

	dependenceMap->clear();
	delete dependenceMap;
}


void RecognizeParallelIterationSpanningDependences(
	unordered_map<int, ArrayDataAccesses*>* dependenceMap,
	Config *config) {

	if (DEBUG) {
		cout << "In RecognizeParallelIterationSpanningDependences()" << endl;
	}

	for (auto i : *dependenceMap) {
		isl_union_map_foreach_map(i.second->dependences,
			&RecognizeParallelIterationSpanningDependenceMap, config->parallelLoops);
	}
}


vector<WorkingSetSize*>* ComputeWorkingSetSizesForDependences(
	UserInput *userInput,
	unordered_map<int, ArrayDataAccesses*>* dependenceMap,
	pet_scop *scop, Config *config) {
	/*TODO: The following code works for perfectly nested loops
	only. It needs to be extended to cover data dependences that span
	across loops*/

	/* Here we assume that only may_dependences will be present because
	ComputeDataDependences() function is specifying only may_read,
	and may_write references */

	if (DEBUG) {
		for (auto i : *dependenceMap) {
			cout << "Dependence for array number: " << i.first << endl;
			PrintUnionMap(i.second->dependences);
		}
	}

	vector<WorkingSetSize*>* workingSetSizes =
		new vector<WorkingSetSize*>();
	ArgComputeWorkingSetSizesForDependence* arg =
		(ArgComputeWorkingSetSizesForDependence*)malloc(
			sizeof(ArgComputeWorkingSetSizesForDependence));
	arg->scop = scop;
	arg->workingSetSizes = workingSetSizes;
	arg->config = config;

	for (auto i : *dependenceMap) {
		arg->may_reads = i.second->may_reads;
		arg->may_writes = i.second->may_writes;
		isl_union_map_foreach_map(i.second->dependences,
			&ComputeWorkingSetSizesForDependence, arg);
	}

	free(arg);
	return workingSetSizes;
}

isl_stat RecognizeParallelIterationSpanningDependenceMap(isl_map* dep, void *user) {
	ParallelDependenceDetectionData *parallelDependenceDetectionData
		= new ParallelDependenceDetectionData;
	parallelDependenceDetectionData->parallelLoops = (vector<string>*)user;

	isl_map_foreach_basic_map(dep,
		&RecognizeParallelIterationSpanningDependenceBasicMap,
		parallelDependenceDetectionData);

	delete parallelDependenceDetectionData;
	return isl_stat_ok;
}

isl_stat RecognizeParallelIterationSpanningDependenceBasicMap(
	isl_basic_map* dep,
	void *user) {
	ParallelDependenceDetectionData *parallelDependenceDetectionData =
		(ParallelDependenceDetectionData *)user;
	vector<string> *parallelLoops = parallelDependenceDetectionData->parallelLoops;
	parallelDependenceDetectionData->parallelDependence = false;

	if (parallelLoops) {
		if (DEBUG) {
			for (int i = 0; i < parallelLoops->size(); i++) {
				cout << parallelLoops->at(i) << " ";
			}

			cout << endl;
		}

		for (int i = 0; i < parallelLoops->size(); i++) {
			bool dependenceCrossing = IsDependenceProcessorCrossing(parallelLoops->at(i), dep);

			if (DEBUG) {
				cout << "In RecognizeParallelIterationSpanningDependenceBasicMap()" << endl;
				PrintBasicMap(dep);
				cout << "The dependence with respect to the parallel loop " << parallelLoops->at(i);

				if (dependenceCrossing) {
					cout << " IS_processor_crossing";
				}
				else {
					cout << " is_NOT_processor_crossing";
				}

				cout << endl;
			}

			if (dependenceCrossing) {
				parallelDependenceDetectionData->parallelDependence = true;
				parallelDependenceDetectionData->parallelLoop = parallelLoops->at(i);
				break;
			}
		}

		if (DEBUG) {
			cout << "*********************" << endl;
		}
	}

	return isl_stat_ok;
}

bool IsDependenceProcessorCrossing(string parallelLoop, isl_basic_map *deps) {
	bool isDependenceProcessorCrossing = false;
	DimPositions* dimPositions = FindPositionOfParallelLoop(parallelLoop, deps);

	/*FIXME: The code works for perfectly nested loops only.*/
	if (dimPositions->input == -1 || dimPositions->output == -1 ||
		isl_basic_map_dim(deps, isl_dim_in) != isl_basic_map_dim(deps, isl_dim_out)) {
		isDependenceProcessorCrossing = false;
	}
	else {
		if (DEBUG) {
			cout << "In IsDependenceProcessorCrossing()" << endl;
			cout << "The parallel loop variable " << parallelLoop << " is found at "
				<< dimPositions->input << " input position and " << dimPositions->output
				<< " output position" << endl;
		}

		if (AreInequalitiesPresentOuterToParallelLoop(dimPositions, deps, parallelLoop)) {
			if (DEBUG) {
				cout << "AreInequalitiesPresentOuterToParallelLoop is TRUE" << endl;
			}

			isDependenceProcessorCrossing = false;
		}
		else {
			if (DEBUG) {
				cout << "AreInequalitiesPresentOuterToParallelLoop is FALSE" << endl;
			}

			if (AreInequalitiesPresentInTheParallelLoop(dimPositions, deps, parallelLoop)) {
				if (DEBUG) {
					cout << "AreInequalitiesPresentInTheParallelLoop is TRUE" << endl;
				}

				isDependenceProcessorCrossing = true;
			}
			else {
				if (DEBUG) {
					cout << "AreInequalitiesPresentInTheParallelLoop is FALSE" << endl;
				}
			}
		}
	}

	return isDependenceProcessorCrossing;
}

bool AreInequalitiesPresentOuterToParallelLoop(
	DimPositions* dimPositions,
	isl_basic_map *deps,
	string parallelLoop) {
	bool inequalityPresent = false;

	vector<string>* loopVarsOuterToParallelLoop =
		GetLoopVariables(deps, 0, dimPositions->input, 0, dimPositions->output);

	if (DEBUG) {
		cout << "The dependence: " << endl;
		PrintBasicMap(deps);
		cout << "Loop variables outer to the parallel loop << " << parallelLoop
			<< " the loop variables from 0 -> " << dimPositions->input <<
			" and from 0 -> " << dimPositions->output << " are: " << endl;
		for (int i = 0; i < loopVarsOuterToParallelLoop->size(); i++) {
			cout << loopVarsOuterToParallelLoop->at(i) << " ";
		}

		cout << endl;
	}

	for (int k = 0; k < loopVarsOuterToParallelLoop->size(); k++) {
		const char* loopVar = (loopVarsOuterToParallelLoop->at(k)).c_str();
		if (DEBUG) {
			cout << "loopVar: " << loopVar << " ";
		}

		if (DoesLoopVariableOccurInInqualityConstraints(deps, loopVar)) {
			inequalityPresent = true;
		}
	}


	loopVarsOuterToParallelLoop->clear();
	delete loopVarsOuterToParallelLoop;
	return inequalityPresent;
}

bool DoesLoopVariableOccurInInqualityConstraints(isl_basic_map *deps, const char* loopVar) {
	bool inequalityPresent = false;
	bool inequalityPresentInInputDims = false;
	bool inequalityPresentInOutputDims = false;

	isl_size inputDim = isl_basic_map_dim(deps, isl_dim_in);
	isl_size outputDim = isl_basic_map_dim(deps, isl_dim_out);

	if (DEBUG) {
		cout << "inputDim: " << inputDim << endl;
		cout << "outputDim: " << outputDim << endl;
	}

	isl_constraint_list *constraints =
		isl_basic_map_get_constraint_list(deps);
	isl_size constraintSize = isl_constraint_list_size(constraints);
	for (int i = 0; i < constraintSize; i++) {
		isl_constraint* constraint = isl_constraint_list_get_at(constraints, i);
		if (DEBUG) {
			cout << "CONSTRAINT: " << endl;
			PrintConstraint(constraint);
			if (isl_constraint_is_equality(constraint)) {
				cout << "EQUALITY" << endl;
			}
			else {
				cout << "INequality" << endl;
			}
		}

		if (!isl_constraint_is_equality(constraint)) {
			for (int j = 0; j < inputDim; j++) {
				const char *name = isl_basic_map_get_dim_name(deps, isl_dim_in, j);

				if (DEBUG) {
					cout << "Checking for variable at position: " << j << " " << name << endl;
				}

				if (strcmp(name, loopVar) == 0) {
					isl_bool involves = isl_constraint_involves_dims(
						constraint,
						isl_dim_in, j, 1);

					if (involves) {
						inequalityPresentInInputDims = true;
					}

					if (DEBUG) {
						if (involves) {
							cout << "Input " << name << " IS involved in the constraint in the input dims" << endl;
						}
						else {
							cout << "Input " << name << " is NOT involved in the constraint in the input dims" << endl;
						}
					}
				}
			}

			for (int j = 0; j < outputDim; j++) {
				const char *name = isl_basic_map_get_dim_name(deps, isl_dim_out, j);

				if (DEBUG) {
					cout << "Checking for variable at position: " << j << " " << name << endl;
				}

				if (strcmp(name, loopVar) == 0) {
					isl_bool involves = isl_constraint_involves_dims(
						constraint,
						isl_dim_out, j, 1);

					if (involves) {
						inequalityPresentInOutputDims = true;
					}

					if (DEBUG) {
						if (involves) {
							cout << "Input " << name << " IS involved in the constraint in the output dims" << endl;
						}
						else {
							cout << "Input " << name << " is NOT involved in the constraint in the output dims" << endl;
						}
					}
				}
			}
		}
	}


	if (inequalityPresentInInputDims && inequalityPresentInOutputDims) {
		inequalityPresent = true;
	}

	return inequalityPresent;
}

vector<string>* GetLoopVariables(isl_basic_map *deps, int firstIn, int nIn, int firstOut, int nOut) {
	vector<string>* loopVars = new vector<string>();

	isl_size inputDim = isl_basic_map_dim(deps, isl_dim_in);
	isl_size outputDim = isl_basic_map_dim(deps, isl_dim_out);

	for (int i = firstIn; i < firstIn + nIn; i++) {
		const char *name = isl_basic_map_get_dim_name(deps, isl_dim_in, i);
		loopVars->push_back(name);
	}

	for (int i = firstOut; i < firstOut + nOut; i++) {
		const char *name = isl_basic_map_get_dim_name(deps, isl_dim_out, i);
		loopVars->push_back(name);
	}

	return loopVars;
}

bool AreInequalitiesPresentInTheParallelLoop(
	DimPositions* dimPositions,
	isl_basic_map *deps,
	string parallelLoop) {
	return DoesLoopVariableOccurInInqualityConstraints(deps, parallelLoop.c_str());
}

bool AreInequalitiesPresent(isl_basic_map* deps) {

	isl_mat *equalities = isl_basic_map_equalities_matrix(deps,
		isl_dim_cst, isl_dim_in,
		isl_dim_out, isl_dim_param, isl_dim_div);

	isl_mat *inequalities = isl_basic_map_inequalities_matrix(deps,
		isl_dim_cst, isl_dim_in,
		isl_dim_out, isl_dim_param, isl_dim_div);

	if (DEBUG) {
		cout << "Equalities matrix: " << endl;
		PrintMat(equalities);

		cout << endl << "Inequalities matrix: " << endl;
		PrintMat(inequalities);
	}

	bool inequalitiesPresent = false;

	isl_constraint_list *constraints =
		isl_basic_map_get_constraint_list(deps);
	isl_size constraintSize = isl_constraint_list_size(constraints);
	for (int i = 0; i < constraintSize; i++) {
		isl_constraint* constraint = isl_constraint_list_get_at(constraints, i);
		PrintConstraint(constraint);
		if (isl_constraint_is_equality(constraint) == false) {
			inequalitiesPresent = true;
		}
	}

	if (DEBUG) {
		cout << "constraintSize: " << constraintSize << endl;
		cout << "In AreInequalitiesPresent() for: " << endl;
		PrintBasicMap(deps);
		cout << "inequalitiesPresent: " << inequalitiesPresent << endl;
	}

	return inequalitiesPresent;
}

isl_basic_map* EliminateAllConstraintsInnerToParallelLoop(
	DimPositions* dimPositions,
	isl_basic_map *deps) {

	if (DEBUG) {
		cout << "In EliminateAllConstraintsInnerToParallelLoop()" << endl;
	}

	return
		isl_basic_map_drop_constraints_involving_dims(
			isl_basic_map_drop_constraints_involving_dims(
				deps,
				isl_dim_in,
				dimPositions->input + 1,
				dimPositions->inputDim - (dimPositions->input + 1)),
			isl_dim_out,
			dimPositions->output + 1,
			dimPositions->outputDim - (dimPositions->output + 1));
}

isl_basic_map* EliminateInnerConstraintsIncludingParallelLoop(
	DimPositions* dimPositions,
	isl_basic_map *deps) {

	// return deps;
	if (DEBUG) {
		cout << "In EliminateInnerConstraintsIncludingParallelLoop()" << endl;
	}

	return
		isl_basic_map_drop_constraints_involving_dims(
			isl_basic_map_drop_constraints_involving_dims(
				deps,
				isl_dim_in,
				dimPositions->input,
				dimPositions->inputDim - dimPositions->input),
			isl_dim_out,
			dimPositions->output,
			dimPositions->outputDim - dimPositions->output);
}

isl_basic_map* RetailOnlyParallelLoopConstraints(
	DimPositions* dimPositions,
	isl_basic_map *deps) {

	// return deps;

	if (DEBUG) {
		cout << "In RetailOnlyParallelLoopConstraints()" << endl;
	}

	return
		isl_basic_map_drop_constraints_not_involving_dims(
			isl_basic_map_drop_constraints_not_involving_dims(
				deps,
				isl_dim_in,
				dimPositions->input,
				1),
			isl_dim_out,
			dimPositions->output,
			1);
}

DimPositions* FindPositionOfParallelLoop(string var,
	isl_basic_map *map) {
	DimPositions* dimPositions = new DimPositions;
	dimPositions->input = -1;
	dimPositions->output = -1;
	isl_size inputDim = isl_basic_map_dim(map, isl_dim_in);
	isl_size outputDim = isl_basic_map_dim(map, isl_dim_out);
	dimPositions->inputDim = inputDim;
	dimPositions->outputDim = outputDim;

	for (int i = 0; i < inputDim; i++) {
		const char *name = isl_basic_map_get_dim_name(map, isl_dim_in, i);
		if (strcmp(name, var.c_str()) == 0) {
			dimPositions->input = i;
		}
	}

	for (int i = 0; i < outputDim; i++) {
		const char *name = isl_basic_map_get_dim_name(map, isl_dim_out, i);
		if (strcmp(name, var.c_str()) == 0) {
			dimPositions->output = i;
		}
	}

	return dimPositions;
}

isl_stat ComputeWorkingSetSizesForDependence(isl_map* dep, void *user) {
	isl_map_foreach_basic_map(dep,
		&ComputeWorkingSetSizesForDependenceBasicMap,
		user);
	return isl_stat_ok;
}

isl_stat ComputeWorkingSetSizesForDependenceBasicMap(isl_basic_map* dep,
	void *user) {
	ArgComputeWorkingSetSizesForDependence* arg =
		(ArgComputeWorkingSetSizesForDependence*)user;
	pet_scop *scop = arg->scop;
	isl_union_map* may_reads = arg->may_reads;
	isl_union_map* may_writes = arg->may_writes;
	Config* config = arg->config;
	vector<WorkingSetSize*>* workingSetSizes = arg->workingSetSizes;

	ParallelDependenceDetectionData *parallelDependenceDetectionData
		= new ParallelDependenceDetectionData;
	parallelDependenceDetectionData->parallelLoops = arg->config->parallelLoops;
	RecognizeParallelIterationSpanningDependenceBasicMap(dep, parallelDependenceDetectionData);
	WorkingSetSize* workingSetSize =
		(WorkingSetSize*)malloc(sizeof(WorkingSetSize));
	workingSetSize->dependence = dep;

	if (parallelDependenceDetectionData->parallelDependence == false) {
		if (DEBUG) {
			cout << "Data_dependence: " << endl;
			PrintBasicMap(dep);
			cout << "May_writes: " << endl;
			PrintUnionMap(may_writes);
			cout << "May_reads: " << endl;
			PrintUnionMap(may_reads);
		}

		isl_basic_set* sourceDomain = isl_basic_map_domain(
			isl_basic_map_copy(dep));

		isl_set* source = isl_basic_set_lexmin(
			isl_basic_set_copy(sourceDomain));

		isl_set* target = isl_set_apply(isl_set_copy(source),
			isl_map_from_basic_map(dep));

		isl_set* minTarget = isl_set_lexmin(isl_set_copy(target));
		isl_set* maxTarget = isl_set_lexmax(isl_set_copy(target));

		if (DEBUG) {
			cout << "Computing minWSSize: " << endl;
		}

		isl_union_pw_qpolynomial* minWSSize =
			ComputeDataSetSize(sourceDomain, source, minTarget, may_reads,
				may_writes);

		if (DEBUG) {
			cout << "Computing maxWSSize: " << endl;
		}

		isl_union_pw_qpolynomial* maxWSSize =
			ComputeDataSetSize(sourceDomain, source, maxTarget, may_reads,
				may_writes);

		workingSetSize->source = source;
		workingSetSize->target = target;
		workingSetSize->minTarget = minTarget;
		workingSetSize->maxTarget = maxTarget;
		workingSetSize->minSize = minWSSize;
		workingSetSize->maxSize = maxWSSize;
		workingSetSize->parallelLoop = false;
		workingSetSize->size = 0;
		workingSetSize->dataSetUnionCardInt = 0;
		workingSetSize->dataSetCommonCardInt = 0;
		isl_basic_set_free(sourceDomain);
	}
	else {
		// Parallel dependence branch
		// 1. Project out all dimensions starting from the parallel loop to all outer loops
		// 2. Compute the data set accessed in one iteration of the parallel loop
		// 3. Compute the data accessed in all iterations of the parallel loop
		//    3.a) Compute the data accessed in the lexmin, and lexmax
		//    3.b) Estimate the data accessed in all the iterations of the loop

		isl_basic_set* sourceDomain = isl_basic_map_domain(
			isl_basic_map_copy(dep));

		int pos = FindThePositionOfTheLoopVariable(sourceDomain,
			parallelDependenceDetectionData->parallelLoops);

		if (DEBUG) {
			cout << "Data_dependence: " << endl;
			PrintBasicMap(dep);
			cout << "position of the parallel loop: " << pos << endl;
			cout << "sourceDomain:" << endl;
			PrintBasicSet(sourceDomain);
		}

		isl_set* sourceDomainLexmin = isl_basic_set_lexmin(isl_basic_set_copy(sourceDomain));
		isl_basic_set* sourceDomainProjectedOuterLoopsProjected = ProjectBSetToLexExtreme(
			sourceDomain,
			sourceDomainLexmin, pos - 1);

		isl_set* sourceDomainProjectedLexMin = isl_basic_set_lexmin(
			isl_basic_set_copy(sourceDomainProjectedOuterLoopsProjected));

		isl_set* sourceDomainProjectedLexMax = isl_basic_set_lexmax(
			isl_basic_set_copy(sourceDomainProjectedOuterLoopsProjected));

		isl_set* sourceDomainProjectedMin = isl_set_from_basic_set(
			ProjectBSetToLexExtreme(
				sourceDomain,
				sourceDomainProjectedLexMin, pos));
		isl_set* sourceDomainProjectedMax = isl_set_from_basic_set(
			ProjectBSetToLexExtreme(
				sourceDomain,
				sourceDomainProjectedLexMax, pos));

		ComputeWorkingSetSize(sourceDomainProjectedMin, sourceDomainProjectedMax,
			may_reads, may_writes, config, sourceDomainProjectedOuterLoopsProjected,
			pos, workingSetSize);

		isl_basic_set_free(sourceDomain);
		isl_set_free(sourceDomainProjectedMin);
		isl_set_free(sourceDomainProjectedMax);
		isl_basic_set_free(sourceDomainProjectedOuterLoopsProjected);

		workingSetSize->source = sourceDomainLexmin;
		workingSetSize->target = NULL;
		workingSetSize->minTarget = NULL;
		workingSetSize->maxTarget = NULL;
		workingSetSize->minSize = NULL;
		workingSetSize->maxSize = NULL;
		workingSetSize->parallelLoop = true;

		if (DEBUG) {
			cout << "In ComputeWorkingSetSize:" << endl;
			cout << "workingSetSize->size: " << workingSetSize->size << endl;
			cout << "workingSetSize->dataSetUnionCardInt: "
				<< workingSetSize->dataSetUnionCardInt << endl;
			cout << "workingSetSize->dataSetCommonCardInt: "
				<< workingSetSize->dataSetCommonCardInt << endl;
		}
	}

	delete parallelDependenceDetectionData;
	workingSetSizes->push_back(workingSetSize);
	return isl_stat_ok;
}

isl_union_pw_qpolynomial* ComputeTotalDataSetSize(pet_scop *scop) {
	isl_union_map *all_may_reads = pet_scop_get_may_reads(scop);
	isl_union_map *all_may_writes = pet_scop_get_may_writes(scop);
	isl_union_set* totalDataSet = NULL;

	for (int i = 0; i < scop->n_stmt; i++) {
		isl_union_set* domain = isl_union_set_from_set(
			isl_set_copy(scop->stmts[i]->domain));
		isl_union_set* readSet = isl_union_set_apply(isl_union_set_copy(domain),
			isl_union_map_copy(all_may_reads));
		isl_union_set* writeSet = isl_union_set_apply(isl_union_set_copy(domain),
			isl_union_map_copy(all_may_writes));
		isl_union_set* dataSet = isl_union_set_union(readSet, writeSet);

		if (i == 0) {
			totalDataSet = dataSet;
		}
		else {
			totalDataSet = isl_union_set_union(totalDataSet, dataSet);
		}

		isl_union_set_free(domain);
	}

	return isl_union_set_card(totalDataSet);
}

long ComputeNumberOfItersInParallelLoop(isl_basic_set* bset, int pos,
	unordered_map<string, int>* paramValues) {
	// Project out the dimensions up to pos
	isl_basic_set* bsetProjected = isl_basic_set_copy(bset);
	isl_size dimSize = isl_basic_set_dim(bsetProjected, isl_dim_set);
	if (DEBUG) {
		cout << "pos: " << pos << " dimSize: " << dimSize << endl;
		cout << "bsetProjectedBefore: " << endl;
		PrintBasicSet(bsetProjected);
	}

	if (pos > 0) {
		bsetProjected = isl_basic_set_project_out(bsetProjected, isl_dim_set, 0, pos);
	}

	if (DEBUG) {
		cout << "bsetProjected after outer indexes are projected: " << endl;
		PrintBasicSet(bsetProjected);
	}

	if (pos < (dimSize - 1)) {
		int numDimsToEliminate = (dimSize - 1) - pos;
		if (DEBUG) {
			cout << "numDimsToEliminate: " << numDimsToEliminate << endl;
		}

		// Since the outer dimensions have been eliminated already, the index of the parallel
		// dimension must be 0. Thus, starting index 1 must be other inner sequential loops.
		bsetProjected = isl_basic_set_project_out(bsetProjected,
			isl_dim_set, 1, numDimsToEliminate);
	}

	if (DEBUG) {
		cout << "bsetProjected after inner indexes are projected: " << endl;
		PrintBasicSet(bsetProjected);
	}

	isl_basic_set* bsetProjectedSimplified = SimplifyBasicSet(bsetProjected, paramValues);

	if (DEBUG) {
		cout << "bsetProjectedSimplified: " << endl;
		PrintBasicSet(bsetProjectedSimplified);
	}

	isl_union_pw_qpolynomial* bsetProjectedSimplifiedCard =
		isl_union_set_card(isl_union_set_from_basic_set(bsetProjectedSimplified));

	long numIters = ExtractIntegerFromUnionPwQpolynomial(bsetProjectedSimplifiedCard);
	isl_union_pw_qpolynomial_free(bsetProjectedSimplifiedCard);
	isl_basic_set_free(bsetProjected);
	return numIters;
}

void ComputeWorkingSetSize(isl_set*  min, isl_set* max,
	isl_union_map* may_reads,
	isl_union_map* may_writes, Config* config, isl_basic_set* domain, int pos,
	WorkingSetSize* workingSetSize) {


	if (config == NULL || config->programParameterVector == NULL ||
		config->programParameterVector->size() != 1) {
		cout << "The number of sets of parameters required is 1" << endl;
		exit(1);
	}

	isl_union_set* writeSetMin =
		isl_union_set_apply(isl_union_set_from_set(isl_set_copy(min)),
			isl_union_map_copy(may_writes));

	isl_union_set* readSetMin =
		isl_union_set_apply(isl_union_set_from_set(isl_set_copy(min)),
			isl_union_map_copy(may_reads));

	isl_union_set* dataSetMinOrig = isl_union_set_union(writeSetMin, readSetMin);
	isl_union_set* dataSetMin = SimplifyUnionSet(dataSetMinOrig, config->programParameterVector->at(0));
	isl_union_pw_qpolynomial *dataSetMinCard = isl_union_set_card(isl_union_set_copy(dataSetMin));

	isl_union_set* writeSetMax =
		isl_union_set_apply(isl_union_set_from_set(isl_set_copy(max)),
			isl_union_map_copy(may_writes));

	isl_union_set* readSetMax =
		isl_union_set_apply(isl_union_set_from_set(isl_set_copy(max)),
			isl_union_map_copy(may_reads));

	isl_union_set* dataSetMaxOrig = isl_union_set_union(writeSetMax, readSetMax);
	isl_union_set* dataSetMax = SimplifyUnionSet(dataSetMaxOrig, config->programParameterVector->at(0));
	isl_union_pw_qpolynomial *dataSetMaxCard = isl_union_set_card(isl_union_set_copy(dataSetMax));

	isl_union_set* dataSetCommon = isl_union_set_intersect(isl_union_set_copy(dataSetMin),
		isl_union_set_copy(dataSetMax));
	isl_union_pw_qpolynomial *dataSetCommonCard = isl_union_set_card(isl_union_set_copy(dataSetCommon));

	isl_union_set* dataSetUnion = isl_union_set_union(isl_union_set_copy(dataSetMin),
		isl_union_set_copy(dataSetMax));
	isl_union_pw_qpolynomial *dataSetUnionCard = isl_union_set_card(isl_union_set_copy(dataSetUnion));

	// Compute the number of iterations
	long numParallelIters = ComputeNumberOfItersInParallelLoop(domain, pos,
		config->programParameterVector->at(0));
	long dataSetUnionCardInt = ExtractIntegerFromUnionPwQpolynomial(dataSetUnionCard);
	long dataSetCommonCardInt = ExtractIntegerFromUnionPwQpolynomial(dataSetCommonCard);

	if (dataSetUnionCardInt < 0) {
		dataSetUnionCardInt = 0;
	}

	if (dataSetCommonCardInt < 0) {
		dataSetCommonCardInt = 0;
	}

	if (numParallelIters <= 0) {
		cout << "numParallelIters is 0. Quitting" << endl;
		exit(1);
	}

	// We divide numParallelIters because the dataSetUnionCardInt contains the number
	// of data elements accessed in 2 iterations (NOT 1).
	long WSSize = (dataSetUnionCardInt - dataSetCommonCardInt) * numParallelIters / 2.0
		+ dataSetCommonCardInt;

	workingSetSize->size = WSSize;
	workingSetSize->dataSetUnionCardInt = dataSetUnionCardInt;
	workingSetSize->dataSetCommonCardInt = dataSetCommonCardInt;

	if (DEBUG) {
		cout << "minIterationSet: " << endl;
		PrintSet(min);

		cout << "maxIterationSet: " << endl;
		PrintSet(max);

		cout << "dataSetMin: " << endl;
		PrintUnionSet(dataSetMin);
		cout << "dataSetMinCard: " << endl;
		PrintUnionPwQpolynomial(dataSetMinCard);

		cout << "dataSetMax: " << endl;
		PrintUnionSet(dataSetMax);
		cout << "dataSetMaxCard: " << endl;
		PrintUnionPwQpolynomial(dataSetMaxCard);

		cout << "dataSetCommon: " << endl;
		PrintUnionSet(dataSetMax);
		cout << "dataSetCommonCard: " << endl;
		PrintUnionPwQpolynomial(dataSetCommonCard);

		cout << "dataSetUnion: " << endl;
		PrintUnionSet(dataSetUnion);
		cout << "dataSetUnionCard: " << endl;
		PrintUnionPwQpolynomial(dataSetUnionCard);

		cout << "Domain: " << endl;
		PrintBasicSet(domain);

		cout << "numParallelIters: " << numParallelIters << endl;
		cout << "dataSetUnionCardInt: " << dataSetUnionCardInt << endl;
		cout << "dataSetCommonCardInt: " << dataSetCommonCardInt << endl;
		cout << "WSSize: " << WSSize << endl;
	}

	isl_union_set_free(dataSetMinOrig);
	isl_union_set_free(dataSetMaxOrig);
	isl_union_set_free(dataSetMin);
	isl_union_set_free(dataSetMax);
	isl_union_set_free(dataSetCommon);
	isl_union_set_free(dataSetUnion);

	isl_union_pw_qpolynomial_free(dataSetMinCard);
	isl_union_pw_qpolynomial_free(dataSetMaxCard);
	isl_union_pw_qpolynomial_free(dataSetCommonCard);
	isl_union_pw_qpolynomial_free(dataSetUnionCard);
}


isl_basic_set* ProjectBSetToLexExtreme(isl_basic_set* sourceDomain,
	isl_set* sourceDomainLexExtreme, int pos)
{
	isl_basic_set* sourceDomainProjected = isl_basic_set_copy(sourceDomain);
	isl_basic_set_list *bsetList = isl_set_get_basic_set_list(sourceDomainLexExtreme);
	isl_size basetListSize = isl_basic_set_list_size(bsetList);

	if (DEBUG) {
		cout << "sourceDomain:" << endl;
		PrintBasicSet(sourceDomain);

		cout << "sourceDomainLexExtreme: " << endl;
		PrintSet(sourceDomainLexExtreme);
	}

	for (int k = 0; k < basetListSize; k++) {
		isl_basic_set* bset = isl_basic_set_list_get_at(bsetList, k);

		isl_constraint_list *constraints =
			isl_basic_set_get_constraint_list(bset);
		isl_size constraintSize = isl_constraint_list_size(constraints);

		for (int i = 0; i < constraintSize; i++) {
			isl_constraint* constraint = isl_constraint_list_get_at(constraints, i);

			for (int j = 0; j <= pos; j++) {
				if (isl_constraint_involves_dims(constraint, isl_dim_set, j, 1)) {
					if (DEBUG) {
						cout << "Constraint that involves the outer loop dim: " << endl;
						PrintConstraint(constraint);
					}

					sourceDomainProjected = isl_basic_set_add_constraint(
						sourceDomainProjected, isl_constraint_copy(constraint));
				}
			}

			isl_constraint_free(constraint);
		}

		isl_basic_set_free(bset);
	}


	if (DEBUG) {
		cout << "sourceDomainProjected: " << endl;
		PrintBasicSet(sourceDomainProjected);
	}

	return sourceDomainProjected;
}


int FindThePositionOfTheLoopVariable(isl_basic_set *bset,
	vector<string> *parallelLoops) {
	isl_size dimSize = isl_basic_set_dim(bset, isl_dim_set);

	for (int j = 0; j < dimSize; j++) {
		const char *name = isl_basic_set_get_dim_name(bset, isl_dim_set, j);

		vector<string>::iterator it;
		string nameStr(name);
		it = find(parallelLoops->begin(), parallelLoops->end(), nameStr);

		if (it != parallelLoops->end()) {
			return j;
		}
	}

	return -1;
}

isl_union_pw_qpolynomial* ComputeDataSetSize(isl_basic_set* sourceDomain,
	isl_set* source, isl_set* target, isl_union_map* may_reads,
	isl_union_map* may_writes) {

	/* itersUptoSourceExcludingSource := sourceDomain << source */
	isl_union_set* itersUptoSourceExcludingSource =
		isl_union_map_domain(
			isl_union_set_lex_lt_union_set(
				isl_union_set_from_basic_set(
					isl_basic_set_copy(sourceDomain)),
				isl_union_set_from_set(isl_set_copy(source))));

	/* itersUptoTargetIncludingTarget := sourceDomain <<= target */
	isl_union_set* itersUptoTargetIncludingTarget =
		isl_union_map_domain(
			isl_union_set_lex_le_union_set(
				isl_union_set_from_basic_set(
					isl_basic_set_copy(sourceDomain)),
				isl_union_set_from_set(isl_set_copy(target))));

	/* WS :=  itersUptoTargetIncludingTarget - itersUptoSourceExcludingSource */

	isl_union_set* WS =
		isl_union_set_subtract(
			itersUptoTargetIncludingTarget,
			itersUptoSourceExcludingSource);

	isl_union_pw_qpolynomial* WSSize = ComputeDataSetSize(
		WS, may_reads, may_writes);
	isl_union_set_free(WS);
	return WSSize;
}

isl_union_pw_qpolynomial* ComputeDataSetSize(isl_union_set* WS,
	isl_union_map *may_reads, isl_union_map *may_writes) {
	isl_union_set* readSet =
		isl_union_set_apply(isl_union_set_copy(WS),
			isl_union_map_copy(may_reads));
	isl_union_set* writeSet =
		isl_union_set_apply(isl_union_set_copy(WS),
			isl_union_map_copy(may_writes));

	if (DEBUG) {
		cout << "Working set: " << endl;
		PrintUnionSet(WS);
		cout << "readSet: " << endl;
		PrintUnionSet(readSet);
		cout << "writeSet: " << endl;
		PrintUnionSet(writeSet);
	}

	isl_union_set* dataSet = isl_union_set_union(readSet, writeSet);
	return isl_union_set_card(dataSet);
}

string ExtractFileName(string fileName) {
	string returnFileName;
	size_t found = fileName.find_last_of("/\\");
	if (found == string::npos) {
		returnFileName = fileName;
	}
	else {
		returnFileName = fileName.substr(found + 1);
	}

	return returnFileName;
}


bool compareByMinMaxSize(const MinMaxTuple* a, const MinMaxTuple* b) {
	if (a->min == b->min) {
		return (a->max < b->max);
	}
	else {
		return a->min < b->min;
	}
}

void SimplifyWorkingSetSizes(vector<WorkingSetSize*>* workingSetSizes,
	UserInput *userInput, Config *config, pet_scop *scop) {

	isl_union_pw_qpolynomial* totalDataSetSizeCard =
		ComputeTotalDataSetSize(scop);

	string suffix = "_ws_stats.csv";
	ofstream file;
	string configFileName = ExtractFileName(userInput->configFile);
	string fullFileName = userInput->inputFile + configFileName
		+ suffix;
	file.open(fullFileName);

	if (file.is_open()) {
		cout << "Writing to file " << fullFileName << endl;
	}
	else {
		cout << "Could not open the file: " << fullFileName << endl;
		exit(1);
	}

	ProgramCharacteristics* programChar = new ProgramCharacteristics;
	vector<MinMaxTuple*> *minMaxTupleVector = new vector<MinMaxTuple*>();
	programChar->datatypeSize = config->datatypeSize;

	if (userInput->minOutput == false) {
		file << "params,L1,L2,L3,Mem,L1DataSetSize,L2DataSetSize,L3DataSetSize,MemDataSetSize" << endl;
	}

	for (int j = 0; j < config->programParameterVector->size(); j++) {
		InitializeProgramCharacteristics(programChar);
		unordered_map<string, int>* paramValues =
			config->programParameterVector->at(j);

		string totalDataSetSizeCardString = SimplifyUnionPwQpolynomial(
			totalDataSetSizeCard, paramValues);
		long totalDataSetSize = -1;

		if (!totalDataSetSizeCardString.empty()) {
			totalDataSetSize = ConvertStringToLong(totalDataSetSizeCardString)
				* programChar->datatypeSize;
		}

		if (DEBUG) {
			cout << "totalDataSetSize: " << totalDataSetSize << endl;
		}

		if (userInput->minOutput == false) {
			file << GetParameterValuesString(paramValues) << ",";
		}

		bool doesParallelLoopExist = false;
		long dataSetUnionCardInt = -1;
		long dataSetCommonCardInt = -1;

		for (int i = 0; i < workingSetSizes->size(); i++) {
			if (workingSetSizes->at(i)->parallelLoop == true) {
				doesParallelLoopExist = true;
				dataSetUnionCardInt = max(dataSetUnionCardInt,
					workingSetSizes->at(i)->dataSetUnionCardInt
					* programChar->datatypeSize);
				dataSetCommonCardInt =
					max(dataSetCommonCardInt, workingSetSizes->at(i)->dataSetCommonCardInt
						* programChar->datatypeSize);
			}
		}

		if (DEBUG) {
			cout << "doesParallelLoopExist: " << doesParallelLoopExist << endl;
			cout << "dataSetUnionCardInt: " << dataSetUnionCardInt << endl;
			cout << "dataSetCommonCardInt: " << dataSetCommonCardInt << endl;
		}

		for (int i = 0; i < workingSetSizes->size(); i++) {
			long min = -1, max = -1;

			bool isParallelLoopEncountered = false;

			if (workingSetSizes->at(i)->parallelLoop == false) {
				isl_union_pw_qpolynomial* minSizePoly =
					workingSetSizes->at(i)->minSize;
				string minSize = SimplifyUnionPwQpolynomial(
					minSizePoly,
					paramValues);

				isl_union_pw_qpolynomial* maxSizePoly =
					workingSetSizes->at(i)->maxSize;
				string maxSize = SimplifyUnionPwQpolynomial(
					maxSizePoly,
					paramValues);

				if (!minSize.empty() && !maxSize.empty()) {
					min = ConvertStringToLong(minSize);
					max = ConvertStringToLong(maxSize);
				}

				if (DEBUG) {
					cout << "sequential_loop. min = max = " << min << endl;
				}
			}
			else {
				min = max = workingSetSizes->at(i)->size;
				isParallelLoopEncountered = true;
				if (DEBUG) {
					cout << "parallel_loop. min = max = " << min << endl;
				}
			}

			MinMaxTuple* minMaxTuple = AddToVectorIfUniqueDependence(
				minMaxTupleVector, min, max, isParallelLoopEncountered);

			if (DEBUG) {
				if (minMaxTuple) {
					cout << "working_set: " << endl;
					PrintWorkingSetSize(workingSetSizes->at(i));
					cout << "min = " << min << " max = " << max
						<< " isParallelLoopEncountered = " << isParallelLoopEncountered << endl;
				}
			}

			if (doesParallelLoopExist && minMaxTuple) {
				minMaxTuple->dataSetUnionCardInt = dataSetUnionCardInt;
				minMaxTuple->dataSetCommonCardInt = dataSetCommonCardInt;
			}
		}

		sort(minMaxTupleVector->begin(), minMaxTupleVector->end(),
			compareByMinMaxSize);

		bool isParallelLoopEncountered = false;
		for (int i = 0; i < minMaxTupleVector->size(); i++) {

			if (isParallelLoopEncountered == false) {
				isParallelLoopEncountered = minMaxTupleVector->at(i)->isParallelLoopEncountered;
			}

			UpdateProgramCharacteristics(minMaxTupleVector->at(i)->min,
				config->systemConfig, programChar);
			if (minMaxTupleVector->at(i)->max != minMaxTupleVector->at(i)->min) {
				UpdateProgramCharacteristics(minMaxTupleVector->at(i)->max,
					config->systemConfig, programChar);
			}

			if (DEBUG) {
				cout << "sorted:" << endl;
				cout << "doesParallelLoopExist: " << doesParallelLoopExist
					<< " isParallelLoopEncountered: " << isParallelLoopEncountered << endl;
				cout << "Min: " << minMaxTupleVector->at(i)->min << endl;
				cout << "Max: " << minMaxTupleVector->at(i)->max << endl;
			}

			UpdatePessimisticProgramCharacteristics(minMaxTupleVector->at(i)->min,
				minMaxTupleVector->at(i)->max,
				isParallelLoopEncountered,
				doesParallelLoopExist,
				config->systemConfig,
				programChar, userInput->numProcs, totalDataSetSize,
				dataSetUnionCardInt, dataSetCommonCardInt);
		}

		FreeMinMaxTupleVector(minMaxTupleVector);

			file << programChar->PessiL1DataSetSize << ","
			<< programChar->PessiL2DataSetSize << ","
			<< programChar->PessiL3DataSetSize << ","
			<< programChar->PessiMemDataSetSize
			<< endl;
	}

	file.close();

	delete minMaxTupleVector;
	delete programChar;
}

MinMaxTuple* AddToVectorIfUniqueDependence(vector<MinMaxTuple*> *minMaxTupleVector,
	long min, long max, bool isParallelLoopEncountered) {
	if (min != -1 && max != -1 && min != 0 && max != 0) {
		for (int i = 0; i < minMaxTupleVector->size(); i++) {
			if (minMaxTupleVector->at(i)->min == min &&
				minMaxTupleVector->at(i)->max == max) {
				return NULL;
			}
		}

		MinMaxTuple* minMaxTuple = new MinMaxTuple;
		minMaxTuple->min = min;
		minMaxTuple->max = max;
		minMaxTuple->isParallelLoopEncountered = isParallelLoopEncountered;
		minMaxTupleVector->push_back(minMaxTuple);
		return minMaxTuple;
	}
	else {
		return NULL;
	}
}

void FreeMinMaxTupleVector(vector<MinMaxTuple*> *minMaxTupleVector) {
	for (int i = 0; i < minMaxTupleVector->size(); i++) {
		delete minMaxTupleVector->at(i);
	}

	minMaxTupleVector->clear();
}

void SimplifyWorkingSetSizesInteractively(vector<WorkingSetSize*>* workingSetSizes,
	UserInput *userInput, Config *config) {
	string fileName = userInput->inputFile;

	if (DEBUG) {
		cout << "Number of working set sizes: " << workingSetSizes->size()
			<< endl;
	}

	string DIR = "stats";
	string suffix;

	if (config == NULL) {
		suffix = "_ws_stats.tsv";
	}
	else {
		suffix = ExtractFileName(userInput->configFile) + "_ws_stats.tsv";
	}

	ofstream file;
	string fullFileName = fileName + suffix;
	file.open(fullFileName);

	if (file.is_open()) {
		cout << "Writing to file " << fullFileName << endl;
	}
	else {
		cout << "Could not open the file: " << fullFileName << endl;
		exit(1);
	}

	SystemConfig* systemConfig;

	if (config == NULL) {
		systemConfig = (SystemConfig*)malloc(sizeof(SystemConfig));
	}
	else {
		systemConfig = config->systemConfig;
	}

	ProgramCharacteristics* programChar =
		(ProgramCharacteristics*)malloc(sizeof(ProgramCharacteristics));
	vector<MinMaxTuple*> *minMaxTupleVector = new vector<MinMaxTuple*>();

	if (config == NULL) {
		GetSystemAndProgramCharacteristics(systemConfig, programChar);
	}
	else {
		programChar->datatypeSize = config->datatypeSize;
	}

	char answer = 'Y';
	int count = 0;
	int size = 0;
	if (config) {
		size = config->programParameterVector->size();
	}

	while ((config == NULL && answer == 'Y') || (config && count < size)) {
		InitializeProgramCharacteristics(programChar);
		unordered_map<string, int>* paramValues;

		if (config == NULL) {
			paramValues = GetParameterValues(workingSetSizes);
		}
		else {
			paramValues = config->programParameterVector->at(count);
		}

		file << "Parameters: " << GetParameterValuesString(paramValues)
			<< endl;
		file << "dependence \t source \t min_target \t max_target \t min_WS_size \t max_WS_size\n";
		for (int i = 0; i < workingSetSizes->size(); i++) {
			isl_union_pw_qpolynomial* minSizePoly =
				workingSetSizes->at(i)->minSize;
			string minSize = SimplifyUnionPwQpolynomial(
				minSizePoly,
				paramValues);

			isl_union_pw_qpolynomial* maxSizePoly =
				workingSetSizes->at(i)->maxSize;
			string maxSize = SimplifyUnionPwQpolynomial(
				maxSizePoly,
				paramValues);

			if (!minSize.empty() && !maxSize.empty()) {
				long min = ConvertStringToLong(minSize);
				long max = ConvertStringToLong(maxSize);

				if (AddToVectorIfUniqueDependence(minMaxTupleVector, min, max, false)) {
					file << isl_basic_map_to_str(
						workingSetSizes->at(i)->dependence)
						<< "\t";
					file << isl_set_to_str(workingSetSizes->at(i)->source)
						<< "\t";
					file << isl_set_to_str(workingSetSizes->at(i)->minTarget)
						<< "\t";
					file << isl_set_to_str(workingSetSizes->at(i)->maxTarget)
						<< "\t";
					file << minSize << "\t";
					file << maxSize << endl;
				}
			}
		}

		sort(minMaxTupleVector->begin(), minMaxTupleVector->end(),
			compareByMinMaxSize);
		for (int i = 0; i < minMaxTupleVector->size(); i++) {
			UpdateProgramCharacteristics(minMaxTupleVector->at(i)->min,
				config->systemConfig, programChar);

			if (minMaxTupleVector->at(i)->min != minMaxTupleVector->at(i)->max) {
				UpdateProgramCharacteristics(minMaxTupleVector->at(i)->max,
					config->systemConfig, programChar);
			}

			UpdatePessimisticProgramCharacteristics(minMaxTupleVector->at(i)->min,
				minMaxTupleVector->at(i)->max,
				minMaxTupleVector->at(i)->isParallelLoopEncountered,
				false,
				config->systemConfig,
				programChar, 1, -1, -1, -1);
		}

		file << "#reuses in L1, L2, L3:"
			<< "\t" << programChar->L1Fit
			<< "\t" << programChar->L2Fit
			<< "\t" << programChar->L3Fit
			<< "\t" << programChar->MemFit
			<< "\t" << programChar->L1DataSetSize
			<< "\t" << programChar->L2DataSetSize
			<< "\t" << programChar->L3DataSetSize
			<< "\t" << programChar->MemDataSetSize
			<< "\t" << programChar->PessiL1DataSetSize
			<< "\t" << programChar->PessiL2DataSetSize
			<< "\t" << programChar->PessiL3DataSetSize
			<< "\t" << programChar->PessiMemDataSetSize

			<< endl;

		FreeMinMaxTupleVector(minMaxTupleVector);

		if (config == NULL) {
			paramValues->clear();
			delete paramValues;
			cout << "Would like to enter a new set of parameters? [Y/N]"
				<< endl;
			cin >> answer;
		}
		else {
			count++;
		}
	}
	file.close();

	if (config == NULL) {
		free(systemConfig);
	}

	delete minMaxTupleVector;
	free(programChar);
}

void GetSystemAndProgramCharacteristics(SystemConfig* systemConfig,
	ProgramCharacteristics* programChar) {
	cout << "Enter L1, L2, L3 cache sizes (in bytes): ";
	cin >> systemConfig->L1;
	cin >> systemConfig->L2;
	cin >> systemConfig->L3;

	cout << "Enter the datatype size (in bytes): ";
	cin >> programChar->datatypeSize;
}

void InitializeProgramCharacteristics(ProgramCharacteristics* programChar) {
	programChar->L1Fit = 0;
	programChar->L2Fit = 0;
	programChar->L3Fit = 0;
	programChar->MemFit = 0;
	programChar->L1DataSetSize = 0;
	programChar->L2DataSetSize = 0;
	programChar->L3DataSetSize = 0;
	programChar->MemDataSetSize = 0;
	programChar->PessiL1DataSetSize = 0;
	programChar->PessiL2DataSetSize = 0;
	programChar->PessiL3DataSetSize = 0;
	programChar->PessiMemDataSetSize = 0;
}

long ConvertStringToLong(string sizeStr) {
	try {
		return stol(sizeStr, nullptr, 10);
	}
	catch (const invalid_argument) {
		if (DEBUG) {
			cerr << "Invalid argument while updating" << endl;
		}

		return -1;
	}
}

void UpdateProgramCharacteristics(long size,
	SystemConfig* systemConfig,
	ProgramCharacteristics* programChar) {
	size = size * programChar->datatypeSize;
	if (size > 0) {
		if (size <= systemConfig->L1) {
			programChar->L1Fit += 1;
			programChar->L1DataSetSize += size;
		}
		else if (size <= systemConfig->L2) {
			programChar->L2Fit += 1;
			programChar->L2DataSetSize += size;
		}
		else if (size <= systemConfig->L3) {
			programChar->L3Fit += 1;
			programChar->L3DataSetSize += size;
		}
		else {
			programChar->MemFit += 1;
			programChar->MemDataSetSize += size;
		}
	}
}

void UpdatePessimisticProgramCharacteristics(long minSize, long maxSize,
	bool isParallelLoopEncountered,
	bool doesParallelLoopExist,
	SystemConfig* systemConfig,
	ProgramCharacteristics* programChar,
	int numProcs, long totalDataSetSize,
	long dataSetUnionCardInt, long dataSetCommonCardInt) {
	minSize = minSize * programChar->datatypeSize;
	maxSize = maxSize * programChar->datatypeSize;
	bool maxSizeSatisfied = false;
	bool minSizeSatisfied = false;


	if (systemConfig->L1Shared || systemConfig->L2Shared) {
		cout << "L1 and L2 caches are indicated as shared caches. Quitting." << endl;
		exit(1);
	}


	if (minSize > 0 && maxSize > 0) {
		if (!maxSizeSatisfied && ((maxSize + programChar->PessiL1DataSetSize) <= systemConfig->L1)) {
			programChar->PessiL1DataSetSize += maxSize;
			minSizeSatisfied = true;
			maxSizeSatisfied = true;
		}

		if (!minSizeSatisfied && ((minSize + programChar->PessiL1DataSetSize) <= systemConfig->L1)) {
			programChar->PessiL1DataSetSize += minSize;
			minSizeSatisfied = true;
		}

		if (!maxSizeSatisfied && ((maxSize + programChar->PessiL2DataSetSize) <= systemConfig->L2)) {
			programChar->PessiL2DataSetSize += maxSize;
			minSizeSatisfied = true;
			maxSizeSatisfied = true;
		}

		if (!minSizeSatisfied && ((minSize + programChar->PessiL2DataSetSize) <= systemConfig->L2)) {
			programChar->PessiL2DataSetSize += minSize;
			minSizeSatisfied = true;
		}

		if (!maxSizeSatisfied) {
			long effectiveMaxSize = maxSize;

			if (SCALEDATASETSIZEATL3 && doesParallelLoopExist && !isParallelLoopEncountered
				&& systemConfig->L3Shared) {

				// We halve dataSetUnionCardInt because dataSetUnionCardInt is the union of 
				// the datasets of two iterations. And therefore, halving it will give us the
				// working set of one iteration.
				// Here we are checking that effectiveMaxSize is greater than the working 
				// set of one iteration of the parallel loop. If it is so, then the working set
				// computation of the sequential loop should take into account the shared data
				// between parallel iterations. We should subtract the shared data set size
				// while multiplying the working set size of the sequential loop by the number
				// of processors
				if (effectiveMaxSize >= 0.5 * dataSetUnionCardInt) {
					effectiveMaxSize = (effectiveMaxSize - dataSetCommonCardInt) * numProcs
						+ dataSetCommonCardInt;
				}
				else {
					double commonFraction = ((double)dataSetCommonCardInt) / ((double)dataSetUnionCardInt);

					if (DEBUG) {
						cout << "commonFraction: " << commonFraction << endl;
					}

					effectiveMaxSize = (1.0 - commonFraction) * effectiveMaxSize * numProcs
						+ commonFraction * effectiveMaxSize;
				}
			}


			if ((effectiveMaxSize + programChar->PessiL3DataSetSize) <= systemConfig->L3) {
				programChar->PessiL3DataSetSize += effectiveMaxSize;
				minSizeSatisfied = true;
				maxSizeSatisfied = true;

				if (DEBUG) {
					cout << "L3, max:" << endl;
					cout << "dataSetUnionCardInt: " << dataSetUnionCardInt << endl;
					cout << "dataSetCommonCardInt: " << dataSetCommonCardInt << endl;
					cout << "effectiveMaxSize: " << effectiveMaxSize << endl;
				}
			}
		}

		if (!minSizeSatisfied) {
			long effectiveMinSize = minSize;

			if (SCALEDATASETSIZEATL3 && doesParallelLoopExist && !isParallelLoopEncountered
				&& systemConfig->L3Shared) {
				if (effectiveMinSize >= 0.5 * dataSetUnionCardInt) {
					effectiveMinSize = (effectiveMinSize - dataSetCommonCardInt) * numProcs
						+ dataSetCommonCardInt;
				}
				else {
					double commonFraction = ((double)dataSetCommonCardInt) / ((double)dataSetUnionCardInt);

					if (DEBUG) {
						cout << "commonFraction: " << commonFraction << endl;
					}

					effectiveMinSize = (1.0 - commonFraction) * effectiveMinSize * numProcs
						+ commonFraction * effectiveMinSize;
				}
			}

			if ((effectiveMinSize + programChar->PessiL3DataSetSize) <= systemConfig->L3) {
				programChar->PessiL3DataSetSize += effectiveMinSize;
				minSizeSatisfied = true;

				if (DEBUG) {
					cout << "L3, min:" << endl;
					cout << "dataSetUnionCardInt: " << dataSetUnionCardInt << endl;
					cout << "dataSetCommonCardInt: " << dataSetCommonCardInt << endl;
					cout << "effectiveMinSize: " << effectiveMinSize << endl;
				}
			}
		}

		if (!maxSizeSatisfied) {
			long effectiveMaxSize = maxSize;

			if (SCALEDATASETSIZEATL3 && doesParallelLoopExist && !isParallelLoopEncountered) {
				if (effectiveMaxSize >= 0.5 * dataSetUnionCardInt) {
					effectiveMaxSize = (effectiveMaxSize - dataSetCommonCardInt) * numProcs
						+ dataSetCommonCardInt;
				}
				else {
					double commonFraction = ((double)dataSetCommonCardInt) / ((double)dataSetUnionCardInt);

					if (DEBUG) {
						cout << "commonFraction: " << commonFraction << endl;
					}

					effectiveMaxSize = (1.0 - commonFraction) * effectiveMaxSize * numProcs
						+ commonFraction * effectiveMaxSize;
				}

				if (DEBUG) {
					cout << "Mem adjustment made:" << endl;
				}
			}

			programChar->PessiMemDataSetSize += effectiveMaxSize;
			minSizeSatisfied = true;
			maxSizeSatisfied = true;

			if (DEBUG) {
				cout << "PessiMemDataSetSize: " << endl;
				cout << "dataSetUnionCardInt: " << dataSetUnionCardInt << endl;
				cout << "dataSetCommonCardInt: " << dataSetCommonCardInt << endl;
				cout << "effectiveMaxSize: " << effectiveMaxSize << endl;
			}
		}

		totalDataSetSize = -1;
		if (totalDataSetSize != -1) {
			programChar->PessiL1DataSetSize = min(programChar->PessiL1DataSetSize, totalDataSetSize);
			programChar->PessiL2DataSetSize = min(programChar->PessiL2DataSetSize, totalDataSetSize);
			programChar->PessiL3DataSetSize = min(programChar->PessiL3DataSetSize, totalDataSetSize);
			programChar->PessiMemDataSetSize = min(programChar->PessiMemDataSetSize, totalDataSetSize);
		}
	}
}

unordered_map<string, int>* GetParameterValues(vector<WorkingSetSize*>* workingSetSizes) {
	unordered_map<string, int>* paramValues =
		new unordered_map<string, int>();

	if (workingSetSizes->size() > 0) {
		isl_union_pw_qpolynomial* repPoly =
			workingSetSizes->at(0)->minSize;
		isl_space* space = isl_union_pw_qpolynomial_get_space(repPoly);
		isl_size numParams =
			isl_space_dim(space, isl_dim_param);

		cout << "Enter values for the following parameters:" << endl;
		for (int j = 0; j < numParams; j++) {
			string name(isl_space_get_dim_name(
				space, isl_dim_param, (unsigned)j));
			cout << name << " ";
		}

		cout << endl;

		for (int j = 0; j < numParams; j++) {
			string name(isl_space_get_dim_name(
				space, isl_dim_param, (unsigned)j));
			int val;
			cin >> val;
			paramValues->insert({ {name, val} });
		}

		isl_space_free(space);
	}

	return paramValues;
}

string GetParameterValuesString(unordered_map<string, int>* paramValues) {
	string params = "";
	for (auto i : *paramValues) {
		params += i.first + " = " + to_string(i.second) + " ";
	}

	return params;
}

isl_set* ConstructContextEquatingParametersToConstants(
	isl_space* space, unordered_map<string, int>* paramValues) {
	isl_set* context = isl_set_universe(space);

	/*Add constraints now*/
	isl_constraint *c;
	isl_local_space *ls;
	space = isl_set_get_space(context);
	ls = isl_local_space_from_space(isl_space_copy(space));

	isl_size numParams =
		isl_set_dim(context, isl_dim_param);

	for (int j = 0; j < numParams; j++) {
		string name(isl_space_get_dim_name(
			space, isl_dim_param, (unsigned)j));
		int val = findInParamsMap(paramValues, name);

		c = isl_constraint_alloc_equality(
			isl_local_space_copy(ls));
		c = isl_constraint_set_coefficient_si(
			c, isl_dim_param, j, -1);
		c = isl_constraint_set_constant_si(c, val);
		context = isl_set_add_constraint(context, c);
	}

	isl_space_free(space);
	isl_local_space_free(ls);
	return context;
}

isl_union_map* SimplifyUnionMap(isl_union_map* map,
	unordered_map<string, int>* paramValues) {
	isl_set* context = ConstructContextEquatingParametersToConstants(
		isl_union_map_get_space(map), paramValues);
	return isl_union_map_gist_params(isl_union_map_copy(map), context);
}

isl_union_set* SimplifyUnionSet(isl_union_set* set,
	unordered_map<string, int>* paramValues) {
	isl_set* context = ConstructContextEquatingParametersToConstants(
		isl_union_set_get_space(set), paramValues);
	return isl_union_set_gist_params(isl_union_set_copy(set), context);
}

isl_basic_set* SimplifyBasicSet(isl_basic_set* bset,
	unordered_map<string, int>* paramValues) {
	isl_set* context = ConstructContextEquatingParametersToConstants(
		isl_basic_set_get_space(bset), paramValues);
	isl_set* setGist = isl_set_gist_params(
		isl_set_from_basic_set(isl_basic_set_copy(bset)), context);

	isl_basic_set_list *bsetList = isl_set_get_basic_set_list(setGist);
	isl_size bsetListSize = isl_basic_set_list_size(bsetList);
	if (bsetListSize != 1) {
		cout << "In SimplifyBasicSet(). The following set has more than one basic sets. Expected was 1. Quitting"
			<< endl;
		PrintSet(setGist);
		exit(1);
	}

	isl_basic_set* bsetGist = isl_basic_set_list_get_at(bsetList, 0);
	isl_set_free(setGist);
	isl_basic_set_list_free(bsetList);
	return bsetGist;
}

string SimplifyUnionPwQpolynomial(isl_union_pw_qpolynomial* size,
	unordered_map<string, int>* paramValues) {
	isl_set* context = ConstructContextEquatingParametersToConstants(
		isl_union_pw_qpolynomial_get_space(size), paramValues);
	isl_union_pw_qpolynomial* gistSize =
		isl_union_pw_qpolynomial_gist_params(
			isl_union_pw_qpolynomial_copy(size),
			context);

	long sizeInteger = ExtractIntegerFromUnionPwQpolynomial(gistSize);
	if (DEBUG) {
		cout << "gistSize: " << sizeInteger << endl;
	}

	string sizeString;
	if (sizeInteger != -1) {
		sizeString = to_string(sizeInteger);
	}

	return sizeString;
}

long ExtractIntegerFromUnionPwQpolynomial(
	isl_union_pw_qpolynomial* polynomial)
{
	/*The string representation typically will be of the following kind:
	[pad_w, ifwp, pad_h, ifhp, nIfm, ofwp, ofhp, nOfm, kw, kh, nImg, ofh, ofw] -> { 290 }

	This function extracts the integer -- in this case 290 from the string
	representation such as the above.*/

	long val = -1;
	if (DEBUG) {
		cout << "Converting: " << endl;
		PrintUnionPwQpolynomial(polynomial);
	}
	string poly(isl_union_pw_qpolynomial_to_str(polynomial));

	string openingBrace = "{";
	string closingBrace = "}";

	size_t begin = -1, end = -1;
	size_t found = poly.find(openingBrace);
	if (found != string::npos) {
		begin = found + 1;
	}

	found = poly.find(closingBrace);
	if (found != string::npos) {
		end = found - 1;
	}

	if (begin != -1 && end != -1 && begin < end) {
		string valStr = poly.substr(begin, end - begin + 1);
		try {
			val = stol(valStr, nullptr, 10);
			if (DEBUG) {
				cout << "Converted string " << valStr << " to integer "
					<< val << endl;
			}

			if (IGNORE_WS_SIZE_ONE && val == 1) {
				val = -1;
			}

			return val;
		}
		catch (const invalid_argument) {
			if (DEBUG) {
				cerr << "Invalid argument" << endl;
			}
		}
	}

	return val;
}

int findInParamsMap(unordered_map<string, int>* map, string key) {
	unordered_map<string, int>::const_iterator find =
		map->find(key);

	if (find == map->end()) {
		cout << "Parameter value not found for " << key << endl;
		exit(1);
		return -1;
	}
	else {
		return find->second;
	}
}

void PrintWorkingSetSize(WorkingSetSize* wss) {
	cout << "dependence: " << endl;
	PrintBasicMap(wss->dependence);

	cout << "source: " << endl;
	PrintSet(wss->source);

	cout << "target: " << endl;
	PrintSet(wss->target);

	cout << "MinSize: " << endl;
	PrintUnionPwQpolynomial(wss->minSize);

	cout << "MaxSize: " << endl;
	PrintUnionPwQpolynomial(wss->maxSize);

	cout << "parallelLoop: " << wss->parallelLoop << endl;
	cout << "size: " << wss->size << endl;
}

void PrintWorkingSetSizes(vector<WorkingSetSize*>* workingSetSizes) {
	cout << "Number of working set sizes: " << workingSetSizes->size()
		<< endl;
	for (int i = 0; i < workingSetSizes->size(); i++) {
		cout << "*********************************************" << endl;
		cout << "*********************************************" << endl;
		PrintWorkingSetSize(workingSetSizes->at(i));
		cout << "*********************************************" << endl;
		cout << "*********************************************" << endl;
		cout << endl;
	}
}


void FreeWorkingSetSizes(vector<WorkingSetSize*>* workingSetSizes) {
	for (int i = 0; i < workingSetSizes->size(); i++) {
		if (workingSetSizes->at(i) == NULL) {
			continue;
		}

		if (workingSetSizes->at(i)->dependence) {
			isl_basic_map_free(workingSetSizes->at(i)->dependence);
		}

		if (workingSetSizes->at(i)->source) {
			isl_set_free(workingSetSizes->at(i)->source);
		}

		if (workingSetSizes->at(i)->target) {
			isl_set_free(workingSetSizes->at(i)->target);
		}

		if (workingSetSizes->at(i)->minTarget) {
			isl_set_free(workingSetSizes->at(i)->minTarget);
		}

		if (workingSetSizes->at(i)->maxTarget) {
			isl_set_free(workingSetSizes->at(i)->maxTarget);
		}

		if (workingSetSizes->at(i)->minSize) {
			isl_union_pw_qpolynomial_free(
				workingSetSizes->at(i)->minSize);
		}

		if (workingSetSizes->at(i)->maxSize) {
			isl_union_pw_qpolynomial_free(
				workingSetSizes->at(i)->maxSize);
		}

		free(workingSetSizes->at(i));
	}

	delete workingSetSizes;
}

isl_union_map* IntersetMapWithSet(isl_union_map* map, isl_set* set) {
	return isl_union_map_intersect_range(isl_union_map_copy(map),
		isl_union_set_from_set(isl_set_copy(set)));
}

unordered_map<int, ArrayDataAccesses*>* ComputeDataDependences(
	UserInput *userInput,
	isl_ctx* ctx, pet_scop* scop,
	Config *config) {
	/*TODO: Print the array because of which the dependence is formed -
	use "full" dependence structrues*/
	isl_schedule* schedule = pet_scop_get_schedule(scop);
	isl_union_map *all_may_reads = pet_scop_get_may_reads(scop);
	isl_union_map *all_may_writes = pet_scop_get_may_writes(scop);

	unordered_map<int, ArrayDataAccesses*>* dependenceMap =
		new unordered_map<int, ArrayDataAccesses*>();

	if (config && config->programParameterVector->size() == 1) {
		/* Since there is only one set of parameters, we will simplify the read and write relations by
		converting the parametric relations
		to concrete ones*/
		if (DEBUG) {
			cout << "Simplifying_dependences in ComputeDataDependences" << endl;
		}

		all_may_reads = SimplifyUnionMap(all_may_reads, config->programParameterVector->at(0));
		all_may_writes = SimplifyUnionMap(all_may_writes, config->programParameterVector->at(0));
	}

	if (userInput->perarray) {
		if (DEBUG) {
			cout << "scop->n_array: " << scop->n_array << endl;
		}

		for (int i = 0; i < scop->n_array; i++) {
			if (scop->arrays[i]->extent) {
				if (DEBUG) {
					cout << "scop->arrays[i]->extent: " << endl;
					PrintSet(scop->arrays[i]->extent);
				}

				isl_union_map* may_reads = IntersetMapWithSet(all_may_reads,
					scop->arrays[i]->extent);

				isl_union_map* may_writes = IntersetMapWithSet(all_may_writes,
					scop->arrays[i]->extent);

				if (DEBUG) {
					cout << "Per-array reads: " << endl;
					PrintUnionMap(may_reads);
					cout << "Per-array writes: " << endl;
					PrintUnionMap(may_writes);
				}

				ArrayDataAccesses* dependences = ComputeAllDataDependences
				(may_reads, may_writes,
					schedule);

				dependenceMap->insert({ i, dependences });
			}
		}
	}
	else {
		ArrayDataAccesses* dependences = ComputeAllDataDependences(all_may_reads, all_may_writes,
			schedule);
		dependenceMap->insert({ 0, dependences });
	}

	isl_schedule_free(schedule);
	return dependenceMap;
}

ArrayDataAccesses* ComputeAllDataDependences(isl_union_map* may_reads,
	isl_union_map* may_writes, isl_schedule* schedule) {

	ArrayDataAccesses* arrayDataAccesses =
		new ArrayDataAccesses;
	arrayDataAccesses->may_reads = may_reads;
	arrayDataAccesses->may_writes = may_writes;

	// RAR
	isl_union_map* RAR = ComputeDataDependences(may_reads,
		may_reads, schedule);

	// RAW
	isl_union_map* RAW = ComputeDataDependences(may_writes,
		may_reads, schedule);

	// WAR
	isl_union_map* WAR = ComputeDataDependences(may_reads,
		may_writes, schedule);

	// WAW
	isl_union_map* WAW = ComputeDataDependences(may_writes,
		may_writes, schedule);

	isl_union_map* all_dependences = isl_union_map_union(RAR,
		isl_union_map_union(RAW, isl_union_map_union(WAR, WAW)));

	arrayDataAccesses->dependences = all_dependences;
	return arrayDataAccesses;
}

isl_union_map* ComputeDataDependences(isl_union_map *source,
	isl_union_map *target, isl_schedule* schedule)
{
	isl_union_access_info* access_info =
		isl_union_access_info_from_sink(isl_union_map_copy(target));
	access_info = isl_union_access_info_set_may_source(access_info,
		isl_union_map_copy(source));
	isl_union_access_info_set_schedule(access_info,
		isl_schedule_copy(schedule));

	isl_union_flow *deps =
		isl_union_access_info_compute_flow(access_info);

	if (DEBUG) {
		cout << "Dependences are:" << endl;
		if (deps == NULL) {
			cout << "No RAR dependences found" << endl;
		}
		else {
			cout << "Calling PrintUnionAccessInfo" << endl;
			PrintUnionFlow(deps);
		}
	}

	isl_union_map *may_dependences =
		isl_union_flow_get_may_dependence(deps);
	isl_union_flow_free(deps);
	return may_dependences;
}


pet_scop* ParseScop(isl_ctx* ctx, const char *fileName) {
	pet_options_set_autodetect(ctx, 0);
	pet_scop *scop = pet_scop_extract_from_C_source(ctx, fileName, NULL);
	if (DEBUG) {
		PrintScop(ctx, scop);
	}

	return scop;
}
