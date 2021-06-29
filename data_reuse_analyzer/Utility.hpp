#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <pet.h>
#include <isl/union_set.h>
#include <isl/flow.h>
#include <barvinok/isl.h>
#include <isl/space.h>
#include <string>
#include <algorithm>
#include <vector>
#include <isl/constraint.h>
using namespace std;

void PrintScop(isl_ctx* ctx, struct pet_scop *scop);
void PrintExpressions(isl_printer *printer, pet_expr *expr);
void PrintUnionFlow(isl_union_flow* flow);
void PrintUnionMap(isl_union_map* map);
void PrintMap(isl_map* map);
void PrintBasicMap(isl_basic_map* map);
void PrintUnionSet(isl_union_set* set);
void PrintSet(isl_set* set);
void PrintUnionPwQpolynomial(isl_union_pw_qpolynomial* poly);
void PrintSpace(isl_space* space);
void PrintScopOriginal(isl_ctx *ctx, pet_scop* scop);
void PrintBasicSet(isl_basic_set* set);
void PrintConstraint(isl_constraint* constraint);
void PrintMat(isl_mat* mat);
void CollectArrayNames(isl_union_map *may_reads, isl_union_map *may_writes, vector<string>* arrayNames);
void CollectArrayNamesFromUnionMap(isl_union_map* orig_map, vector<string>* arrayNames);
#endif