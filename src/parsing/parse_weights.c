#include <stdio.h>
#define SGA_INTERNAL

#include "../generic_data_structures/arraylist.h"
#include "cursor.h"
#include "parse_weights.h"

DefineArrayList(LerpWeightPoint);
DefineArrayList(LerpWeightPointArrayList);
DefineArrayList(ParsedLerpWeight);

#define CHECK_FAIL(err_msg) SGA_ParsingResult_crash_if_fail(cursor, &result, (err_msg), print_lerp_format)

void print_lerp_format() {
	// TODO
}

LerpWeightPointArrayList lerp_parse_interval(SGA_ParsingCursor* cursor) {
	SGA_ParsingResult result;
	LerpWeightPointArrayList weights = LerpWeightPointArrayList_new();

	size_t last_instant_parsed = SGA_Time_max();
	while (SGA_ParsingCursor_current_char(cursor) != ']') {
		SGA_ParsingCursor_skip_whitespace(cursor);

		size_t instant_parsed;
		result = SGA_ParsingCursor_get_number_and_move(cursor, &instant_parsed, SGA_CODE_HERE);
		CHECK_FAIL("Failed to parse time instant!");

		// Check if too big to fit
		if (instant_parsed >= SGA_Time_max()) {
			result.success = false;
			result.src_ref = SGA_CODE_HERE;
			result.message = String_from_format("Time instant is too big! %zu > %zu (max)", instant_parsed, SGA_Time_max() - 1);
		}
		SGA_Time time_instant = (SGA_Time)instant_parsed;

		// Check if instants are in the wrong order
		if (last_instant_parsed != SGA_Time_max() && instant_parsed <= last_instant_parsed) {
			result.success = false;
			result.src_ref = SGA_CODE_HERE;
			result.message =
			    String_from_format("Time instants %zu and %zu are in the wrong order!", instant_parsed, last_instant_parsed);
		}

		SGA_ParsingCursor_skip_whitespace(cursor);

		result = SGA_ParsingCursor_expect_and_move(cursor, ':', SGA_CODE_HERE);
		CHECK_FAIL("Expected a ':' to associate a time instant with a weight!");
		SGA_ParsingCursor_skip_whitespace(cursor);

		double parsed_weight;
		result = SGA_ParsingCursor_get_real_and_move(cursor, &parsed_weight, SGA_CODE_HERE);
		CHECK_FAIL("Failed to parse weight!");

		// Separated by comma if not the last
		if (SGA_ParsingCursor_current_char(cursor) != ']') {
			result = SGA_ParsingCursor_expect_and_move(cursor, ',', SGA_CODE_HERE);
			CHECK_FAIL("Expected different weight points to be separated by commas!");
		}

		LerpWeightPoint new_point = {
		    .associated_weight = (SGA_Weight)parsed_weight,
		    .time_instant      = time_instant,
		};
		LerpWeightPointArrayList_push(&weights, new_point);
	}

	return weights;
}

SGA_NodeOrLink get_elem(SGA_ParsingCursor* cursor, bool is_node) {
	SGA_NodeOrLink elem;
	if (is_node) {
		size_t node_id;
		SGA_ParsingResult result = SGA_ParsingCursor_get_number_and_move(cursor, &node_id, SGA_CODE_HERE);
		CHECK_FAIL("Failed to parse Node ID!");
		if (node_id > SGA_NodeId_max()) {
			result.success = false;
			result.src_ref = SGA_CODE_HERE;
			result.message = String_from_format("Parsed Node ID is too big (%zu > %zu (max))!", node_id, SGA_NodeId_max());
		}
		CHECK_FAIL("Failed to parse Node ID");
		elem.node = node_id;
	}
	else {
		size_t node_id1;
		SGA_ParsingResult result = SGA_ParsingCursor_get_number_and_move(cursor, &node_id1, SGA_CODE_HERE);
		CHECK_FAIL("Failed to parse 1st Node ID!");
		if (node_id1 > SGA_NodeId_max()) {
			result.success = false;
			result.src_ref = SGA_CODE_HERE;
			result.message = String_from_format("Parsed Node ID is too big (%zu > %zu (max))!", node_id1, SGA_NodeId_max());
		}
		CHECK_FAIL("Failed to parse 1st Node ID");

		size_t node_id2;
		result = SGA_ParsingCursor_get_number_and_move(cursor, &node_id2, SGA_CODE_HERE);
		CHECK_FAIL("Failed to parse 2nd Node ID!");
		if (node_id2 > SGA_NodeId_max()) {
			result.success = false;
			result.src_ref = SGA_CODE_HERE;
			result.message = String_from_format("Parsed Node ID is too big (%zu > %zu (max))!", node_id2, SGA_NodeId_max());
		}
		CHECK_FAIL("Failed to parse 1st Node ID");
		elem.link.nodes[0] = node_id1;
		elem.link.nodes[1] = node_id2;
	}
	return elem;
}

ParsedLerpWeightFunction SGA_parse_lerp_weight_function(SGA_ParsingCursor* cursor, bool is_node) {
	ParsedLerpWeightFunction weight_fn = {
	    .weights_per_elem = ParsedLerpWeightArrayList_new(),
	};

	while (true) {
		// Stop if we arrived at a new header
		if (SGA_ParsingCursor_line_starts_with(cursor, "[")) {
			break;
		}

		if (SGA_ParsingCursor_line_is_empty(cursor)) {
			SGA_ParsingCursor_move_to_next_line(cursor, SGA_CODE_HERE);
			continue;
		}

		ParsedLerpWeightArrayList elem_weight = ParsedLerpWeightArrayList_new();

		SGA_NodeOrLink elem = get_elem(cursor, is_node);

		SGA_ParsingCursor_skip_whitespace(cursor);
		SGA_ParsingResult result = SGA_ParsingCursor_expect_and_move(cursor, '(', SGA_CODE_HERE);
		CHECK_FAIL("Expected a '(' to open weights!");

		ParsedLerpWeight elem_weights = {
		    .elem		= elem,
		    .associated_weights = LerpWeightPointArrayListArrayList_new(),
		};
		ParsedLerpWeightArrayList_push(&weight_fn.weights_per_elem, elem_weights);

		while (SGA_ParsingCursor_current_char(cursor) != ')') {
			SGA_ParsingCursor_skip_whitespace(cursor);
			result = SGA_ParsingCursor_expect_and_move(cursor, '[', SGA_CODE_HERE);

			ParsedLerpWeight* elem_weights			  = ParsedLerpWeightArrayList_last(&weight_fn.weights_per_elem);
			LerpWeightPointArrayList weights_current_interval = lerp_parse_interval(cursor);
			size_t nb_intervals				  = elem_weights->associated_weights.length;
			if (nb_intervals != 0) {
				LerpWeightPointArrayList previous_interval_weights =
				    elem_weights->associated_weights.array[nb_intervals - 1];
				SGA_Time last_time_of_previous_interval =
				    LerpWeightPointArrayList_last(&previous_interval_weights)->time_instant;

				SGA_Time first_time_of_current_interval =
				    LerpWeightPointArrayList_first(&weights_current_interval)->time_instant;

				if (last_time_of_previous_interval >= first_time_of_current_interval) {
					result.success = false;
					result.src_ref = SGA_CODE_HERE;
					result.message =
					    String_from_format("Time instants %zu from previous internal and %zu are in the wrong order!",
							       last_time_of_previous_interval,
							       first_time_of_current_interval);
				}
			}

			LerpWeightPointArrayListArrayList_push(&elem_weights->associated_weights, weights_current_interval);

			cursor->cursor++;
		}

		result = SGA_ParsingCursor_move_to_next_line(cursor, SGA_CODE_HERE);
		CHECK_FAIL("Reached end of file prematurely!");
	}
	return weight_fn;
}

#undef CHECK_FAIL
#define CHECK_FAIL(err_msg) SGA_ParsingResult_crash_if_fail(cursor, &result, (err_msg), print_universal_format)

void print_universal_format() {
	fprintf(stderr, "Expected: \"weight=x\", with x a real number\n");
}

ParsedUniversalWeightFunction SGA_parse_universal_weight_function(SGA_ParsingCursor* cursor) {
	SGA_ParsingResult result = SGA_ParsingCursor_expect_sequence_and_move(cursor, "weight=", SGA_CODE_HERE);
	CHECK_FAIL("Failed to parse universal weight!");

	double parsed_weight;
	result = SGA_ParsingCursor_get_real_and_move(cursor, &parsed_weight, SGA_CODE_HERE);
	CHECK_FAIL("Failed to parse the weight!");

	return (ParsedUniversalWeightFunction){
	    .weight = (SGA_Weight)parsed_weight,
	};
}

void print_weight_type() {
	fprintf(stderr, "Expected \"type=T\"\n");
	fprintf(stderr, "Available types:\n");
	fprintf(stderr, "\t - universal\n");
	fprintf(stderr, "\t - lerp\n");
}

#undef CHECK_FAIL
#define CHECK_FAIL(err_msg) SGA_ParsingResult_crash_if_fail(cursor, &result, (err_msg), print_weight_type)

ParsedWeightFunction SGA_parse_weight_function(SGA_ParsingCursor* cursor, bool is_node) {
	ParsedWeightFunction parsed_fn;
	SGA_ParsingResult result = SGA_ParsingCursor_expect_sequence_and_move(cursor, "type=", SGA_CODE_HERE);
	CHECK_FAIL("Failed to parse weight function type");

	if (SGA_ParsingCursor_line_starts_with(cursor, "universal")) {
		parsed_fn.tag = CONST_UNIVERSALLY;
		result	      = SGA_ParsingCursor_move_to_next_line(cursor, SGA_CODE_HERE);
		CHECK_FAIL("Reached end of file prematurely!");
		parsed_fn.data.universal = SGA_parse_universal_weight_function(cursor);
	}
	else if (SGA_ParsingCursor_line_starts_with(cursor, "lerp")) {
		parsed_fn.tag = LERP;
		result	      = SGA_ParsingCursor_move_to_next_line(cursor, SGA_CODE_HERE);
		CHECK_FAIL("Reached end of file prematurely!");
		parsed_fn.data.lerp = SGA_parse_lerp_weight_function(cursor, is_node);
	}

	return parsed_fn;
}