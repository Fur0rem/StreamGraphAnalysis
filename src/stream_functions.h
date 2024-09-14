#ifndef STREAM_FUNCTIONS_H
#define STREAM_FUNCTIONS_H

#include "iterators.h"
#include "stream.h"
#include "stream_graph/links_set.h"
#include "stream_graph/nodes_set.h"

typedef struct {
	NodesIterator (*nodes_set)(StreamData*);
	LinksIterator (*links_set)(StreamData*);
	Interval (*lifespan)(StreamData*);
	size_t (*scaling)(StreamData*);

	NodesIterator (*nodes_present_at_t)(StreamData*, TimeId);
	LinksIterator (*links_present_at_t)(StreamData*, TimeId);

	TimesIterator (*times_node_present)(StreamData*, NodeId);
	TimesIterator (*times_link_present)(StreamData*, LinkId);

	Link (*link_by_id)(StreamData*, size_t);
	TemporalNode (*node_by_id)(StreamData*, size_t);

	LinksIterator (*neighbours_of_node)(StreamData*, NodeId);
	// TODO: rename to link_between_nodes (singular)
	LinkId (*link_between_nodes)(StreamData*, NodeId, NodeId);
} StreamFunctions;

#define STREAM_FUNCS(variable, stream_var)                                                                                                 \
	({                                                                                                                                     \
		switch ((stream_var)->type) {                                                                                                      \
			case FULL_STREAM_GRAPH: {                                                                                                      \
				(variable) = FullStreamGraph_stream_functions;                                                                             \
				break;                                                                                                                     \
			}                                                                                                                              \
			case LINK_STREAM: {                                                                                                            \
				(variable) = LinkStream_stream_functions;                                                                                  \
				break;                                                                                                                     \
			}                                                                                                                              \
			case CHUNK_STREAM: {                                                                                                           \
				(variable) = ChunkStream_stream_functions;                                                                                 \
				break;                                                                                                                     \
			}                                                                                                                              \
			case CHUNK_STREAM_SMALL: {                                                                                                     \
				(variable) = ChunkStreamSmall_stream_functions;                                                                            \
				break;                                                                                                                     \
			}                                                                                                                              \
			case SNAPSHOT_STREAM: {                                                                                                        \
				(variable) = SnapshotStream_stream_functions;                                                                              \
				break;                                                                                                                     \
			}                                                                                                                              \
			default: {                                                                                                                     \
				UNREACHABLE_CODE;                                                                                                          \
			}                                                                                                                              \
		}                                                                                                                                  \
		(variable);                                                                                                                        \
	})

#endif // STREAM_FUNCTIONS_H