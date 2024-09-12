#ifndef SGA_NODE_ACCESS_H
#define SGA_NODE_ACCESS_H

#include "../iterators.h"

LinksIterator StreamGraph_neighbours_of_node(StreamGraph* stream_graph, NodeId node_id);
TimesIterator StreamGraph_times_node_present(StreamGraph* stream_graph, NodeId node_id);
NodesIterator StreamGraph_nodes_set(StreamGraph* stream_graph);
TemporalNode StreamGraph_node_by_id(StreamGraph* stream_graph, NodeId node_id);

#endif