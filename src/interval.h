#ifndef INTERVAL_H
#define INTERVAL_H

#include "generic_data_structures/arraylist.h"
#include "units.h"
#include "utils.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
	SGA_Time start;
	SGA_Time end;
} SGA_Interval;

typedef struct {
	size_t nb_intervals;
	SGA_Interval* intervals;
} SGA_IntervalsSet;

// TODO: maybe change the name of contains ?
// TODO: maybe use pointers for those instead?
bool SGA_Interval_contains(SGA_Interval interval, SGA_Time time);
bool SGA_Interval_contains_interval(SGA_Interval container, SGA_Interval contained);
bool SGA_Interval_overlaps_interval(SGA_Interval left, SGA_Interval right);

size_t SGA_Interval_duration(SGA_Interval interval);
SGA_Interval SGA_Interval_from(SGA_Time start, SGA_Time end);
SGA_Interval SGA_Interval_intersection(SGA_Interval left, SGA_Interval right);
SGA_Interval SGA_Interval_minus(SGA_Interval left, SGA_Interval right);

DeclareToString(SGA_Interval);
DeclareEquals(SGA_Interval);

DeclareArrayList(SGA_Interval);
DeclareArrayListDeriveRemove(SGA_Interval);
DeclareArrayListDeriveEquals(SGA_Interval);
DeclareArrayListDeriveToString(SGA_Interval);

DeclareToString(SGA_IntervalsSet);

// int SGA_Interval_starts_before(const void* a, const void* b);

size_t SGA_IntervalsSet_size(SGA_IntervalsSet intervals_set);
SGA_IntervalsSet SGA_IntervalsSet_alloc(size_t nb_intervals);
void SGA_IntervalsSet_merge(SGA_IntervalsSet* intervals_set);
SGA_IntervalsSet SGA_IntervalsSet_intersection(SGA_IntervalsSet left, SGA_IntervalsSet right);
SGA_IntervalArrayList SGA_IntervalArrayList_intersection(SGA_IntervalArrayList* left, SGA_IntervalArrayList* right);
SGA_IntervalsSet SGA_IntervalsSet_intersection_with_single(SGA_IntervalsSet set, SGA_Interval interval);
void SGA_IntervalsSet_self_intersection_with_single(SGA_IntervalsSet* intervals_set, SGA_Interval interval);
SGA_IntervalsSet SGA_IntervalsSet_union(SGA_IntervalsSet left, SGA_IntervalsSet right);
void SGA_IntervalsSet_destroy(SGA_IntervalsSet intervals_set);
SGA_Interval SGA_IntervalsSet_last(SGA_IntervalsSet* intervals_set);
bool SGA_IntervalsSet_contains(SGA_IntervalsSet intervals_set, SGA_Time time);
bool SGA_IntervalsSet_contains_sorted(SGA_IntervalsSet intervals_set, SGA_Time time);

void SGA_IntervalsSet_add_at(SGA_IntervalsSet* intervals_set, SGA_Interval interval, size_t index);

SGA_IntervalsSet SGA_IntervalsSet_from_arraylist(SGA_IntervalArrayList intervals);
SGA_IntervalArrayList SGA_IntervalArrayList_from_intervals_set(SGA_IntervalsSet intervals_set);

void SGA_IntervalsSet_sort(SGA_IntervalsSet* intervals_set);

SGA_Interval SGA_Interval_empty();
bool SGA_Interval_is_empty(SGA_Interval interval);

typedef struct {
	enum {
		OK,
		EMPTY_INTERVALSET,
		NO_MATCHING_OFFSET,
	} result;
	size_t offset;
} SGA_Offset;

SGA_Offset SGA_IntervalArrayList_offset_of(const SGA_IntervalArrayList* self, const SGA_IntervalArrayList* other);
SGA_Offset SGA_Interval_offset_of(const SGA_Interval* self, const SGA_Interval* other);

bool SGA_Offset_is_ok(SGA_Offset offset);

bool SGA_Offset_is_empty(SGA_Offset offset);

bool SGA_Offset_is_not_matching(SGA_Offset offset);

size_t SGA_Offset_unwrap(SGA_Offset offset);

/**
 * @brief Builder for SGA_IntervalsSet from appearance and disappearance events.
 */
typedef struct SGA_IntervalsSetBuilder {
	SGA_Time last_time;		      ///< The last time added.
	size_t nb_pushed;		      ///< The number of events pushed.
	SGA_IntervalArrayList intervals_list; ///< The intervals being built.
} SGA_IntervalsSetBuilder;

DeclareDestroy(SGA_IntervalsSetBuilder);

/**
 * @brief All the possible errors when building an intervals set.
 */
typedef struct SGA_IntervalsSetBuilderError {
	enum : uint8_t {
		None,			 ///< No error.
		TwoAppearancesInARow,	 ///< Two appearances in a row without a disappearance.
		TwoDisappearancesInARow, ///< Two disappearances in a row without an appearance.
		NotSortedTimes,		 ///< The times are not sorted.
		UnevenNumberOfEvents,	 ///< There is an uneven number of events (presence not closed).
	} type;				 ///< The type of error.
	union {
		struct {
			SGA_Time appearance;	  ///< The time of the appearance event.
			SGA_Time last_appearance; ///< The time of the last appearance event.
		} two_appearances_in_a_row;	  ///< Data for two appearances in a row error.
		struct {
			SGA_Time last_disappearance; ///< The time of the last disappearance event.
			SGA_Time disappearance;	     ///< The time of the disappearance event.
		} two_disappearances_in_a_row;	     ///< Data for two disappearances in a row error.
		struct {
			SGA_Time previous_time; ///< The previous time.
			SGA_Time current_time;	///< The current time.
		} not_sorted_times;		///< Data for not sorted times error.
		struct {
			SGA_Time last_appearance; ///< The time of the last appearance event.
			size_t nb_events;	  ///< The number of events.
		} uneven_number_of_events;	  ///< Data for uneven number of events error.
	} details;				  ///< Additional details about the error.
} SGA_IntervalsSetBuilderError;

SGA_IntervalsSetBuilder SGA_IntervalsSetBuilder_new();

String SGA_IntervalsSetBuilderError_to_string(SGA_IntervalsSetBuilderError* error);

DeclareArrayList(SGA_IntervalsSetBuilder);
DeclareArrayListDeriveRemove(SGA_IntervalsSetBuilder);

/**
 * @brief Add an appearance event to the builder.
 * @param[in,out] builder The builder.
 * @param[in] time The time of the appearance event.
 * @return An error if any occurred.
 */
SGA_IntervalsSetBuilderError SGA_IntervalsSetBuilder_add_appearance(SGA_IntervalsSetBuilder* builder, SGA_Time time);

/**
 * @brief Add a disappearance event to the builder.
 * @param[in,out] builder The builder.
 * @param[in] time The time of the disappearance event.
 * @return An error if any occurred.
 */
SGA_IntervalsSetBuilderError SGA_IntervalsSetBuilder_add_disappearance(SGA_IntervalsSetBuilder* builder, SGA_Time time);

SGA_IntervalsSetBuilderError SGA_IntervalsSetBuilder_build(SGA_IntervalsSetBuilder* builder, SGA_IntervalsSet* out_intervals_set);

#endif // INTERVAL_H