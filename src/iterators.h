/**
 * @file src/iterators.h
 * @brief Iterators over nodes, links and time intervals.
 *
 * An iterator is a structure that allows to iterate over a set of data.
 * <br>
 * The iterators are lazy, meaning it does not load all the data at once, but only when needed.
 * We say an iterator is consumed when it has been iterated over all its elements and been destroyed.
 * <br>
 * There exists 3 types of iterators: for nodes, links and times.
 * However, they are very similar in their implementation.
 * Each iterator contains a reference to the stream, extra data it needs to iterate, a function to get the next element,
 * a function to destroy the iterator and a function to skip n elements.
 * Each type of stream can they define how to create an iterator for it, and each function can use them interchangeably.
 * <br>
 * A consumed iterator should not be used anymore.
 */

#ifndef SGA_ITERATORS_H
#define SGA_ITERATORS_H

#include "interval.h"
#include "stream.h"
#include "units.h"
#include <stddef.h>
#include <stdint.h>

/**
 * @brief The value returned by a times iterator when there are no more time intervals.
 */
// TODO: switch to LINK_MAX, TIME_MAX, NODE_MAX when it is defined
#define SGA_NODES_ITERATOR_END SIZE_MAX

/**
 * @brief An iterator over nodes.
 * Returns the id of the next node. If there are no more nodes, it returns NODES_ITERATOR_END.
 */
typedef struct SGA_NodesIterator SGA_NodesIterator;
struct SGA_NodesIterator {
	SGA_Stream stream_graph;		///< The stream graph from which the iterator is created.
	void* iterator_data;			///< Extra data needed to iterate.
	SGA_NodeId (*next)(SGA_NodesIterator*); ///< Function to get the next node id.
						///< If there are no more nodes, it returns NODES_ITERATOR_END.
						///< Takes itself as the argument.
	void (*destroy)(SGA_NodesIterator*);	///< Function to destroy the iterator.
						///< Takes itself as the argument.

	// void (*skip_n)(void*, size_t); // TODO : those
};

/**
 * @brief The value returned by a links iterator when there are no more links.
 */
#define SGA_LINKS_ITERATOR_END SIZE_MAX

/**
 * @brief An iterator over links.
 */
typedef struct SGA_LinksIterator SGA_LinksIterator;
struct SGA_LinksIterator {
	SGA_Stream stream_graph;		///< The stream graph from which the iterator is created.
	void* iterator_data;			///< Extra data needed to iterate.
	SGA_LinkId (*next)(SGA_LinksIterator*); ///< Function to get the next link id.
						///< If there are no more links, it returns LINKS_ITERATOR_END.
						///< Takes itself as the argument.
	void (*destroy)(SGA_LinksIterator*);	///< Function to destroy the iterator.
						///< Takes itself as the argument.

	// void (*skip_n)(void*, size_t); // TODO : those
};

/**
 * @brief The value returned by a times iterator when there are no more time intervals.
 */
#define SGA_TIMES_ITERATOR_END ((SGA_Interval){.start = SIZE_MAX, .end = 0})

/**
 * @brief An iterator over a set of time intervals.
 */
// TODO: rename to SGA_TimeIntervalsIterator
typedef struct SGA_TimesIterator SGA_TimesIterator;
struct SGA_TimesIterator {
	SGA_Stream stream_graph;		  ///< The stream graph from which the iterator is created.
	void* iterator_data;			  ///< Extra data needed to iterate.
	SGA_Interval (*next)(SGA_TimesIterator*); ///< Function to get the next time interval.
						  ///< Takes itself as the argument.
						  ///< If there are no more time intervals, it returns TIMES_ITERATOR_END.
	void (*destroy)(SGA_TimesIterator*);	  ///< Function to destroy the iterator.
					     ///< Takes itself as the argument.
};

/** @cond */
// TRICK : The || ({ x.destroy(&x); 0; }) executes the destroy function of the iterator when it ends
// The destroy function is only called when the previous condition is false, and then evaluates to 0 (false)
// This uses the GNU extension of "Statement Expressions"
#define SGA_FOR_EACH(type_iterated, iterated, iterator, end_cond)                                                                          \
	for (type_iterated iterated = (iterator).next(&(iterator)); (end_cond) || ({                                                       \
									    (iterator).destroy(&(iterator));                               \
									    0;                                                             \
								    });                                                                    \
	     (iterated)		    = (iterator).next(&(iterator)))
/** @endcond */

/**
 * @name Iteration macros
 * @brief Macros to iterate over nodes, links and times.
 *
 * The iterated variable becomes before the iterator in the macro call to mimic a for elem in list syntax.
 * Consumes the iterator.
 * @{
 */
#define SGA_FOR_EACH_NODE(iterated, iterator) SGA_FOR_EACH(SGA_NodeId, iterated, iterator, (iterated) != SGA_NODES_ITERATOR_END)
#define SGA_FOR_EACH_LINK(iterated, iterator) SGA_FOR_EACH(SGA_LinkId, iterated, iterator, (iterated) != SGA_LINKS_ITERATOR_END)
#define SGA_FOR_EACH_TIME(iterated, iterator)                                                                                              \
	SGA_FOR_EACH(SGA_Interval, iterated, iterator, !SGA_Interval_equals(&(iterated), &SGA_TIMES_ITERATOR_END))
/** @} */

/**
 * @brief Returns the total time of the given set of time intervals.

 * Consumes the iterator.
 * @param times The set of time intervals.
 * @return The total time of the set of time intervals.
 */
size_t SGA_total_time_of(SGA_TimesIterator times);

#ifdef SGA_INTERNAL
/** @cond */
#	define _COUNT_ITERATOR(type, iterated, iterator, end_cond)                                                                        \
		({                                                                                                                         \
			size_t count = 0;                                                                                                  \
			SGA_FOR_EACH(type, iterated, iterator, end_cond) {                                                                 \
				count++;                                                                                                   \
			}                                                                                                                  \
			count;                                                                                                             \
		})
/** @endcond */
#endif // SGA_INTERNAL

/**
 * @brief Counts the number of nodes in the given iterator.
 */
SGA_NodeId SGA_count_nodes(SGA_NodesIterator nodes);

/**
 * @brief Counts the number of links in the given iterator.
 */
SGA_LinkId SGA_count_links(SGA_LinksIterator links);

/**
 * @brief Counts the number of time intervals in the given iterator.
 */
SGA_TimeId SGA_count_intervals(SGA_TimesIterator times);

#ifdef SGA_INTERNAL
/**
 * @brief Counts the number of elements in the given iterator.
 *
 * Consumes the iterator.
 * @param iterator The iterator to count.
 * @return The number of elements in the iterator.
 */
#	define COUNT_ITERATOR(iterator)                                                                                                   \
		_Generic((iterator),                                                                                                       \
		    SGA_NodesIterator: SGA_count_nodes,                                                                                    \
		    SGA_LinksIterator: SGA_count_links,                                                                                    \
		    SGA_TimesIterator: SGA_count_intervals)(iterator)
#endif // SGA_INTERNAL

/**
 * @brief Creates an iterator over the union of two sets of time intervals.

 * Consumes both iterators.
 * @param a The first set of time intervals.
 * @param b The second set of time intervals.
 * @return The iterator over the union of the two sets of time intervals.
 */
SGA_TimesIterator SGA_TimesIterator_union(SGA_TimesIterator a, SGA_TimesIterator b);

/**
 * @brief Creates an iterator over the intersection of two sets of time intervals.

 * Consumes both iterators.
 * @param a The first set of time intervals.
 * @param b The second set of time intervals.
 * @return The iterator over the intersection of the two sets of time intervals.
 */
SGA_TimesIterator SGA_TimesIterator_intersection(SGA_TimesIterator a, SGA_TimesIterator b);

/**
 * @brief Collects all the time intervals of the given iterator into a arraylist.

 * Consumes the iterator.
 * @param times The iterator over time intervals.
 * @return A arraylist containing all the time intervals of the iterator.
 */
SGA_IntervalArrayList SGA_collect_times(SGA_TimesIterator times);

/**
 * @brief Collects all the nodes of the given iterator into a arraylist.

 * Consumes the iterator.
 * @param nodes The iterator over nodes.
 * @return A arraylist containing all the node ids of the iterator.
 */
SGA_NodeIdArrayList SGA_collect_node_ids(SGA_NodesIterator nodes);

/**
 * @brief Collects all the links of the given iterator into a arraylist.

 * Consumes the iterator.
 * @param links The iterator over links.
 * @return A arraylist containing all the link ids of the iterator.
 */
SGA_LinkIdArrayList SGA_collect_link_ids(SGA_LinksIterator links);

/**
 * @brief Breaks the iteration over the given iterator and destroys it.
 */
#define SGA_BREAK_ITERATOR(iterator)                                                                                                       \
	({                                                                                                                                 \
		(iterator).destroy(&(iterator));                                                                                           \
		break;                                                                                                                     \
	})

#endif // ITERATORS_H