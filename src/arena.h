#ifndef ARENA_H
#define ARENA_H

#include "utils.h"
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
DeclareToString(Arena);
DeclareEquals(Arena);

DeclareVector(Arena);
DeclareVectorDeriveRemove(Arena);
DeclareVectorDeriveEquals(Arena);
DeclareVectorDeriveToString(Arena);

void* ArenaVector_alloc(ArenaVector* vector, size_t size);
ArenaVector ArenaVector_init();

#endif // ARENA_H