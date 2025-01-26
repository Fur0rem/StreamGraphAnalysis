/**
 * @file src/stream.h
 * @brief Create StreamGraph's and manipulate generic Stream's.
 * A StreamGraph is the main data structure to represent a stream graph.
 * It contains all the information about the nodes, links, and its presence in time.
 * <br>
 * A Stream is a wrapper/interface around a StreamGraph.
 * All the functions to access the data and metrics take a Stream as input, and not a StreamGraph.
 * The interface asks for the bare minimum to access the data and compute metrics on it.
 * Various actual implementations of Stream's can change the way a StreamGraph is viewed.
 * <br>
 * The 4 main types of Stream are:
 * - FullStreamGraph: A StreamGraph as is.
 * - LinkStream: A StreamGraph where all nodes are present at all times.
 * - ChunkStream and ChunkStreamSmall: A subset of the StreamGraph.
 * - SnapshotStream: A StreamGraph at a specific time interval.
 * <br>
 * All of these Stream's use an underlying StreamGraph, but transform the data in different ways.
 * <br>
 * The Stream's do not actually modify the underlying StreamGraph, they only provide a different access to it.
 * Each implementation tries to do lazy evaluation and only compute the data when needed, and transform it on the fly.
 * Each implementation can also specialise some implementations of metrics to be more efficient.
 * You can also manipulate multiple Stream's of the same StreamGraph at the same time.
 */

#ifndef STREAM_H
#define STREAM_H

#include "bit_array.h"
#include "interval.h"
#include "stream_graph/events_table.h"
#include "stream_graph/key_moments_table.h"
#include "stream_graph/links_set.h"
#include "stream_graph/nodes_set.h"
#include "units.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief The structure used to represent a stream graph.
 *
 * It is opaque to the user, you should not access its fields directly.
 */
typedef struct SGA_StreamGraph SGA_StreamGraph;

/**
 * @brief The internal implementation of a StreamGraph.
 */
struct SGA_StreamGraph {
	KeyMomentsTable key_moments; ///< Key moments of the StreamGraph. A key moment is a moment where the StreamGraph changes.
	NodesSet nodes;		     ///< The nodes of the StreamGraph.
	LinksSet links;		     ///< The links of the StreamGraph.
	SGA_Interval lifespan;	     ///< The time interval of the StreamGraph.
	size_t time_scale;	     ///< By how much the time is scaled. Used to normalise many time-related metrics.

	EventsTable events; ///< The events of the StreamGraph. An event keeps track of additions and removals of nodes and links.
			    ///< Needs to be initialised separately with init_events_table. As it is quite memory and time consuming, and
			    ///< not needed in all cases.
};

/**
 * @brief A size_t that can be optional.
 */
typedef struct {
	bool present; ///< Tells if the value is present.
	size_t data;  ///< The value.
} OptionalSizeT;

/**
 * @brief A cache of information about the 4 main sets of a Stream: the temporal nodes W, the temporal links E, the life span T, and the
 * distinct nodes V.
 *
 * Some metrics share the same computation, for example, the average node degree, compactness, and density all need the cardinal of W.
 * Instead of recomputing the same value multiple times, we store it in a cache, as all the values are constant for a given Stream.
 */
typedef struct {
	OptionalSizeT temporal_cardinal_of_node_set; ///< The cardinal of the temporal nodes set W.
	OptionalSizeT duration;			     ///< The cardinal of the life span T.
	OptionalSizeT temporal_cardinal_of_link_set; ///< The cardinal of the temporal links set E.
	OptionalSizeT distinct_cardinal_of_node_set; ///< The cardinal of the distinct nodes set V.
	OptionalSizeT distinct_cardinal_of_link_set; ///< The cardinal of the distinct links set. (Not defined in the paper)
} InformationCache;

SGA_StreamGraph SGA_StreamGraph_from_string(const char* str);
SGA_StreamGraph SGA_StreamGraph_from_file(const char* filename);
String SGA_StreamGraph_to_string(SGA_StreamGraph* sg);
void SGA_StreamGraph_destroy(SGA_StreamGraph sg);

// void SGA_init_events_table(SGA_StreamGraph* sg);
// void SGA_events_destroy(SGA_StreamGraph* sg);
char* SGA_InternalFormat_from_External_str(const char* str);
SGA_StreamGraph SGA_StreamGraph_from_external(const char* filename);

// TRICK: StreamData is a blanket struct that serves as a placeholder for the union of all stream types
// (FullStreamGraph, LinkStream, ect...).
// Otherwise a circular dependency would be created since these files need stream.h.
// And if we used a void* pointer, we would lose type safety.

/**
 * @brief Blanket struct that serves as a placeholder for the union of all stream types (FullStreamGraph, LinkStream, etc...).
 * Otherwise a circular dependency would be created since these files need stream.h.
 * It is safe to cast this to the actual type of the Stream (which is known in the Stream struct).
 */
// typedef struct {
// } SGA_StreamData;
typedef void SGA_StreamData;

/**
 * @brief The structure of a Stream.
 *
 * Depends on an underlying StreamGraph, so its lifetime/scope should be less than the StreamGraph.
 */
typedef struct SGA_Stream {
	/**
	 * @brief The type of the Stream.
	 */
	enum {
		FULL_STREAM_GRAPH,  ///< A FullStreamGraph
		LINK_STREAM,	    ///< A LinkStream
		CHUNK_STREAM,	    ///< A ChunkStream
		CHUNK_STREAM_SMALL, ///< A ChunkStreamSmall
		SNAPSHOT_STREAM,    ///< A SnapshotStream
	} type;

	SGA_StreamData* stream_data; ///< The data of the Stream. It is a union of all the different types of StreamData.
	InformationCache cache;	     ///< A cache of information about the 4 main sets of a Stream.
} SGA_Stream;

#ifdef SGA_INTERNAL

/**
 * @brief Initialise the cache of a Stream, starting empty.
 * @param[out] stream The Stream to initialise the cache of.
 */
void init_cache(SGA_Stream* stream);

/**
 * @brief Fetch a value from the cache of a Stream.
 * @param[in] stream The Stream to fetch the value from.
 * @param[in] field The field of the cache to fetch.
 *s
 * If the value is present in the cache, it is returned early in the function.
 * Otherwise, the function continues normally.
 */
#	define FETCH_CACHE(stream, field)                                                                                                 \
		if ((stream)->cache.field.present) {                                                                                       \
			return (stream)->cache.field.data;                                                                                 \
		}

/**
 * @brief Write a value to the cache of a Stream.
 * @param[out] stream The Stream to write the value to.
 * @param[out] field The field of the cache to write to.
 * @param[in] value The value to write to the cache.
 */
#	define UPDATE_CACHE(stream, field, value)                                                                                         \
		(stream)->cache.field.present = true;                                                                                      \
		(stream)->cache.field.data    = value;

/**
 * @brief Reset the cache of a Stream, emptying it.
 * @param[out] stream The Stream to reset the cache of.
 */
void reset_cache(SGA_Stream* stream);

SGA_Interval SGA_StreamGraph_lifespan(SGA_StreamGraph* sg);
size_t SGA_StreamGraph_time_scale(SGA_StreamGraph* sg);
void init_events_table(SGA_StreamGraph* sg);
void events_destroy(SGA_StreamGraph* sg);

#endif // SGA_INTERNAL

#endif // STREAM_H