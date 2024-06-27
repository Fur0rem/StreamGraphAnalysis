#ifndef STREAM_FUNCTIONS_H
#define STREAM_FUNCTIONS_H

#include "iterators.h"
#include "stream_graph/links_set.h"

typedef struct {
	NodesIterator (*nodes_set)(void*);
	LinksIterator (*links_set)(void*);
	Interval (*lifespan)(void*);
	size_t (*scaling)(void*);

	NodesIterator (*nodes_present_at_t)(void*, TimeId);
	LinksIterator (*links_present_at_t)(void*, TimeId);

	TimesIterator (*times_node_present)(void*, NodeId);
	TimesIterator (*times_link_present)(void*, LinkId);

	Link (*nth_link)(void*, size_t);
	LinksIterator (*neighbours_of_node)(void*, NodeId);

} StreamFunctions;

#define STREAM_FUNCS(variable, stream_var)                                                                             \
	({                                                                                                                 \
		switch ((stream_var)->type) {                                                                                  \
			case FULL_STREAM_GRAPH: {                                                                                  \
				(variable) = FullStreamGraph_stream_functions;                                                         \
				break;                                                                                                 \
			}                                                                                                          \
			case LINK_STREAM: {                                                                                        \
				(variable) = LinkStream_stream_functions;                                                              \
				break;                                                                                                 \
			}                                                                                                          \
			case CHUNK_STREAM: {                                                                                       \
				(variable) = ChunkStream_stream_functions;                                                             \
				break;                                                                                                 \
			}                                                                                                          \
			case CHUNK_STREAM_SMALL: {                                                                                 \
				(variable) = ChunkStreamSmall_stream_functions;                                                        \
				break;                                                                                                 \
			}                                                                                                          \
		}                                                                                                              \
		(variable);                                                                                                    \
	})

#endif // STREAM_FUNCTIONS_H