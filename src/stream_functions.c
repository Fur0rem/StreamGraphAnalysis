#include "iterators.h"
#include "stream.h"
#define SGA_INTERNAL

// TODO: rename to stream_access maybe?

#include "stream_functions.h"
#include "streams.h"

SGA_Interval SGA_Stream_lifespan(const SGA_Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.lifespan(stream->stream_data);
}

size_t SGA_Stream_time_scale(const SGA_Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.time_scale(stream->stream_data);
}

SGA_NodesIterator SGA_Stream_nodes_set(const SGA_Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.nodes_set(stream->stream_data);
}

SGA_LinksIterator SGA_Stream_links_set(const SGA_Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.links_set(stream->stream_data);
}

SGA_NodesIterator SGA_Stream_nodes_present_at_t(const SGA_Stream* stream, SGA_Time time) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.nodes_present_at_t(stream->stream_data, time);
}

SGA_LinksIterator SGA_Stream_links_present_at_t(const SGA_Stream* stream, SGA_Time time) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.links_present_at_t(stream->stream_data, time);
}

SGA_TimesIterator SGA_Stream_times_node_present(const SGA_Stream* stream, SGA_NodeId node_id) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.times_node_present(stream->stream_data, node_id);
}

SGA_TimesIterator SGA_Stream_times_link_present(const SGA_Stream* stream, SGA_LinkId link_id) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.times_link_present(stream->stream_data, link_id);
}

SGA_Link SGA_Stream_link_by_id(const SGA_Stream* stream, size_t link_id) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.link_by_id(stream->stream_data, link_id);
}

SGA_Node SGA_Stream_node_by_id(const SGA_Stream* stream, size_t node_id) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.node_by_id(stream->stream_data, node_id);
}

SGA_LinksIterator SGA_Stream_neighbours_of_node(const SGA_Stream* stream, SGA_NodeId node_id) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.neighbours_of_node(stream->stream_data, node_id);
}

SGA_LinkId SGA_Stream_link_between_nodes(const SGA_Stream* stream, SGA_NodeId node_id, SGA_NodeId other_node_id) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.link_between_nodes(stream->stream_data, node_id, other_node_id);
}

SGA_TimesIterator SGA_Stream_key_instants(const SGA_Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.key_instants(stream->stream_data);
}

void SGA_Stream_destroy(SGA_Stream stream) {
	switch (stream.type) {
		case LINK_STREAM:
			SGA_LinkStream_destroy(stream);
			break;
		case CHUNK_STREAM:
			SGA_ChunkStream_destroy(stream);
			break;
		case CHUNK_STREAM_SMALL:
			SGA_ChunkStreamSmall_destroy(stream);
			break;
		case FULL_STREAM_GRAPH:
			SGA_FullStreamGraph_destroy(stream);
			break;
		case TIMEFRAME_STREAM:
			SGA_TimeFrameStream_destroy(stream);
			break;
		case DELTA_STREAM:
			SGA_DeltaStream_destroy(stream);
			break;
	}
}