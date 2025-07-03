#ifndef ARENA_H
#define ARENA_H

#include "generic_data_structures/arraylist.h"
#include "utils.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ARENA_SIZE 1024

/**
 * @defgroup testgroup Internal API
 * @brief Functions and types for internal use.
 * @{
 */

/** @} */ // end of testgroup

/**
 * @ingroup testgroup
 * @{
 */

typedef struct InnerArena {
	uint8_t* memory;
	size_t size;
	size_t capacity;
} InnerArena;

typedef struct Arena {
	InnerArena* arenas;  // array of arenas
	size_t nb_arenas;    // size of the arenas array
	size_t capacity;     // capacity of the arenas array (uninitialized inner arenas)
	size_t nb_allocated; // number of inner arenas already allocated
} Arena;

Arena Arena_init();
void Arena_destroy(Arena arena);
void Arena_clear(Arena* arena);

void* Arena_alloc(Arena* arena, size_t size);

/** @} */

#endif // ARENA_H