#include "node_access.h"

typedef struct {
	NodeId current_node;
} NodesSetIteratorData;

NodeId NodesSet_next(NodesIterator* iter) {
	NodesSetIteratorData* nodes_iter_data = (NodesSetIteratorData*)iter->iterator_data;
	StreamGraph* stream_graph			  = iter->stream_graph.stream_data;
	if (nodes_iter_data->current_node >= stream_graph->nodes.nb_nodes) {
		return NODES_ITERATOR_END;
	}
	NodeId return_val = nodes_iter_data->current_node;
	nodes_iter_data->current_node++;
	return return_val;
}

void NodesSetIterator_destroy(NodesIterator* iterator) {
	free(iterator->iterator_data);
}

NodesIterator StreamGraph_nodes_set(StreamGraph* stream_graph) {
	NodesSetIteratorData* iterator_data = MALLOC(sizeof(NodesSetIteratorData));
	iterator_data->current_node			= 0;

	Stream stream = {
		.stream_data = stream_graph,
	};
	NodesIterator nodes_iterator = {
		.stream_graph  = stream,
		.iterator_data = iterator_data,
		.next		   = NodesSet_next,
		.destroy	   = NodesSetIterator_destroy,
	};
	return nodes_iterator;
}

typedef struct {
	TimeId current_interval;
	TemporalNode* node;
} TimesNodePresentIteratorData;

Interval StreamGraph_times_node_present_next(TimesIterator* iter) {
	TimesNodePresentIteratorData* times_iter_data = iter->iterator_data;
	TemporalNode* node							  = times_iter_data->node;
	if (times_iter_data->current_interval >= node->presence.nb_intervals) {
		return TIMES_ITERATOR_END;
	}
	Interval return_val = node->presence.intervals[times_iter_data->current_interval];
	times_iter_data->current_interval++;
	return return_val;
}

void StreamGraph_times_node_present_destroy(TimesIterator* iterator) {
	free(iterator->iterator_data);
}

TimesIterator StreamGraph_times_node_present(StreamGraph* stream_graph, NodeId node_id) {
	ASSERT(node_id < stream_graph->nodes.nb_nodes);

	TimesNodePresentIteratorData* iterator_data = MALLOC(sizeof(TimesNodePresentIteratorData));
	iterator_data->current_interval				= 0;
	iterator_data->node							= &stream_graph->nodes.nodes[node_id];

	Stream stream = {
		.stream_data = stream_graph,
	};
	TimesIterator times_iterator = {
		.stream_graph  = stream,
		.iterator_data = iterator_data,
		.next		   = StreamGraph_times_node_present_next,
		.destroy	   = StreamGraph_times_node_present_destroy,
	};
	return times_iterator;
}

typedef struct {
	TemporalNode* node;
	LinkId current_neighbour;
} NeighboursOfNodeIteratorData;

size_t NeighboursOfNode_next(LinksIterator* iter) {
	NeighboursOfNodeIteratorData* neighbours_iter_data = iter->iterator_data;
	TemporalNode* node								   = neighbours_iter_data->node;
	if (neighbours_iter_data->current_neighbour >= node->nb_neighbours) {
		return LINKS_ITERATOR_END;
	}
	size_t return_val = node->neighbours[neighbours_iter_data->current_neighbour];
	neighbours_iter_data->current_neighbour++;
	return return_val;
}

void NeighboursOfNodeIterator_destroy(LinksIterator* iterator) {
	free(iterator->iterator_data);
}

LinksIterator StreamGraph_neighbours_of_node(StreamGraph* stream_graph, NodeId node_id) {
	ASSERT(node_id < stream_graph->nodes.nb_nodes);

	NeighboursOfNodeIteratorData* iterator_data = MALLOC(sizeof(NeighboursOfNodeIteratorData));
	iterator_data->node							= &stream_graph->nodes.nodes[node_id];
	iterator_data->current_neighbour			= 0;

	Stream stream = {
		.stream_data = stream_graph,
	};
	LinksIterator neighbours_iterator = {
		.stream_graph  = stream,
		.iterator_data = iterator_data,
		.next		   = NeighboursOfNode_next,
		.destroy	   = NeighboursOfNodeIterator_destroy,
	};
	return neighbours_iterator;
}

TemporalNode StreamGraph_node_by_id(StreamGraph* stream_graph, NodeId node_id) {
	ASSERT(node_id < stream_graph->nodes.nb_nodes);
	return stream_graph->nodes.nodes[node_id];
}