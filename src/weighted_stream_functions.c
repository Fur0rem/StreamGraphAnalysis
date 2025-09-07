#define SGA_INTERNAL

#include "weighted_stream_functions.h"

#include "stream.h"
#include "streams.h"
#include "weighted_stream.h"

/**
 * @brief Get the table of functions to access the data of a weighted StreamGraph.
 * @param stream The Stream to get the functions from.
 * @return The table of functions to access the data of a weighted StreamGraph.
 */
WeightedStreamFunctions SGA_Weighted_StreamFunctions(const SGA_W_Stream* stream) {
	switch (stream->base.type) {
		case FULL_STREAM_GRAPH: {
			return FullStreamGraph_weighted_stream_functions;
		}
		case LINK_STREAM: {
			return LinkStream_weighted_stream_functions;
		}
		case TIMEFRAME_STREAM: {
			return TimeFrameStream_weighted_stream_functions;
		}
	}
	// case CHUNK_STREAM: {
	// 	return ChunkStream_weighted_stream_functions;
	// }
	// case CHUNK_STREAM_SMALL: {
	// 	return ChunkStreamSmall_weighted_stream_functions;
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

/**
 * @brief Casts a weighted stream into a regular Stream to use the regular Stream functions.
 * @param self The weighted stream to cast.
 * @return A pointer to a regular SGA_Stream
 */
SGA_Stream* SGA_W_Stream_as_regular_stream(SGA_W_Stream* self) {
	return &self->base;
}

void SGA_W_Stream_destroy(SGA_W_Stream stream) {
	switch (stream.base.type) {
		case FULL_STREAM_GRAPH: {
			SGA_W_FullStreamGraph_destroy(stream);
			break;
		}
		case LINK_STREAM: {
			SGA_W_LinkStream_destroy(stream);
			break;
		}
		// case CHUNK_STREAM: {
		// 	SGA_W_ChunkStream_destroy(stream);
		// 	break;
		// }
		// case CHUNK_STREAM_SMALL: {
		// 	SGA_W_ChunkStreamSmall_destroy(stream);
		// 	break;
		// }
		case TIMEFRAME_STREAM: {
			SGA_W_TimeFrameStream_destroy(stream);
			break;
		}
		// case DELTA_STREAM: {
		// 	SGA_W_DeltaStream_destroy(stream);
		// 	break;
		// }
		default: {
			UNREACHABLE_CODE;
		}
	}
}
