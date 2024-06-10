#include "bit_array.h"
#include "units.h"
#include <stddef.h>
#include <stdint.h>

typedef uint8_t RelativeMoment;

typedef struct {
	size_t nb_moments;
	RelativeMoment* moments;
} MomentsSlice;

typedef struct {
	size_t nb_slices;
	MomentsSlice* slices;
} MomentsTable;

typedef struct {
	TimeId start;
	TimeId end;
} Interval;

typedef struct {
	size_t nb_intervals;
	Interval* present_at;
	size_t nb_neighbours;
	EdgeId* neighbours;
} TemporalNode;

typedef struct {
	size_t nb_nodes;
	TemporalNode* nodes;
} TemporalNodesSet;

typedef struct {
	size_t nb_intervals;
	Interval* present_at;
	NodeId nodes[2];
} Link;

typedef struct {
	size_t nb_links;
	Link* links;
} LinksSet;

/*typedef struct {
	size_t* data; // The first element contains the number of nodes in the event, then the number of
				  // edges, then the nodes, then the edges
} Event;*/

typedef size_t* Event;

typedef struct {
	TimeId disappearance_index;
	SGA_BitArray presence_mask;
	size_t nb_events;
	Event* events; // The first element contains the number of nodes in the array, then the n
} EventsTable;

typedef struct {
	const char** names;
} NodeNameTable;

typedef struct {
	MomentsTable moments;
	TemporalNodesSet nodes;
	LinksSet links;
	EventsTable events;
	NodeNameTable node_names;
} StreamGraph;

StreamGraph SGA_StreamGraph_from_string(const char* str);
StreamGraph SGA_StreamGraph_from_file(const char* filename);
char* SGA_StreamGraph_to_string(StreamGraph* sg);