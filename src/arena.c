#include "arena.h"
#include "generic_data_structures/vector.h"
#include "utils.h"

InnerArena InnerArena_init() {
	InnerArena arena = {
		.memory	  = MALLOC(ARENA_SIZE),
		.size	  = 0,
		.capacity = ARENA_SIZE,
	};
	return arena;
}

void InnerArena_destroy(InnerArena arena) {
	free(arena.memory);
}

void* InnerArena_alloc(InnerArena* arena, size_t size) {
	if (arena->capacity - arena->size < size) {
		return NULL;
	}
	void* ptr = arena->memory + arena->size;
	arena->size += size;
	return ptr;
}

Arena Arena_init() {
	Arena arena = {
		.arenas		  = MALLOC(sizeof(InnerArena) * 2),
		.nb_arenas	  = 1,
		.capacity	  = 2,
		.nb_allocated = 1,
	};
	arena.arenas[0] = InnerArena_init();
	return arena;
}

void Arena_destroy(Arena arena) {
	for (size_t i = 0; i < arena.nb_allocated; i++) {
		free(arena.arenas[i].memory);
	}
	free(arena.arenas);
}

void Arena_clear(Arena* arena) {
	for (size_t i = 0; i < arena->nb_arenas; i++) {
		arena->arenas[i].size = 0;
	}
	arena->nb_arenas = 0;
}

void* Arena_alloc(Arena* arena, size_t size) {
	// Use the first arena that has enough space
	for (size_t i = 0; i < arena->nb_arenas; i++) {
		if (arena->arenas[i].capacity - arena->arenas[i].size >= size) {
			void* ptr = arena->arenas[i].memory + arena->arenas[i].size;
			arena->arenas[i].size += size;
			return ptr;
		}
	}

	size_t index = arena->nb_arenas;
	arena->nb_arenas++;

	if (arena->nb_arenas > arena->capacity) {
		arena->capacity *= 2;
		arena->arenas = realloc(arena->arenas, sizeof(InnerArena) * arena->capacity);
	}

	if (arena->nb_arenas > arena->nb_allocated) {
		// initialize the new arena
		arena->arenas[index] = InnerArena_init();
		arena->nb_allocated++;
	}

	void* ptr = NULL;
	while (ptr == NULL) {
		ptr = InnerArena_alloc(&arena->arenas[index], size);
		if (ptr == NULL) {
			index++;
			if (index >= arena->nb_arenas) {
				index = 0;
			}
		}
	}

	return ptr;
}
