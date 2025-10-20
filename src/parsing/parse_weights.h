#ifndef PARSING_WEIGHTS_H
#define PARSING_WEIGHTS_H

#include "../units.h"
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

ParsedLerpWeightFunction SGA_parse_lerp_weight_function(SGA_ParsingCursor* cursor, bool is_node);

#endif // SGA_INTERNAL

#endif // PARSING_WEIGHTS_H