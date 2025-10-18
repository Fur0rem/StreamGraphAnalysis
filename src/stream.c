#define SGA_INTERNAL
#include "units.h"

#include "parsing/parse_stream_graph.h"

#include "stream.h"
#include "stream_graph/events_table.h"
#include "stream_graph/key_instants_table.h"
#include "stream_graph/links_set.h"
#include "stream_graph/nodes_set.h"

void init_events_table(SGA_StreamGraph* sg, size_t nb_events) {
	// Accumulator for the node events
	BitArray node_presence_mask  = BitArray_n_ones(nb_events);
	size_tArrayList* node_events = MALLOC(sizeof(size_tArrayList) * nb_events);
	size_tHashset* node_deleted  = MALLOC(sizeof(size_tHashset) * nb_events);
	for (size_t i = 0; i < nb_events; i++) {
		node_events[i]	= size_tArrayList_new();
		node_deleted[i] = size_tHashset_new();
	}

	// For each node, we add the creation and deletion times to the events table
	for (size_t i = 0; i < sg->nodes.nb_nodes; i++) {
		SGA_Node* node = &sg->nodes.nodes[i];
		for (size_t j = 0; j < node->presence.nb_intervals; j++) {
			SGA_Interval interval = node->presence.intervals[j];
			// Find where the start and end of the interval are in the key instants table
			size_t start = KeyInstantsTable_find_time_index_if_pushed(&sg->key_instants, interval.start);
			size_t end   = KeyInstantsTable_find_time_index_if_pushed(&sg->key_instants, interval.end);

			// printf("Node %zu interval %zu: start %zu end %zu mapped to events %zu - %zu\n",
			//        i,
			//        j,
			//        interval.start,
			//        interval.end,
			//        start,
			//        end);
			// Invalidate the bit of the presence mask when the node disappears
			BitArray_set_zero(node_presence_mask, end);

			// Push the creation event at the start time
			size_tArrayList_push(&node_events[start], i);

			// Add the deleted node to the set of nodes deleted at that time
			size_tHashset_insert(&node_deleted[end], i);
		}
	}

	// Re-copy present nodes in disappearance events
	for (size_t i = 0; i < nb_events; i++) {
		bool is_disappearance = BitArray_is_zero(node_presence_mask, i);
		// If the event is not a disappearance, we don't want to re-copy the present nodes
		if (!is_disappearance) {
			continue;
		}

		// Go through all the previous events to find the nodes that were present at this time
		for (int j = i - 1; j >= 0; j--) {
			// Recopy all the nodes that are present at this time
			for (size_t k = 0; k < node_events[j].length; k++) {
				size_t node_to_push = node_events[j].array[k];
				// If the node is not deleted at this time, we can recopy it
				if (!size_tHashset_contains(node_deleted[i], node_to_push)) {
					size_tArrayList_push(&node_events[i], node_to_push);
				}
			}
			bool is_also_disappearance = BitArray_is_zero(node_presence_mask, j);

			// If the previous event is also a disappearance, we can stop here because it also recopied the present nodes
			if (is_also_disappearance) {
				break;
			}
		}
	}

	// Do the same for links
	// Accumulator for the link events
	BitArray link_presence_mask  = BitArray_n_ones(nb_events);
	size_tArrayList* link_events = MALLOC(sizeof(size_tArrayList) * nb_events);
	size_tHashset* link_deleted  = MALLOC(sizeof(size_tHashset) * nb_events);
	for (size_t i = 0; i < nb_events; i++) {
		link_events[i]	= size_tArrayList_new();
		link_deleted[i] = size_tHashset_new();
	}

	// For each link, we add the creation and deletion times to the events table
	for (size_t i = 0; i < sg->links.nb_links; i++) {
		SGA_Link* link = &sg->links.links[i];
		for (size_t j = 0; j < link->presence.nb_intervals; j++) {
			SGA_Interval interval = link->presence.intervals[j];
			// Find where the start and end of the interval are in the key instants table
			size_t start = KeyInstantsTable_find_time_index_if_pushed(&sg->key_instants, interval.start);
			size_t end   = KeyInstantsTable_find_time_index_if_pushed(&sg->key_instants, interval.end);

			// Invalidate the bit of the presence mask when the link disappears
			BitArray_set_zero(link_presence_mask, end);

			// Push the creation event at the start time
			size_tArrayList_push(&link_events[start], i);

			// Add the deleted link to the set of links deleted at that time
			size_tHashset_insert(&link_deleted[end], i);
		}
	}

	// Re-copy present links in disappearance events
	for (size_t i = 0; i < nb_events; i++) {
		bool is_disappearance = BitArray_is_zero(link_presence_mask, i);
		// If the event is not a disappearance, we don't want to re-copy the present links
		if (!is_disappearance) {
			continue;
		}

		// Go through all the previous events to find the links that were present at this time
		for (int j = i - 1; j >= 0; j--) {
			// Recopy all the links that are present at this time
			for (size_t k = 0; k < link_events[j].length; k++) {
				size_t link_to_push = link_events[j].array[k];
				// If the link is not deleted at this time, we can recopy it
				if (!size_tHashset_contains(link_deleted[i], link_to_push)) {
					size_tArrayList_push(&link_events[i], link_to_push);
				}
			}
			bool is_also_disappearance = BitArray_is_zero(link_presence_mask, j);

			// If the previous event is also a disappearance, we can stop here because it also recopied the present links
			if (is_also_disappearance) {
				break;
			}
		}
	}

	sg->events = EventsTable_create(node_events, link_events, node_presence_mask, link_presence_mask, nb_events);

	// Free the accumulators
	for (size_t i = 0; i < nb_events; i++) {
		size_tArrayList_destroy(node_events[i]);
		size_tHashset_destroy(node_deleted[i]);
		size_tArrayList_destroy(link_events[i]);
		size_tHashset_destroy(link_deleted[i]);
	}
	free(node_events);
	free(node_deleted);
	free(link_events);
	free(link_deleted);
}

SGA_StreamGraph SGA_stream_graph_from_parsed(const SGA_ParsedStreamGraph* parsed) {
	SGA_StreamGraph sg;

	sg.lifespan   = parsed->general_header.lifespan;
	sg.time_scale = parsed->general_header.time_scale;

	sg.nodes = NodesSet_alloc(parsed->node_presences.length);
	for (size_t i = 0; i < parsed->node_presences.length; i++) {
		SGA_IntervalsSetBuilderError err =
		    SGA_IntervalsSetBuilder_build(&parsed->node_presences.array[i], &sg.nodes.nodes[i].presence);
		if (err.type != None) {
			fprintf(stderr,
				"Error while building presence intervals for node %zu: %s\n",
				i,
				SGA_IntervalsSetBuilderError_to_string(&err).data);
			exit(1);
		}

		SGA_Node* node	    = &sg.nodes.nodes[i];
		node->nb_neighbours = parsed->neighbours_of_nodes.array[i].length;
		node->neighbours    = MALLOC(node->nb_neighbours * sizeof(SGA_LinkId));
		for (size_t j = 0; j < node->nb_neighbours; j++) {
			node->neighbours[j] = parsed->neighbours_of_nodes.array[i].array[j];
		}
	}

	sg.links = LinksSet_alloc(parsed->link_presences.length);
	for (size_t i = 0; i < parsed->link_presences.length; i++) {
		SGA_IntervalsSetBuilderError err =
		    SGA_IntervalsSetBuilder_build(&parsed->link_presences.array[i], &sg.links.links[i].presence);
		if (err.type != None) {
			fprintf(stderr,
				"Error while building presence intervals for link %zu: %s\n",
				i,
				SGA_IntervalsSetBuilderError_to_string(&err).data);
			exit(1);
		}

		// Find the nodes linked by this link
		SGA_Link* link = &sg.links.links[i];
		FOR_EACH_ELEM(LinkIdMap, entry, parsed->link_id_map, {
			if (entry.id == i) {
				link->nodes[0] = entry.nodes[0];
				link->nodes[1] = entry.nodes[1];
				break;
			}
		})
	}

	SGA_TimeArrayList key_instants = SGA_TimeArrayList_new();
	SGA_TimeArrayList_push(&key_instants, sg.lifespan.start);
	SGA_Time last_instant = sg.lifespan.start;
	for (size_t i = 0; i < parsed->events.length; i++) {
		SGA_ParsedEvent event = parsed->events.array[i];
		if (event.instant != last_instant) {
			SGA_TimeArrayList_push(&key_instants, event.instant);
			last_instant = event.instant;
		}
	}
	sg.key_instants = KeyInstantsTable_from_list(&key_instants);
	SGA_TimeArrayList_destroy(key_instants);

	init_events_table(&sg, key_instants.length);

	return sg;
}

void SGA_StreamGraph_destroy(SGA_StreamGraph sg) {
	for (size_t i = 0; i < sg.nodes.nb_nodes; i++) {
		free(sg.nodes.nodes[i].neighbours);
		free(sg.nodes.nodes[i].presence.intervals);
	}
	free(sg.nodes.nodes);
	for (size_t i = 0; i < sg.links.nb_links; i++) {
		free(sg.links.links[i].presence.intervals);
	}
	free(sg.links.links);
	KeyInstantsTable_destroy(sg.key_instants);
	EventsTable_destroy(sg.events);
}

// TODO: WHY ISN'T THIS IN NODES_SET.C ??
// TODO: it's because i need the neighbours and all of that...
String SGA_Node_to_string(SGA_StreamGraph* sg, size_t node_idx) {

	SGA_Node* node = &sg->nodes.nodes[node_idx];

	String str = String_from_duplicate("Node ");
	String_append_formatted(&str, "%zu {\n", node_idx);
	String_push_str(&str, "\tPresence : ");

	String presence_str = SGA_IntervalsSet_to_string(&node->presence);
	String_concat_consume(&str, presence_str);

	String_push_str(&str, "\n\tNeighbours : [\n\t\t");
	for (size_t i = 0; i < node->nb_neighbours; i++) {
		// get the names of the neighbours through the links
		size_t link_idx	     = node->neighbours[i];
		size_t node1	     = sg->links.links[link_idx].nodes[0];
		size_t node2	     = sg->links.links[link_idx].nodes[1];
		size_t neighbour_idx = (node1 == node_idx) ? node2 : node1;
		String_append_formatted(&str, "%zu, ", neighbour_idx);
	}
	String_push_str(&str, "\n\t]\n\n}");

	return str;
}

SGA_Interval SGA_StreamGraph_lifespan(SGA_StreamGraph* sg) {
	return sg->lifespan;
}

size_t SGA_StreamGraph_time_scale(SGA_StreamGraph* sg) {
	return sg->time_scale;
}

String SGA_StreamGraph_to_string(SGA_StreamGraph* sg) {
	String str = String_from_duplicate("StreamGraph {\n");
	String_append_formatted(&str, "\tLifespan=[%zu %zu[\n", sg->lifespan.start, sg->lifespan.end);

	// SGA_Nodes
	String_push_str(&str, "\tNodes=[\n");
	for (size_t i = 0; i < sg->nodes.nb_nodes; i++) {
		String node_str = SGA_Node_to_string(sg, i);
		String_push(&node_str, '\n');
		String_concat_consume(&str, node_str);
	}
	String_push_str(&str, "\t]\n");

	// Links
	String_push_str(&str, "\tLinks=[\n");
	for (size_t i = 0; i < sg->links.nb_links; i++) {
		String link_str = SGA_Link_to_string(&sg->links.links[i]);
		String_push(&link_str, '\n');
		String_concat_consume(&str, link_str);
	}
	String_push_str(&str, "\t]\n");

	String_push_str(&str, "}\n");

	return str;
}

SGA_StreamGraph SGA_StreamGraph_from_file(const char* filename) {
	SGA_ParsedStreamGraph parsed = SGA_parse_stream_graph(filename);
	SGA_StreamGraph sg	     = SGA_stream_graph_from_parsed(&parsed);
	SGA_ParsedStreamGraph_destroy(parsed);
	return sg;
}

void init_cache(SGA_Stream* stream) {
	stream->cache.temporal_cardinal_of_node_set.present = false;
	stream->cache.duration.present			    = false;
	stream->cache.temporal_cardinal_of_link_set.present = false;
	stream->cache.distinct_cardinal_of_node_set.present = false;
}

void reset_cache(SGA_Stream* stream) {
	stream->cache.temporal_cardinal_of_node_set.present = false;
	stream->cache.duration.present			    = false;
	stream->cache.temporal_cardinal_of_link_set.present = false;
	stream->cache.distinct_cardinal_of_node_set.present = false;
}

SGA_StreamGraph SGA_StreamGraph_from(SGA_Interval lifespan, size_t time_scale, NodesSet nodes, LinksSet links,
				     SGA_TimeArrayList key_instants) {

	// If the last instant isn't the end of the stream, we add it
	SGA_Time last_key_instant = key_instants.array[key_instants.length - 1];
	if (last_key_instant != lifespan.end) {
		SGA_TimeArrayList_push(&key_instants, lifespan.end);
	}

	// Create the key instants table
	KeyInstantsTable kmt = KeyInstantsTable_from_list(&key_instants);

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

	SGA_StreamGraph sg = {
	    .lifespan	  = lifespan,
	    .time_scale	  = time_scale,
	    .nodes	  = nodes,
	    .links	  = links,
	    .key_instants = kmt,
	};
	// Initialise the events table
	init_events_table(&sg, key_instants.length);

	return sg;
}