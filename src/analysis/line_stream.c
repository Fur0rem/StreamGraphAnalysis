#define SGA_INTERNAL

#include "line_stream.h"
#include "../stream.h"
#include "../stream_functions.h"
#include "../stream_graph/key_instants_table.h"
#include "../streams.h"

SGA_StreamGraph SGA_line_stream_of(const SGA_Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);

	// All the nodes of the line stream are the links of the original stream
	size_t line_stream_nb_nodes = SGA_Stream_distinct_cardinal_of_link_set(stream);

	// Same lifespan and time scale as the original stream
	SGA_Interval lifespan = SGA_Stream_lifespan(stream);
	size_t time_scale     = SGA_Stream_time_scale(stream);

	// The presence of each node in the line stream is the presence of the corresponding link in the original stream
	SGA_IntervalArrayList* node_presences = MALLOC(sizeof(SGA_IntervalArrayList) * line_stream_nb_nodes);
	SGA_LinksIterator links		      = fns.links_set(stream->stream_data);
	int idx				      = 0;
	SGA_FOR_EACH_LINK(link_id, links) {
		SGA_TimesIterator times = fns.times_link_present(stream->stream_data, link_id);
		node_presences[idx]	= SGA_collect_times(times);
		idx++;
	}

	// Each link represents when two links have a node in common.
	SGA_LinkArrayList links_array = SGA_LinkArrayList_new();

	// List of all the neighbours of each node in the line stream.
	SGA_LinkIdArrayList* neighbours_of_nodes = MALLOC(sizeof(SGA_LinkIdArrayList) * line_stream_nb_nodes);
	for (size_t i = 0; i < line_stream_nb_nodes; i++) {
		neighbours_of_nodes[i] = SGA_LinkIdArrayList_new();
	}

	// For each node present in the stream, we look at its neighbours and create a link between each pair of neighbours
	SGA_NodesIterator nodes = fns.nodes_set(stream->stream_data);
	SGA_FOR_EACH_NODE(node_id, nodes) {
		SGA_LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, node_id);
		SGA_FOR_EACH_LINK(link_id_a, neighbours) {
			SGA_LinksIterator neighbours_b = fns.neighbours_of_node(stream->stream_data, node_id);
			SGA_FOR_EACH_LINK(link_id_b, neighbours_b) {
				// Avoids double counting links
				if (link_id_a >= link_id_b) {
					continue;
				}

				// Compute the intersection of the presence of the two links
				SGA_IntervalArrayList intersection =
				    SGA_IntervalArrayList_intersection(&node_presences[link_id_a], &node_presences[link_id_b]);
				// Don't create a link if the intersection is empty
				if (intersection.length == 0) {
					SGA_IntervalArrayList_destroy(intersection);
					continue;
				}

				// Create the link between the two links
				SGA_Link link = {
				    .nodes    = {link_id_a, link_id_b},
				    .presence = SGA_IntervalsSet_from_interval_arraylist(intersection),
				};
				SGA_LinkArrayList_push(&links_array, link);

				// Add the link to the neighbours of the nodes
				SGA_LinkIdArrayList_push(&neighbours_of_nodes[link_id_a], link_id_b);
				SGA_LinkIdArrayList_push(&neighbours_of_nodes[link_id_b], link_id_a);
			}
		}
	}

	// Collect all key instants of the original stream
	SGA_TimesIterator key_instants	     = fns.key_instants(stream->stream_data);
	SGA_TimeArrayList key_instants_array = SGA_TimeArrayList_new();
	SGA_FOR_EACH_TIME(instant, key_instants) {
		SGA_TimeArrayList_push(&key_instants_array, instant.start);
	}

	// If the last instant isn't the end of the stream, we add it
	SGA_Time last_key_instant = key_instants_array.array[key_instants_array.length - 1];
	if (last_key_instant != lifespan.end) {
		SGA_TimeArrayList_push(&key_instants_array, lifespan.end);
	}

	KeyInstantsTable kmt = KeyInstantsTable_from_list(&key_instants_array);

	// If the last key instant wasn't the end of the stream, we remove it from the table
	if (last_key_instant != lifespan.end) {
		size_t slice_idx = KeyInstantsTable_in_which_slice_is(&kmt, lifespan.end);
		ASSERT(slice_idx != SIZE_MAX);
		kmt.slices[slice_idx].nb_instants--;
		// If it was the only one in the slice, we free the instants array
		if (kmt.slices[slice_idx].nb_instants == 0) {
			free(kmt.slices[slice_idx].instants);
		}
	}

	// Create the node set
	NodesSet nodes_set = {
	    .nb_nodes = line_stream_nb_nodes,
	    .nodes    = MALLOC(sizeof(SGA_Node) * line_stream_nb_nodes),
	};
	for (size_t i = 0; i < line_stream_nb_nodes; i++) {
		// Shrink the array to the actual size
		neighbours_of_nodes[i].array = realloc(neighbours_of_nodes[i].array, neighbours_of_nodes[i].length * sizeof(SGA_LinkId));

		// Create the node from the info in the arrays
		nodes_set.nodes[i] = (SGA_Node){
		    .presence	   = SGA_IntervalsSet_from_interval_arraylist(node_presences[i]),
		    .neighbours	   = neighbours_of_nodes[i].array,
		    .nb_neighbours = neighbours_of_nodes[i].length,
		};
	}

	// Create the links set
	LinksSet links_set = {
	    .nb_links = links_array.length,
	    .links    = links_array.array,
	};

	// Create the stream graph
	SGA_StreamGraph line_stream = SGA_StreamGraph_from(lifespan, time_scale, nodes_set, links_set, kmt, key_instants_array.length);

	// Free the allocated memory
	SGA_TimeArrayList_destroy(key_instants_array);
	free(node_presences);
	free(neighbours_of_nodes);

	return line_stream;
}
