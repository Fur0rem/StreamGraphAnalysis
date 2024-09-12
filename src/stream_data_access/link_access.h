#ifndef SGA_LINK_ACCESS_H
#define SGA_LINK_ACCESS_H

#include "../iterators.h"

LinksIterator StreamGraph_links_set(StreamGraph* stream_graph);
LinksIterator StreamGraph_links_present_at(StreamGraph* stream_graph, TimeId t);
Link StreamGraph_link_by_id(StreamGraph* stream_graph, LinkId link_id);
TimesIterator StreamGraph_times_link_present(StreamGraph* stream_graph, LinkId link_id);
LinkId StreamGraph_link_between_nodes(StreamGraph* stream_graph, NodeId node_id, NodeId other_node_id);

#endif