#ifndef INDUCED_GRAPH_H
#define INDUCED_GRAPH_H

#include "stream_graph.h"
#include <stddef.h>

NodesIterator get_nodes_present_at_t(StreamGraph* stream_graph, TimeId t);

typedef struct {
	size_t current_event;
	size_t current_node;
} NodesPresentAtTIterator;

LinksIterator get_links_present_at_t(StreamGraph* stream_graph, TimeId t);

typedef struct {
	size_t current_event;
	size_t current_link;
} LinksPresentAtTIterator;

// The || ({ x.destroy(&x); 0; }) is a trick to execute the destroy function of the iterator when it ends
// The destroy function is only called when the previous condition is false, and then evaluates to 0 (false)
// This uses the GNU extension of "Statement Expressions"
#define FOR_EACH(type_iterated, iterated, iterator, end_cond)                                                          \
	for (type_iterated iterated = (iterator).next(&(iterator)); (end_cond) || ({                                       \
																	(iterator).destroy(&(iterator));                   \
																	0;                                                 \
																});                                                    \
		 (iterated) = (iterator).next(&(iterator)))

// TODO : Change the order of iterated and iterator in the FOR_EACH macro
#define FOR_EACH_NODE(iterator, iterated) FOR_EACH(size_t, iterated, iterator, (iterated) != SIZE_MAX)
#define FOR_EACH_LINK(iterator, iterated) FOR_EACH(size_t, iterated, iterator, (iterated) != SIZE_MAX)
#define FOR_EACH_TIME(iterator, iterated) FOR_EACH(Interval, iterated, iterator, (iterated).start != SIZE_MAX)

#define ITERATOR_IS_EMPTY(iterator)                                                                                    \
	({                                                                                                                 \
		size_t __tmp = (iterator).next(&(iterator));                                                                   \
		(iterator).destroy(&(iterator));                                                                               \
		__tmp == SIZE_MAX;                                                                                             \
	})

#endif // INDUCED_GRAPH_H