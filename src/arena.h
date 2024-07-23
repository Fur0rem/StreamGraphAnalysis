#ifndef ARENA_H
#define ARENA_H

#include "vector.h"
#include <stdbool.h>
#include <stddef.h>

#define ARENA_SIZE 1024

typedef struct {
	void* data;
	size_t size_left;
} Arena;

void Arena_init(Arena* arena);
void Arena_destroy(Arena arena);
char* Arena_to_string(Arena* arena);
bool Arena_equals(Arena a, Arena b);

DefVector(Arena, Arena_destroy);

void* ArenaVector_alloc(ArenaVector* vector, size_t size);
ArenaVector ArenaVector_init();

#endif // ARENA_H