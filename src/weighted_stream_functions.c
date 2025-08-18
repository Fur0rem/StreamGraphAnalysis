#include "weighted_stream_functions.h"

#include "stream.h"
#include "streams/full_stream_graph.h"
#include "weighted_stream.h"

#ifdef SGA_INTERNAL
/**
 * @brief Get the table of functions to access the data of a weighted StreamGraph.
 * @param stream The Stream to get the functions from.
 * @return The table of functions to access the data of a weighted StreamGraph.
 */
WeightedStreamFunctions SGA_Weighted_StreamFunctions(const SGA_W_Stream* stream) {
	// switch (stream->base.type) {
	// case FULL_STREAM_GRAPH: {
	// 	return FullStreamGraph_weighted_stream_functions;
	// }
	// case LINK_STREAM: {
	// 	return LinkStream_weighted_stream_functions;
	// }
	// case CHUNK_STREAM: {
	// 	return ChunkStream_weighted_stream_functions;
	// }
	// case CHUNK_STREAM_SMALL: {
	// 	return ChunkStreamSmall_weighted_stream_functions;
	// }
	// case TIMEFRAME_STREAM: {
	// 	return TimeFrameStream_weighted_stream_functions;
	// }
	// case DELTA_STREAM: {
	// 	return DeltaStream_weighted_stream_functions;
	// }
	// default: {
	// 	UNREACHABLE_CODE;
	// }
	// }
	return FullStreamGraph_weighted_stream_functions;
}
#endif // SGA_INTERNAL

/**
 * @brief Casts a weighted stream into a regular Stream to use the regular Stream functions.
 * @param self The weighted stream to cast.
 * @return A pointer to a regular SGA_Stream
 */
SGA_Stream* SGA_W_Stream_as_regular_stream(SGA_W_Stream* self) {
	return &self->base;
}

void SGA_W_Stream_destroy(SGA_Stream stream) {}
