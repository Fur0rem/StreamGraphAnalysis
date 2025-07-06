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
 * - TimeFrameStream: A StreamGraph at a specific time interval.
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
#include "stream_graph/key_instants_table.h"
#include "stream_graph/links_set.h"
#include "stream_graph/nodes_set.h"
#include "units.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief The internal implementation of a StreamGraph.
 */
typedef struct {
	KeyInstantsTable key_instants; ///< Key instants of the StreamGraph. A key instant is a instant where the StreamGraph changes.
	NodesSet nodes;		       ///< The nodes of the StreamGraph.
	LinksSet links;		       ///< The links of the StreamGraph.
	SGA_Interval lifespan;	       ///< The time interval of the StreamGraph.
	size_t time_scale;	       ///< By how much the time is scaled. Used to normalise many time-related metrics.

	EventsTable events; ///< The events of the StreamGraph. An event keeps track of additions and removals of nodes and links.
			    ///< Needs to be initialised separately with init_events_table. As it is quite memory and time consuming, and
			    ///< not needed in all cases.
} SGA_StreamGraph;

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

/**
 * @brief Creates a StreamGraph from an external format of the version 1.0.0 of the library.
 * @param[in] external_format The string to parse. Assumes it is in the external format.
 * The external format is made to be human-readable and writable, but a bit longer to parse.
 * It is composed of only a general section, and the events section.
 * The general header section the lifespan and the time scale.
 * Each event contains the time instant, the type of event, whether it concerns a node or a link, and the node/link concerned.
 * The events are ordered by time. A link can only appear after both its nodes have appeared. Multiple events can happen at the same time.
 * Here's how it looks:
 * ```
 * SGA External Format version 1.0.0
 *
 * [General]	// Header of the general section
 * Lifespan=(start end)
 * TimeScale=scale
 *
 * [Events] 	// Header of the events section
 * 0 + N 1	// Node 1 appears at time 0
 * 3 + N 0	// Node 0 appears at time 3
 * 3 + L 0 1	// Link 0-1 appears at time 3
 * 4 - L 0 1	// Link 0-1 disappears at time 4
 * 5 - N 1	// Node 1 disappears at time 5
 * 6 - N 0	// Node 0 disappears at time 6
 *
 * [EndOfStream] // Signals the end of the stream
 * Any text after this is ignored, and can be used for comments
 * ```
 * You can check the ```data/``` folder which contains some examples of external formats.
 * @return The StreamGraph.
 */
SGA_StreamGraph SGA_StreamGraph_from_external_format_v_1_0_0(const String* format);

/**
 * @brief Creates a StreamGraph the internal format of the version 1.0.0 of the library.
 * @param[in] internal_format The string to parse. Assumes it is in the internal format.
 * The internal format is made to be more compact and efficient to parse, but way harder to read and write by hand.
 * It is composed of a general section, a memory section, and a data section.
 * The general header section contains the lifespan and the time scale.
 * The memory section contains the number of nodes, links, and key instants.
 * It is also composed of sub-sections :
 * One for the nodes, which contains its number of neighbours and number of presence intervals.
 * One for the links, which contains its number of presence intervals.
 * One for the key instants, which contains in how many slices the time is divided. @see KeyInstantsTable.
 * The data section is also composed of sub-sections :
 * One for the neighbours, for nodes it contains its neighbouring links, for links it contains the two nodes it links.
 * One for the events, which contains the time of the event, and each events that happened at that time.
 * Unlike the external format, all events of a same time are grouped together.
 * Here's how it looks:
 * ```
 * SGA Internal version 1.0.0
 *
 * [General]
 * Lifespan=(start end)
 * TimeScale=scale
 *
 * [Memory]
 * NumberOfNodes=x
 * NumberOfLinks=y
 * NumberOfKeyInstants=z
 *
 * [[Nodes]]
 * [[[NumberOfNeighbours]]]
 * 1	// Node 0 has 1 neighbour
 * 2	// Node 1 has 2 neighbours
 * 1	// Node 2 has 1 neighbour
 * ...
 * [[[NumberOfIntervals]]]
 * 2	// Node 0 has 2 presence intervals
 * 1	// Node 1 has 1 presence interval
 * 3	// Node 2 has 3 presence intervals
 * ...
 *
 * [[Links]]
 * [[[NumberOfIntervals]]]
 * 1	// Link 0 has 1 presence interval
 * 2	// Link 1 has 2 presence intervals
 * 1	// Link 2 has 1 presence interval
 * ...
 *
 * [[[NumberOfKeyInstantsPerSlice]]]
 * 2	// The first slice contains 2 key instants
 * ...
 *
 * [Data]
 * [[Neighbours]]
 * [[[NodesToLinks]]]
 * (1)		// Node 0 is linked to link 1
 * (0 2)	// Node 1 is linked to link 0 and 2
 * (1)		// Node 2 is linked to link 1
 * ...
 * [[[LinksToNodes]]]
 * (1 3)	// Link 0 links nodes 1 and 3
 * (0 2)	// Link 1 links nodes 0 and 2
 * (1 4)	// Link 2 links nodes 1 and 4
 * ...
 * [[Events]]
 * 0=((+ N 0) (+ N 2) (+ L 1)) 	// At time 0, node 0 and 2 appear, and link 1 appears
 * 1=((+ N 1) (- L 1)) 		// At time 1, node 1 appears, and link 1 disappears
 * ...
 *
 * [EndOfStream] // Signals the end of the stream
 * Any text after this is ignored, and can be used for comments
 * ```
 * @return The StreamGraph.
 */
SGA_StreamGraph SGA_StreamGraph_from_internal_format_v_1_0_0(const String* format);

/**
 * @brief Creates a StreamGraph
 * @param lifespan The lifespan of the StreamGraph.
 * @param time_scale The time scale of the StreamGraph.
 * @param nodes The set of nodes in the StreamGraph.
 * @param links The set of links in the StreamGraph.
 * @param key_instants The key instants of the StreamGraph, i.e. all the instants where the StreamGraph changes.
 * @return The StreamGraph.
 */
SGA_StreamGraph SGA_StreamGraph_from(SGA_Interval lifespan, size_t time_scale, NodesSet nodes, LinksSet links,
				     SGA_TimeArrayList key_instants);

/**
 * @brief Creates a StreamGraph from a string.
 * @param sg_as_str The string to parse. Determines the type and version of the format from the header.
 * @see SGA_StreamGraph_from_external_format_v_1_0_0, @see SGA_StreamGraph_from_internal_format_v_1_0_0.
 * @return The StreamGraph.
 */
SGA_StreamGraph SGA_StreamGraph_from_string(const String* sg_as_str);

/**
 * @brief Loads a StreamGraph from a file. Reads the contents of the file and parses it as a StreamGraph. @see SGA_StreamGraph_from_string.
 * @param[in] filename The name of the file to load.
 * @return The StreamGraph.
 */
SGA_StreamGraph SGA_StreamGraph_from_file(const char* filename);

/**
 * @brief Creates a string representation of a StreamGraph.
 * @param[in] sg The StreamGraph to convert to a string.
 * @return The string representation of the StreamGraph.
 */
String SGA_StreamGraph_to_string(SGA_StreamGraph* sg);

/**
 * @brief Destroys a StreamGraph.
 * @param[in] sg The StreamGraph to destroy.
 */
void SGA_StreamGraph_destroy(SGA_StreamGraph sg);

/**
 * @brief Converts an external format string to an internal format string. Both from version 1.0.0.
 * @param[in] format The string in external format to parse.
 * @return The string of the internal format
 */
String SGA_external_v_1_0_0_to_internal_v_1_0_0_format(const String* format);

// TRICK: StreamData is a blanket struct that serves as a placeholder for the union of all stream types
// (FullStreamGraph, LinkStream, ect...).
// Otherwise a circular dependency would be created since these files need stream.h.
// And if we used a void* pointer, we would lose type safety.

/**
 * @brief Blanket struct that serves as a placeholder for the union of all stream types (FullStreamGraph, LinkStream, etc...).
 * Otherwise a circular dependency would be created since these files need stream.h.
 * It is safe to cast this to the actual type of the Stream (which is known in the Stream struct).
 */
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
	enum StreamType {
		FULL_STREAM_GRAPH,  ///< A FullStreamGraph
		LINK_STREAM,	    ///< A LinkStream
		CHUNK_STREAM,	    ///< A ChunkStream
		CHUNK_STREAM_SMALL, ///< A ChunkStreamSmall
		TIMEFRAME_STREAM,   ///< A TimeFrameStream
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
		((SGA_Stream*)(stream))->cache.field.present = true;                                                                       \
		((SGA_Stream*)(stream))->cache.field.data    = value;

/**
 * @brief Reset the cache of a Stream, emptying it.
 * @param[out] stream The Stream to reset the cache of.
 */
void reset_cache(SGA_Stream* stream);

/**
 * @brief Get the lifespan of a StreamGraph.
 * @param[in] sg The StreamGraph to get the lifespan of.
 * @return The lifespan of the StreamGraph.
 */
SGA_Interval SGA_StreamGraph_lifespan(SGA_StreamGraph* sg);

/**
 * @brief Get the time scale of a StreamGraph.
 * @param[in] sg The StreamGraph to get the time scale of.
 * @return The time scale of the StreamGraph.
 */
size_t SGA_StreamGraph_time_scale(SGA_StreamGraph* sg);

/**
 * @brief Initialise the events table of a StreamGraph.
 * @param[out] sg The StreamGraph to initialise the events table of.
 */
void init_events_table(SGA_StreamGraph* sg);

/**
 * @brief Destroy the events table of a StreamGraph.
 * @param[out] sg The StreamGraph to destroy the events table of.
 */
void events_destroy(SGA_StreamGraph* sg);

#endif // SGA_INTERNAL

#endif // STREAM_H