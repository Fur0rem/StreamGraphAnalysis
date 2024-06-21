#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

// Dynamic array (vectors)
#define DefVector(type, freefunc)                                                                                      \
                                                                                                                       \
	typedef struct T_##type##Vector {                                                                                  \
		type* array;                                                                                                   \
		size_t size;                                                                                                   \
		size_t capacity;                                                                                               \
	} type##Vector;                                                                                                    \
                                                                                                                       \
	static type##Vector type##Vector_with_capacity(size_t capacity) {                                                  \
		type##Vector vec = {.array = (type*)malloc(sizeof(type) * capacity), .size = 0, .capacity = capacity};         \
		return vec;                                                                                                    \
	}                                                                                                                  \
                                                                                                                       \
	static type##Vector type##Vector_new() {                                                                           \
		return type##Vector_with_capacity(2);                                                                          \
	}                                                                                                                  \
                                                                                                                       \
	static void type##Vector_destroy(type##Vector vec) {                                                               \
		if (freefunc) {                                                                                                \
			for (size_t i = 0; i < vec.size; i++) {                                                                    \
				freefunc(vec.array[i]);                                                                                \
			}                                                                                                          \
		}                                                                                                              \
		free(vec.array);                                                                                               \
	}                                                                                                                  \
                                                                                                                       \
	static void type##Vector_push(type##Vector* vec, type value) {                                                     \
		if (vec->size == vec->capacity) {                                                                              \
			vec->capacity *= 2;                                                                                        \
			type* new_array = (type*)malloc(sizeof(type) * vec->capacity);                                             \
			memcpy(new_array, vec->array, sizeof(type) * vec->size);                                                   \
			free(vec->array);                                                                                          \
			vec->array = new_array;                                                                                    \
		}                                                                                                              \
		vec->array[vec->size] = value;                                                                                 \
		vec->size++;                                                                                                   \
	}                                                                                                                  \
                                                                                                                       \
	static void type##Vector_pop(type##Vector* vec) {                                                                  \
		if (freefunc) {                                                                                                \
			freefunc(vec->array[vec->size - 1]);                                                                       \
		}                                                                                                              \
		vec->size--;                                                                                                   \
	}                                                                                                                  \
                                                                                                                       \
	static void type##Vector_append(type##Vector* vec, const type* values, size_t nb_values) {                         \
		if (vec->size + nb_values > vec->capacity) {                                                                   \
			vec->capacity = vec->size + nb_values;                                                                     \
			type* new_array = (type*)malloc(sizeof(type) * vec->capacity);                                             \
			memcpy(new_array, vec->array, sizeof(type) * vec->size);                                                   \
			free(vec->array);                                                                                          \
			vec->array = new_array;                                                                                    \
		}                                                                                                              \
		memcpy(vec->array + vec->size, values, sizeof(type) * nb_values);                                              \
		vec->size += nb_values;                                                                                        \
	}                                                                                                                  \
                                                                                                                       \
	static void type##Vector_remove_and_swap(type##Vector* vec, size_t idx) {                                          \
		if (freefunc) {                                                                                                \
			freefunc(vec->array[idx]);                                                                                 \
		}                                                                                                              \
		vec->array[idx] = vec->array[vec->size - 1];                                                                   \
		vec->size--;                                                                                                   \
	}                                                                                                                  \
                                                                                                                       \
	static void type##Vector_remove(type##Vector* vec, size_t idx) {                                                   \
		if (freefunc) {                                                                                                \
			freefunc(vec->array[idx]);                                                                                 \
		}                                                                                                              \
		for (size_t i = idx; i < vec->size - 1; i++) {                                                                 \
			vec->array[i] = vec->array[i + 1];                                                                         \
		}                                                                                                              \
		vec->size--;                                                                                                   \
	}                                                                                                                  \
                                                                                                                       \
	static bool type##Vector_is_empty(type##Vector vec) {                                                              \
		return vec.size == 0;                                                                                          \
	}                                                                                                                  \
                                                                                                                       \
	static size_t type##Vector_find(type##Vector vec, type value) {                                                    \
		for (size_t i = 0; i < vec.size; i++) {                                                                        \
			if (type##_equals(vec.array[i], value)) {                                                                  \
				return i;                                                                                              \
			}                                                                                                          \
		}                                                                                                              \
		return vec.size;                                                                                               \
	}                                                                                                                  \
                                                                                                                       \
	static bool type##Vector_contains(type##Vector vec, type value) {                                                  \
		return type##Vector_find(vec, value) < vec.size;                                                               \
	}                                                                                                                  \
                                                                                                                       \
	static char* type##Vector_to_string(type##Vector* vec) {                                                           \
		char* str = (char*)malloc(1000);                                                                               \
		str[0] = '\0';                                                                                                 \
		strcat(str, "[ ");                                                                                             \
		for (size_t i = 0; i < vec->size; i++) {                                                                       \
			char* elem_str = type##_to_string(&vec->array[i]);                                                         \
			strcat(str, elem_str);                                                                                     \
			free(elem_str);                                                                                            \
			if (i < vec->size - 1) {                                                                                   \
				strcat(str, ", ");                                                                                     \
			}                                                                                                          \
		}                                                                                                              \
		strcat(str, " ]");                                                                                             \
		return str;                                                                                                    \
	}                                                                                                                  \
                                                                                                                       \
	static bool type##Vector_equals(type##Vector vec1, type##Vector vec2) {                                            \
		if (vec1.size != vec2.size) {                                                                                  \
			return false;                                                                                              \
		}                                                                                                              \
		for (size_t i = 0; i < vec1.size; i++) {                                                                       \
			if (!type##_equals(vec1.array[i], vec2.array[i])) {                                                        \
				return false;                                                                                          \
			}                                                                                                          \
		}                                                                                                              \
		return true;                                                                                                   \
	}                                                                                                                  \
	static size_t type##Vector_size(type##Vector* vec) {                                                               \
		return vec->size;                                                                                              \
	}

#endif // VECTOR_H