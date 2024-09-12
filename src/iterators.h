#ifndef ITERATORS_H
#define ITERATORS_H

/**
 * @file iterators.h
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

#include "interval.h"
#include "stream.h"
#include <stddef.h>
#include <stdint.h>

/**
 * @brief An iterator over nodes.
 */
typedef struct NodesIterator NodesIterator;
struct NodesIterator {
	Stream stream_graph;
	void* iterator_data;
	size_t (*next)(NodesIterator*);
	void (*destroy)(NodesIterator*);
	void (*skip_n)(void*, size_t);
};

/**
 * @brief An iterator over links.
 */
typedef struct LinksIterator LinksIterator;
struct LinksIterator {
	Stream stream_graph;
	void* iterator_data;
	size_t (*next)(LinksIterator*);
	void (*destroy)(LinksIterator*);
	void (*skip_n)(void*, size_t); // TODO : those
};

/**
 * @brief An iterator over a set of time intervals.
 */
typedef struct TimesIterator TimesIterator;
struct TimesIterator {
	Stream stream_graph;
	void* iterator_data;
	Interval (*next)(TimesIterator*);
	void (*destroy)(TimesIterator*);
};

/** @cond */
// TRICK : The || ({ x.destroy(&x); 0; }) executes the destroy function of the iterator when it ends
// The destroy function is only called when the previous condition is false, and then evaluates to 0 (false)
// This uses the GNU extension of "Statement Expressions"
#define FOR_EACH(type_iterated, iterated, iterator, end_cond)                                                          \
	for (type_iterated iterated = (iterator).next(&(iterator)); (end_cond) || ({                                       \
																	(iterator).destroy(&(iterator));                   \
																	0;                                                 \
																});                                                    \
		 (iterated)				= (iterator).next(&(iterator)))
/** @endcond */

// TODO: rename to IntervalIterator probably
#define TIMES_ITERATOR_END ((Interval){.start = SIZE_MAX, .end = SIZE_MAX})
#define NODES_ITERATOR_END SIZE_MAX
#define LINKS_ITERATOR_END SIZE_MAX

/**
 * @name Iteration macros
 * @brief Macros to iterate over nodes, links and times.
 *
 * The iterated variable becomes before the iterator in the macro call to mimic a for elem in list syntax.
 * Consumes the iterator.
 * @{
 */
#define FOR_EACH_NODE(iterated, iterator) FOR_EACH(size_t, iterated, iterator, (iterated) != NODES_ITERATOR_END)
#define FOR_EACH_LINK(iterated, iterator) FOR_EACH(size_t, iterated, iterator, (iterated) != LINKS_ITERATOR_END)
#define FOR_EACH_TIME(iterated, iterator) FOR_EACH(Interval, iterated, iterator, (iterated).start != SIZE_MAX)
/** @} */

/**
 * @brief Returns the total time of the given set of time intervals.

 * Consumes the iterator.
 * @param times The set of time intervals.
 * @return The total time of the set of time intervals.
 */
size_t total_time_of(TimesIterator times);

/** @cond */
#define _COUNT_ITERATOR(type, iterated, iterator, end_cond)                                                            \
	({                                                                                                                 \
		size_t count = 0;                                                                                              \
		FOR_EACH(type, iterated, iterator, end_cond) {                                                                 \
			count++;                                                                                                   \
		}                                                                                                              \
		count;                                                                                                         \
	})
size_t count_nodes(NodesIterator nodes);
size_t count_links(LinksIterator links);
size_t count_times(TimesIterator times);
/** @endcond */

/**
 * @brief Counts the number of elements in the given iterator.
 *
 * Consumes the iterator.
 * @param iterator The iterator to count.
 * @return The number of elements in the iterator.
 */
#define COUNT_ITERATOR(iterator)                                                                                       \
	_Generic((iterator), NodesIterator: count_nodes, LinksIterator: count_links, TimesIterator: count_times)(iterator)

/**
 * @brief Creates an iterator over the union of two sets of time intervals.

 * Consumes both iterators.
 * @param a The first set of time intervals.
 * @param b The second set of time intervals.
 * @return The iterator over the union of the two sets of time intervals.
 */
TimesIterator TimesIterator_union(TimesIterator a, TimesIterator b);

/**
 * @brief Creates an iterator over the intersection of two sets of time intervals.

 * Consumes both iterators.
 * @param a The first set of time intervals.
 * @param b The second set of time intervals.
 * @return The iterator over the intersection of the two sets of time intervals.
 */
TimesIterator TimesIterator_intersection(TimesIterator a, TimesIterator b);

/**
 * @brief Collects all the time intervals of the given iterator into a vector.

 * Consumes the iterator.
 * @param times The iterator over time intervals.
 * @return A vector containing all the time intervals of the iterator.
 */
IntervalVector SGA_collect_times(TimesIterator times);

/**
 * @brief Collects all the nodes of the given iterator into a vector.

 * Consumes the iterator.
 * @param nodes The iterator over nodes.
 * @return A vector containing all the node ids of the iterator.
 */
NodeIdVector SGA_collect_node_ids(NodesIterator nodes);

/**
 * @brief Collects all the links of the given iterator into a vector.

 * Consumes the iterator.
 * @param links The iterator over links.
 * @return A vector containing all the link ids of the iterator.
 */
LinkIdVector SGA_collect_link_ids(LinksIterator links);

#define BREAK_ITER(iterator)                                                                                           \
	({                                                                                                                 \
		(iterator).destroy(&(iterator));                                                                               \
		break;                                                                                                         \
	})

#endif // ITERATORS_H