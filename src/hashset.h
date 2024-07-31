#ifndef HASHSET_H
#define HASHSET_H

#include "utils.h"
#include "vector.h"
#include <stdio.h>

// Hashset
/*#define DefHashset(type, hashfunc, freefunc) \
																													   \
	DefVector(type, freefunc)                                                                                          \
																													   \
		typedef struct T_##type##Hashset {                                                                             \
		type##Vector* buckets;                                                                                         \
		size_t capacity;                                                                                               \
	} type##Hashset;                                                                                                   \
																													   \
	static type##Hashset type##Hashset_with_capacity(size_t capacity) {                                                \
		type##Hashset s = {.buckets = (type##Vector*)malloc(sizeof(type##Vector) * capacity), .capacity = capacity};   \
		for (size_t i = 0; i < capacity; i++) {                                                                        \
			s.buckets[i] = type##Vector_with_capacity(10);                                                             \
		}                                                                                                              \
		return s;                                                                                                      \
	}                                                                                                                  \
																													   \
	static void type##Hashset_destroy(type##Hashset s) {                                                               \
		for (size_t i = 0; i < s.capacity; i++) {                                                                      \
			type##Vector_destroy(s.buckets[i]);                                                                        \
		}                                                                                                              \
		free(s.buckets);                                                                                               \
	}                                                                                                                  \
																													   \
	static bool type##Hashset_insert(type##Hashset* s, type value) {                                                   \
		size_t index = hashfunc(value) % s->capacity;                                                                  \
		for (size_t i = 0; i < s->buckets[index].size; i++) {                                                          \
			if (type##_equals(s->buckets[index].array[i], value)) {                                                    \
				return false;                                                                                          \
			}                                                                                                          \
		}                                                                                                              \
		type##Vector_push(&s->buckets[index], value);                                                                  \
		return true;                                                                                                   \
	}                                                                                                                  \
																													   \
	static bool type##Hashset_contains(type##Hashset s, type value) {                                                  \
		size_t index = hashfunc(value) % s.capacity;                                                                   \
		for (size_t i = 0; i < s.buckets[index].size; i++) {                                                           \
			if (type##_equals(s.buckets[index].array[i], value)) {                                                     \
				return true;                                                                                           \
			}                                                                                                          \
		}                                                                                                              \
		return false;                                                                                                  \
	}                                                                                                                  \
																													   \
	static type* type##Hashset_find(type##Hashset s, type value) {                                                     \
		size_t index = hashfunc(value) % s.capacity;                                                                   \
		for (size_t i = 0; i < s.buckets[index].size; i++) {                                                           \
			if (type##_equals(s.buckets[index].array[i], value)) {                                                     \
				return &s.buckets[index].array[i];                                                                     \
			}                                                                                                          \
		}                                                                                                              \
		return NULL;                                                                                                   \
	}                                                                                                                  \
																													   \
	static bool type##Hashset_remove(type##Hashset* s, type value) {                                                   \
		size_t index = hashfunc(value) % s->capacity;                                                                  \
		for (size_t i = 0; i < s->buckets[index].size; i++) {                                                          \
			if (type##_equals(s->buckets[index].array[i], value)) {                                                    \
				type##Vector_remove_and_swap(&s->buckets[index], i);                                                   \
				return true;                                                                                           \
			}                                                                                                          \
		}                                                                                                              \
		return false;                                                                                                  \
	}                                                                                                                  \
																													   \
	static void type##Hashset_print_debug(type##Hashset* s) {                                                          \
		for (size_t i = 0; i < s->capacity; i++) {                                                                     \
			printf("%ld: (capacity : %ld, size : %ld)", i, s->buckets[i].capacity, s->buckets[i].size);                \
			for (size_t j = 0; j < s->buckets[i].size; j++) {                                                          \
				char* str = type##_to_string(&s->buckets[i].array[j]);                                                 \
				printf(" %s", str);                                                                                    \
				free(str);                                                                                             \
			}                                                                                                          \
			printf("\n");                                                                                              \
		}                                                                                                              \
	}                                                                                                                  \
																													   \
	static void type##Hashset_print(type##Hashset* s) {                                                                \
		for (size_t i = 0; i < s->capacity; i++) {                                                                     \
			if (s->buckets[i].size == 0) {                                                                             \
				continue;                                                                                              \
			}                                                                                                          \
			for (size_t j = 0; j < s->buckets[i].size; j++) {                                                          \
				char* str = type##_to_string(&s->buckets[i].array[j]);                                                 \
				printf("%s ", str);                                                                                    \
				free(str);                                                                                             \
			}                                                                                                          \
		}                                                                                                              \
		printf("\n");                                                                                                  \
	}                                                                                                                  \
																													   \
	static char* type##Hashset_to_string(type##Hashset* s) {                                                           \
		char* str = (char*)malloc(1);                                                                                  \
		str[0] = '\0';                                                                                                 \
		for (size_t i = 0; i < s->capacity; i++) {                                                                     \
			if (s->buckets[i].size == 0) {                                                                             \
				continue;                                                                                              \
			}                                                                                                          \
			for (size_t j = 0; j < s->buckets[i].size; j++) {                                                          \
				char* str2 = type##_to_string(&s->buckets[i].array[j]);                                                \
				str = (char*)realloc(str, strlen(str) + strlen(str2) + 2);                                             \
				strcat(str, str2);                                                                                     \
				strcat(str, " ");                                                                                      \
				free(str2);                                                                                            \
			}                                                                                                          \
		}                                                                                                              \
		return str;                                                                                                    \
	}                                                                                                                  \
																													   \
	static size_t type##Hashset_size(type##Hashset* s) {                                                               \
		size_t size = 0;                                                                                               \
		for (size_t i = 0; i < s->capacity; i++) {                                                                     \
			size += s->buckets[i].size;                                                                                \
		}                                                                                                              \
		return size;                                                                                                   \
	}*/

#define DeclareHashset(type)                                                                                           \
                                                                                                                       \
	typedef struct T_##type##Hashset {                                                                                 \
		type##Vector* buckets;                                                                                         \
		size_t capacity;                                                                                               \
	} type##Hashset;                                                                                                   \
                                                                                                                       \
	type##Hashset type##Hashset_with_capacity(size_t capacity);                                                        \
	bool type##Hashset_insert(type##Hashset* s, type value);                                                           \
	bool type##Hashset_contains(type##Hashset s, type value);                                                          \
	size_t type##Hashset_nb_elems(type##Hashset* s);

#define DefineHashset(type)                                                                                            \
                                                                                                                       \
	type##Hashset type##Hashset_with_capacity(size_t capacity) {                                                       \
		type##Hashset s = {.buckets = (type##Vector*)malloc(sizeof(type##Vector) * capacity), .capacity = capacity};   \
		for (size_t i = 0; i < capacity; i++) {                                                                        \
			s.buckets[i] = type##Vector_with_capacity(10);                                                             \
		}                                                                                                              \
		return s;                                                                                                      \
	}                                                                                                                  \
                                                                                                                       \
	bool type##Hashset_insert(type##Hashset* s, type value) {                                                          \
		size_t index = type##_hash(&value) % s->capacity;                                                              \
		for (size_t i = 0; i < s->buckets[index].size; i++) {                                                          \
			if (type##_equals(&s->buckets[index].array[i], &value)) {                                                  \
				return false;                                                                                          \
			}                                                                                                          \
		}                                                                                                              \
		type##Vector_push(&s->buckets[index], value);                                                                  \
		return true;                                                                                                   \
	}                                                                                                                  \
                                                                                                                       \
	bool type##Hashset_contains(type##Hashset s, type value) {                                                         \
		size_t index = type##_hash(&value) % s.capacity;                                                               \
		for (size_t i = 0; i < s.buckets[index].size; i++) {                                                           \
			if (type##_equals(&s.buckets[index].array[i], &value)) {                                                   \
				return true;                                                                                           \
			}                                                                                                          \
		}                                                                                                              \
		return false;                                                                                                  \
	}                                                                                                                  \
                                                                                                                       \
	size_t type##Hashset_nb_elems(type##Hashset* s) {                                                                  \
		size_t size = 0;                                                                                               \
		for (size_t i = 0; i < s->capacity; i++) {                                                                     \
			size += s->buckets[i].size;                                                                                \
		}                                                                                                              \
		return size;                                                                                                   \
	}                                                                                                                  \
                                                                                                                       \
	const type* type##Hashset_find(type##Hashset s, type value) {                                                      \
		size_t index = type##_hash(&value) % s.capacity;                                                               \
		for (size_t i = 0; i < s.buckets[index].size; i++) {                                                           \
			if (type##_equals(&s.buckets[index].array[i], &value)) {                                                   \
				return &s.buckets[index].array[i];                                                                     \
			}                                                                                                          \
		}                                                                                                              \
		return NULL;                                                                                                   \
	}

// DeclareVectorDeriveEquals(type)
// DeclareVectorDeriveCompare(type)
// DeclareVectorDeriveToString(type)
// DeclareVectorDeriveRemove(type, free_fn)
#define DeclareHashsetDeriveRemove(type, free_fn)                                                                      \
	void type##Hashset_destroy(type##Hashset s);                                                                       \
	bool type##Hashset_remove(type##Hashset* s, type value);

#define DefineHashsetDeriveRemove(type, free_fn)                                                                       \
	void type##Hashset_destroy(type##Hashset s) {                                                                      \
		for (size_t i = 0; i < s.capacity; i++) {                                                                      \
			type##Vector_destroy(s.buckets[i]);                                                                        \
		}                                                                                                              \
		free(s.buckets);                                                                                               \
	}                                                                                                                  \
                                                                                                                       \
	bool type##Hashset_remove(type##Hashset* s, type value) {                                                          \
		size_t index = type##_hash(&value) % s->capacity;                                                              \
		for (size_t i = 0; i < s->buckets[index].size; i++) {                                                          \
			if (type##_equals(&s->buckets[index].array[i], &value)) {                                                  \
				type##Vector_remove_and_swap(&s->buckets[index], i);                                                   \
				return true;                                                                                           \
			}                                                                                                          \
		}                                                                                                              \
		return false;                                                                                                  \
	}

#define DeclareHashsetDeriveToString(type) String type##Hashset_to_string(const type##Hashset* s);
#define DefineHashsetDeriveToString(type)                                                                              \
	String type##Hashset_to_string(const type##Hashset* s) {                                                           \
		String str = String_from_duplicate("{ ");                                                                      \
		for (size_t i = 0; i < s->capacity; i++) {                                                                     \
			if (s->buckets[i].size == 0) {                                                                             \
				continue;                                                                                              \
			}                                                                                                          \
			for (size_t j = 0; j < s->buckets[i].size; j++) {                                                          \
				String str2 = type##_to_string(&s->buckets[i].array[j]);                                               \
				String_concat(&str, &str2);                                                                            \
				String_push(&str, ' ');                                                                                \
				String_destroy(str2);                                                                                  \
			}                                                                                                          \
		}                                                                                                              \
		String_push(&str, '}');                                                                                        \
		return str;                                                                                                    \
	}

#define DeclareHashsetDeriveEquals(type) bool type##Hashset_equals(type##Hashset* a, type##Hashset* b);
#define DefineHashsetDeriveEquals(type)                                                                                \
	bool type##Hashset_equals(type##Hashset* a, type##Hashset* b) {                                                    \
		for (size_t i = 0; i < a->capacity; i++) {                                                                     \
			for (size_t j = 0; j < a->buckets[i].size; j++) {                                                          \
				if (!type##Hashset_contains(*b, a->buckets[i].array[j])) {                                             \
					return false;                                                                                      \
				}                                                                                                      \
			}                                                                                                          \
		}                                                                                                              \
		for (size_t i = 0; i < b->capacity; i++) {                                                                     \
			for (size_t j = 0; j < b->buckets[i].size; j++) {                                                          \
				if (!type##Hashset_contains(*a, b->buckets[i].array[j])) {                                             \
					return false;                                                                                      \
				}                                                                                                      \
			}                                                                                                          \
		}                                                                                                              \
		return true;                                                                                                   \
	}

#endif // HASHSET_H