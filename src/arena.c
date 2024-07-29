#include "arena.h"
#include "utils.h"

void Arena_init(Arena* arena) {
	arena->data = MALLOC(ARENA_SIZE);
	arena->size_left = ARENA_SIZE;
}

void* Arena_alloc(Arena* arena, size_t size) {
	if (size > arena->size_left) {
		return NULL;
	}
	void* ptr = arena->data + ARENA_SIZE - arena->size_left;
	arena->size_left -= size;
	return ptr;
}

void Arena_destroy(Arena arena) {
	free(arena.data);
}
char* Arena_to_string(Arena* arena) {
	char* str = MALLOC(32);
	snprintf(str, 32, "Arena(%p, %lu)", arena->data, arena->size_left);
	return str;
}

bool Arena_equals(Arena a, Arena b) {
	return a.data == b.data && a.size_left == b.size_left;
}

ArenaVector ArenaVector_init() {
	ArenaVector vector = ArenaVector_with_capacity(5);
	Arena first;
	Arena_init(&first);
	ArenaVector_push(&vector, first);
	return vector;
}

void* ArenaVector_alloc(ArenaVector* vector, size_t size) {
	void* ptr = Arena_alloc(&vector->array[vector->size - 1], size);
	if (ptr == NULL) {
		Arena new_arena;
		Arena_init(&new_arena);
		ArenaVector_push(vector, new_arena);
		ptr = Arena_alloc(&vector->array[vector->size - 1], size);
	}
	return ptr;
}