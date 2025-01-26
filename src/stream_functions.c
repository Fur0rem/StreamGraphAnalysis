#include "iterators.h"
#define SGA_INTERNAL

// TODO: rename to stream_access maybe?

#include "stream_functions.h"
#include "streams.h"

SGA_Interval SGA_Stream_lifespan(SGA_Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.lifespan(stream->stream_data);
}

size_t SGA_Stream_time_scale(SGA_Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.time_scale(stream->stream_data);
}

SGA_NodesIterator SGA_Stream_nodes_set(SGA_Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.nodes_set(stream->stream_data);
}

SGA_LinksIterator SGA_Stream_links_set(SGA_Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.links_set(stream->stream_data);
}

SGA_NodesIterator SGA_Stream_nodes_present_at_t(SGA_Stream* stream, SGA_Time time) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.nodes_present_at_t(stream->stream_data, time);
}

SGA_LinksIterator SGA_Stream_links_present_at_t(SGA_Stream* stream, SGA_Time time) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.links_present_at_t(stream->stream_data, time);
}

SGA_TimesIterator SGA_Stream_times_node_present(SGA_Stream* stream, SGA_NodeId node_id) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.times_node_present(stream->stream_data, node_id);
}

SGA_TimesIterator SGA_Stream_times_link_present(SGA_Stream* stream, SGA_LinkId link_id) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.times_link_present(stream->stream_data, link_id);
}

SGA_Link SGA_Stream_link_by_id(SGA_Stream* stream, size_t link_id) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.link_by_id(stream->stream_data, link_id);
}

SGA_Node SGA_Stream_node_by_id(SGA_Stream* stream, size_t node_id) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.node_by_id(stream->stream_data, node_id);
}

SGA_LinksIterator SGA_Stream_neighbours_of_node(SGA_Stream* stream, SGA_NodeId node_id) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.neighbours_of_node(stream->stream_data, node_id);
}

SGA_LinkId SGA_Stream_link_between_nodes(SGA_Stream* stream, SGA_NodeId node_id, SGA_NodeId other_node_id) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.link_between_nodes(stream->stream_data, node_id, other_node_id);
}

SGA_TimesIterator SGA_Stream_key_moments(SGA_Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	return fns.key_moments(stream->stream_data);
}