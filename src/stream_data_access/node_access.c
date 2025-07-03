#define SGA_INTERNAL

#include "node_access.h"

// DEFAULT_EQUALS(SGA_NodeId);
// NO_FREE(SGA_NodeId);
// DEFAULT_COMPARE(SGA_NodeId);
// DEFAULT_TO_STRING(SGA_NodeId, "%zu");
// DefineArrayList(SGA_NodeId);
// DefineArrayListDeriveEquals(SGA_NodeId);
// DefineArrayListDeriveRemove(SGA_NodeId);
// DefineArrayListDeriveToString(SGA_NodeId);
// DefineArrayListDeriveOrdered(SGA_NodeId);

typedef struct {
	SGA_NodeId current_node;
} NodesSetIteratorData;

SGA_NodeId NodesSet_next(SGA_NodesIterator* iter) {
	NodesSetIteratorData* nodes_iter_data = (NodesSetIteratorData*)iter->iterator_data;
	SGA_StreamGraph* stream_graph	      = iter->stream_graph.stream_data;
	if (nodes_iter_data->current_node >= stream_graph->nodes.nb_nodes) {
		return SGA_NODES_ITERATOR_END;
	}
	SGA_NodeId return_val = nodes_iter_data->current_node;
	nodes_iter_data->current_node++;
	return return_val;
}

void NodesSetIterator_destroy(SGA_NodesIterator* iterator) {
	free(iterator->iterator_data);
}

SGA_NodesIterator SGA_StreamGraph_nodes_set(SGA_StreamGraph* stream_graph) {
	NodesSetIteratorData* iterator_data = MALLOC(sizeof(NodesSetIteratorData));
	iterator_data->current_node	    = 0;

	SGA_Stream stream = {
	    .stream_data = stream_graph,
	};
	SGA_NodesIterator nodes_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = NodesSet_next,
	    .destroy	   = NodesSetIterator_destroy,
	};
	return nodes_iterator;
}

typedef struct {
	SGA_TimeId current_interval;
	SGA_Node* node;
} TimesNodePresentIteratorData;

SGA_Interval StreamGraph_times_node_present_next(SGA_TimesIterator* iter) {
	TimesNodePresentIteratorData* times_iter_data = iter->iterator_data;
	SGA_Node* node				      = times_iter_data->node;
	if (times_iter_data->current_interval >= node->presence.nb_intervals) {
		return SGA_TIMES_ITERATOR_END;
	}
	SGA_Interval return_val = node->presence.intervals[times_iter_data->current_interval];
	times_iter_data->current_interval++;
	return return_val;
}

void StreamGraph_times_node_present_destroy(SGA_TimesIterator* iterator) {
	free(iterator->iterator_data);
}

SGA_TimesIterator SGA_StreamGraph_times_node_present(SGA_StreamGraph* stream_graph, SGA_NodeId node_id) {
	ASSERT(node_id < stream_graph->nodes.nb_nodes);

	TimesNodePresentIteratorData* iterator_data = MALLOC(sizeof(TimesNodePresentIteratorData));
	iterator_data->current_interval		    = 0;
	iterator_data->node			    = &stream_graph->nodes.nodes[node_id];

	SGA_Stream stream = {
	    .stream_data = stream_graph,
	};
	SGA_TimesIterator times_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = StreamGraph_times_node_present_next,
	    .destroy	   = StreamGraph_times_node_present_destroy,
	};
	return times_iterator;
}

typedef struct {
	SGA_Node* node;
	SGA_LinkId current_neighbour;
} NeighboursOfNodeIteratorData;

size_t NeighboursOfNode_next(SGA_LinksIterator* iter) {
	NeighboursOfNodeIteratorData* neighbours_iter_data = iter->iterator_data;
	SGA_Node* node					   = neighbours_iter_data->node;
	if (neighbours_iter_data->current_neighbour >= node->nb_neighbours) {
		return SGA_LINKS_ITERATOR_END;
	}
	size_t return_val = node->neighbours[neighbours_iter_data->current_neighbour];
	neighbours_iter_data->current_neighbour++;
	return return_val;
}

void NeighboursOfNodeIterator_destroy(SGA_LinksIterator* iterator) {
	free(iterator->iterator_data);
}

SGA_LinksIterator SGA_StreamGraph_neighbours_of_node(SGA_StreamGraph* stream_graph, SGA_NodeId node_id) {
	ASSERT(node_id < stream_graph->nodes.nb_nodes);

	NeighboursOfNodeIteratorData* iterator_data = MALLOC(sizeof(NeighboursOfNodeIteratorData));
	iterator_data->node			    = &stream_graph->nodes.nodes[node_id];
	iterator_data->current_neighbour	    = 0;

	SGA_Stream stream = {
	    .stream_data = stream_graph,
	};
	SGA_LinksIterator neighbours_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = NeighboursOfNode_next,
	    .destroy	   = NeighboursOfNodeIterator_destroy,
	};
	return neighbours_iterator;
}

SGA_Node SGA_StreamGraph_node_by_id(SGA_StreamGraph* stream_graph, SGA_NodeId node_id) {
	ASSERT(node_id < stream_graph->nodes.nb_nodes);
	return stream_graph->nodes.nodes[node_id];
}