#ifndef ARENA_H
#define ARENA_H

#include "generic_data_structures/arraylist.h"
#include "utils.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ARENA_SIZE 1024

/**
 * @ingroup INTERNAL_API
 * @{
 */

/**
 * @brief An chunk of memory to allocate stuff close to each other and with one allocation.
 */
typedef struct InnerArena {
	uint8_t* memory; ///< Pointer to the memory allocated for this arena
	size_t size;	 ///< Size of the memory allocated for this arena
	size_t capacity; ///< Capacity of the arena (how many bytes can be allocated before needing to allocate more)
} InnerArena;

/**
 * @brief An arena-like allocator made with a list of smaller arenas inside to be able to be dynamic in size
 */
typedef struct Arena {
	InnerArena* arenas;  ///< Array of arenas
	size_t nb_arenas;    ///< Size of the arenas array
	size_t capacity;     ///< Capacity of the arenas array (uninitialized inner arenas)
	size_t nb_allocated; ///< Number of inner arenas already allocated
} Arena;

/**
 * @brief Create an arena with a default size.
 * @return An initialized arena.
 */
Arena Arena_init();

/**
 * @brief Destroy an arena.
 * @param arena The arena to destroy.
 * Frees all the memory allocated for the arenas and clears the arenas array.
 */
void Arena_destroy(Arena arena);

/**
 * @brief Clear an arena.
 * @param arena The arena to clear.
 * All memory used by the arenas is now available, but not freed, so it can be reused.
 */
void Arena_clear(Arena* arena);

/**
 * @brief Allocate memory in an arena.
 * @param arena The arena to allocate memory in.
 * @param size The size of the memory to allocate.
 * @return A pointer to the allocated memory.
 */
void* Arena_alloc(Arena* arena, size_t size);

/** @} */

#endif // ARENA_H