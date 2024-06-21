#ifndef HASHSET_H
#define HASHSET_H

#include "utils.h"
#include "vector.h"
#include <stdio.h>

// Hashset
#define DefHashset(type, hashfunc, freefunc)                                                                           \
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
			s.buckets[i] = type##Vector_with_capacity(1);                                                              \
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
	}
#endif // HASHSET_H