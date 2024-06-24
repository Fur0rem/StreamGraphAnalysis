#ifndef ITERATORS_H
#define ITERATORS_H

#include "interval.h"
#include "stream.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
	Stream stream_graph;
	void* iterator_data;
	size_t (*next)(void*);
	void (*destroy)(void*);
	void (*skip_n)(void*, size_t);
} NodesIterator;

typedef struct {
	Stream stream_graph;
	void* iterator_data;
	size_t (*next)(void*);
	void (*destroy)(void*);
	void (*skip_n)(void*, size_t);
} LinksIterator;

typedef struct {
	Stream stream_graph;
	void* iterator_data;
	Interval (*next)(void*);
	void (*destroy)(void*);
	void (*skip_n)(void*, size_t);
} TimesIterator;

// The || ({ x.destroy(&x); 0; }) is a trick to execute the destroy function of the iterator when it ends
// The destroy function is only called when the previous condition is false, and then evaluates to 0 (false)
// This uses the GNU extension of "Statement Expressions"
#define FOR_EACH(type_iterated, iterated, iterator, end_cond)                                                          \
	for (type_iterated iterated = (iterator).next(&(iterator)); (end_cond) || ({                                       \
																	(iterator).destroy(&(iterator));                   \
																	0;                                                 \
																});                                                    \
		 (iterated) = (iterator).next(&(iterator)))

// TODO : Change the order of iterated and iterator in the FOR_EACH macro
#define FOR_EACH_NODE(iterator, iterated) FOR_EACH(size_t, iterated, iterator, (iterated) != SIZE_MAX)
#define FOR_EACH_LINK(iterator, iterated) FOR_EACH(size_t, iterated, iterator, (iterated) != SIZE_MAX)
#define FOR_EACH_TIME(iterator, iterated) FOR_EACH(Interval, iterated, iterator, (iterated).start != SIZE_MAX)

#define ITERATOR_IS_EMPTY(iterator)                                                                                    \
	({                                                                                                                 \
		size_t __tmp = (iterator).next(&(iterator));                                                                   \
		(iterator).destroy(&(iterator));                                                                               \
		__tmp == SIZE_MAX;                                                                                             \
	})

size_t total_time_of(TimesIterator times);

// TODO : Not very generic for links with other conditions of stop
#define COUNT_ITERATOR(iterator)                                                                                       \
	({                                                                                                                 \
		size_t count = 0;                                                                                              \
		FOR_EACH(size_t, iterated, iterator, (iterated) != SIZE_MAX) {                                                 \
			count++;                                                                                                   \
		}                                                                                                              \
		count;                                                                                                         \
	})

TimesIterator TimesIterator_union(TimesIterator a, TimesIterator b);
TimesIterator TimesIterator_intersection(TimesIterator a, TimesIterator b);

#endif // ITERATORS_H