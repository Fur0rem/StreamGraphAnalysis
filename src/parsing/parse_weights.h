#ifndef PARSING_WEIGHTS_H
#define PARSING_WEIGHTS_H

#include "../units.h"
#include "../weighted/weight_function.h"
#include "cursor.h"
#include "events.h"

#ifdef SGA_INTERNAL

typedef struct LerpWeightPoint {
	SGA_Time time_instant;
	SGA_Weight associated_weight;
} LerpWeightPoint;

DeclareArrayList(LerpWeightPoint);
DeclareArrayList(LerpWeightPointArrayList);

typedef struct ParsedLerpWeight {
	SGA_NodeOrLink elem;
	LerpWeightPointArrayListArrayList associated_weights;
} ParsedLerpWeight;

DeclareArrayList(ParsedLerpWeight);

typedef struct ParsedLerpWeightFunction {
	ParsedLerpWeightArrayList weights_per_elem;
} ParsedLerpWeightFunction;

typedef struct ParsedUniversalWeightFunction {
	SGA_Weight weight;
} ParsedUniversalWeightFunction;

ParsedLerpWeightFunction SGA_parse_lerp_weight_function(SGA_ParsingCursor* cursor, bool is_node);

typedef struct {
	WeightFuncTag tag;
	union {
		ParsedUniversalWeightFunction universal;
		ParsedLerpWeightFunction lerp;
	} data;
} ParsedWeightFunction;

ParsedWeightFunction SGA_parse_weight_function(SGA_ParsingCursor* cursor, bool is_node);

#endif // SGA_INTERNAL

#endif // PARSING_WEIGHTS_H