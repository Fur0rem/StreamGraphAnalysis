#ifndef PARSING_WEIGHTS_H
#define PARSING_WEIGHTS_H

#include "../units.h"
#include "cursor.h"
#include "events.h"

typedef enum WeightFuncTag : uint8_t {
	LERP,		   ///< A linearly interpolated weight function.
	CONST_UNIVERSALLY, ///< A constant weight function that applies universally to all elements at all times.
} WeightFuncTag;

#ifdef SGA_INTERNAL

typedef struct LerpWeightPoint {
	SGA_Time time_instant;
	SGA_Weight associated_weight;
} LerpWeightPoint;

DeclareDestroy(LerpWeightPoint);
DeclareArrayList(LerpWeightPoint);
DeclareArrayListDeriveRemove(LerpWeightPoint);
DeclareArrayList(LerpWeightPointArrayList);
DeclareArrayListDeriveRemove(LerpWeightPointArrayList);

typedef struct ParsedLerpWeight {
	SGA_NodeOrLink elem;
	LerpWeightPointArrayListArrayList associated_weights;
} ParsedLerpWeight;

DeclareDestroy(ParsedLerpWeight);
DeclareArrayList(ParsedLerpWeight);
DeclareArrayListDeriveRemove(ParsedLerpWeight);

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

void ParsedWeightFunction_destroy(ParsedWeightFunction self);

#endif // SGA_INTERNAL

#endif // PARSING_WEIGHTS_H