#ifndef VECTOR_H
#define VECTOR_H

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: maybe change push to push_back and array to data?
// TODO: add macro for elem in array?

// Dynamic array (vectors)
/*#define DefVector(type, freefunc) \
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
	static type type##Vector_pop_front(type##Vector* vec) {                                                            \
		type value = vec->array[0];                                                                                    \
		for (size_t i = 0; i < vec->size - 1; i++) {                                                                   \
			vec->array[i] = vec->array[i + 1];                                                                         \
		}                                                                                                              \
		vec->size--;                                                                                                   \
		return value;                                                                                                  \
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
		char* str = (char*)malloc(100000);                                                                             \
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
																													   \
	static size_t type##Vector_size(type##Vector* vec) {                                                               \
		return vec->size;                                                                                              \
	}                                                                                                                  \
																													   \
	static void type##Vector_sort(type##Vector* vec, int (*cmp)(const void*, const void*)) {                           \
		qsort(vec->array, vec->size, sizeof(type), cmp);                                                               \
	}                                                                                                                  \
																													   \
	static type* type##Vector_get(type##Vector* vec, size_t idx) {                                                     \
		return &vec->array[idx];                                                                                       \
	}                                                                                                                  \
																													   \
	static void type##Vector_reverse(type##Vector* vec) {                                                              \
		for (size_t i = 0; i < vec->size / 2; i++) {                                                                   \
			type tmp = vec->array[i];                                                                                  \
			vec->array[i] = vec->array[vec->size - i - 1];                                                             \
			vec->array[vec->size - i - 1] = tmp;                                                                       \
		}                                                                                                              \
	}

// TODO : dynamic str for to_string (crash if too long)
*/

/// Declare a vector of a given type, no other functions are defined
#define DeclareVector(type)                                                                                            \
	typedef struct {                                                                                                   \
		type* array;                                                                                                   \
		size_t size;                                                                                                   \
		size_t capacity;                                                                                               \
	} type##Vector;                                                                                                    \
                                                                                                                       \
	type##Vector type##Vector_with_capacity(size_t capacity);                                                          \
	type##Vector type##Vector_new();                                                                                   \
	void type##Vector_push(type##Vector* vec, type value);                                                             \
	void type##Vector_push_unchecked(type##Vector* vec, type value);                                                   \
	type* type##Vector_get(type##Vector* vec, size_t idx);                                                             \
	size_t type##Vector_len(type##Vector* vec);                                                                        \
	bool type##Vector_is_empty(type##Vector vec);                                                                      \
	void type##Vector_append(type##Vector* vec, const type* values, size_t nb_values);                                 \
	void type##Vector_reverse(type##Vector* vec);                                                                      \
	type type##Vector_pop_first(type##Vector* vec);                                                                    \
	type type##Vector_pop_last(type##Vector* vec);

#define DefineVector(type)                                                                                             \
                                                                                                                       \
	type##Vector type##Vector_with_capacity(size_t capacity) {                                                         \
		type##Vector vec = {.array = (type*)MALLOC(sizeof(type) * capacity), .size = 0, .capacity = capacity};         \
		return vec;                                                                                                    \
	}                                                                                                                  \
                                                                                                                       \
	type##Vector type##Vector_new() {                                                                                  \
		return type##Vector_with_capacity(2);                                                                          \
	}                                                                                                                  \
                                                                                                                       \
	void type##Vector_push(type##Vector* vec, type value) {                                                            \
		if (vec->size == vec->capacity) {                                                                              \
			vec->capacity *= 2;                                                                                        \
			type* new_array = (type*)MALLOC(sizeof(type) * vec->capacity);                                             \
			memcpy(new_array, vec->array, sizeof(type) * vec->size);                                                   \
			free(vec->array);                                                                                          \
			vec->array = new_array;                                                                                    \
		}                                                                                                              \
		vec->array[vec->size] = value;                                                                                 \
		vec->size++;                                                                                                   \
	}                                                                                                                  \
                                                                                                                       \
	void type##Vector_push_unchecked(type##Vector* vec, type value) {                                                  \
		ASSERT(vec->size < vec->capacity);                                                                             \
		vec->array[vec->size] = value;                                                                                 \
		vec->size++;                                                                                                   \
	}                                                                                                                  \
                                                                                                                       \
	type* type##Vector_get(type##Vector* vec, size_t idx) {                                                            \
		ASSERT(idx < vec->size);                                                                                       \
		return &vec->array[idx];                                                                                       \
	}                                                                                                                  \
                                                                                                                       \
	size_t type##Vector_len(type##Vector* vec) {                                                                       \
		return vec->size;                                                                                              \
	}                                                                                                                  \
                                                                                                                       \
	bool type##Vector_is_empty(type##Vector vec) {                                                                     \
		return vec.size == 0;                                                                                          \
	}                                                                                                                  \
                                                                                                                       \
	void type##Vector_reverse(type##Vector* vec) {                                                                     \
		for (size_t i = 0; i < vec->size / 2; i++) {                                                                   \
			type tmp					  = vec->array[i];                                                             \
			vec->array[i]				  = vec->array[vec->size - i - 1];                                             \
			vec->array[vec->size - i - 1] = tmp;                                                                       \
		}                                                                                                              \
	}                                                                                                                  \
                                                                                                                       \
	void type##Vector_append(type##Vector* vec, const type* values, size_t nb_values) {                                \
		if (vec->size + nb_values > vec->capacity) {                                                                   \
			vec->capacity	= vec->size + nb_values;                                                                   \
			type* new_array = (type*)malloc(sizeof(type) * vec->capacity);                                             \
			memcpy(new_array, vec->array, sizeof(type) * vec->size);                                                   \
			free(vec->array);                                                                                          \
			vec->array = new_array;                                                                                    \
		}                                                                                                              \
		memcpy(vec->array + vec->size, values, sizeof(type) * nb_values);                                              \
		vec->size += nb_values;                                                                                        \
	}                                                                                                                  \
                                                                                                                       \
	type type##Vector_pop_first(type##Vector* vec) {                                                                   \
		ASSERT(vec->size > 0);                                                                                         \
		type value = vec->array[0];                                                                                    \
		for (size_t i = 0; i < vec->size - 1; i++) {                                                                   \
			vec->array[i] = vec->array[i + 1];                                                                         \
		}                                                                                                              \
		vec->size--;                                                                                                   \
		return value;                                                                                                  \
	}                                                                                                                  \
                                                                                                                       \
	type type##Vector_pop_last(type##Vector* vec) {                                                                    \
		ASSERT(vec->size > 0);                                                                                         \
		vec->size--;                                                                                                   \
		return vec->array[vec->size];                                                                                  \
	}

/// Derives functions to the vector to remove elements, given a way to free the elements
#define DeclareVectorDeriveRemove(type)                                                                                \
	void type##Vector_remove_and_swap(type##Vector* vec, size_t idx);                                                  \
	void type##Vector_remove(type##Vector* vec, size_t idx);                                                           \
	void type##Vector_destroy(type##Vector vec);                                                                       \
	void type##Vector_clear(type##Vector* vec);

#define DefineVectorDeriveRemove(type, free_fn)                                                                        \
                                                                                                                       \
	void type##Vector_remove_and_swap(type##Vector* vec, size_t idx) {                                                 \
		if (free_fn != NULL) {                                                                                         \
			free_fn(vec->array[idx]);                                                                                  \
		}                                                                                                              \
		vec->array[idx] = vec->array[vec->size - 1];                                                                   \
		vec->size--;                                                                                                   \
	}                                                                                                                  \
                                                                                                                       \
	void type##Vector_remove(type##Vector* vec, size_t idx) {                                                          \
		if (free_fn != NULL) {                                                                                         \
			free_fn(vec->array[idx]);                                                                                  \
		}                                                                                                              \
		for (size_t i = idx; i < vec->size - 1; i++) {                                                                 \
			vec->array[i] = vec->array[i + 1];                                                                         \
		}                                                                                                              \
		vec->size--;                                                                                                   \
	}                                                                                                                  \
                                                                                                                       \
	void type##Vector_destroy(type##Vector vec) {                                                                      \
		if (free_fn != NULL) {                                                                                         \
			for (size_t i = 0; i < vec.size; i++) {                                                                    \
				free_fn(vec.array[i]);                                                                                 \
			}                                                                                                          \
		}                                                                                                              \
		free(vec.array);                                                                                               \
	}                                                                                                                  \
                                                                                                                       \
	void type##Vector_clear(type##Vector* vec) {                                                                       \
		if (free_fn != NULL) {                                                                                         \
			for (size_t i = 0; i < vec->size; i++) {                                                                   \
				free_fn(vec->array[i]);                                                                                \
			}                                                                                                          \
		}                                                                                                              \
		vec->size = 0;                                                                                                 \
	}

/// Derives functions to the vector, if the elements can be said equal or not equal
/// You should prefer using VectorDeriveCompare if all of the elements can be full ordered
#define DeclareVectorDeriveEquals(type)                                                                                \
	bool type##Vector_equals(type##Vector* vec1, type##Vector* vec2);                                                  \
	size_t type##Vector_find(type##Vector vec, type value);                                                            \
	bool type##Vector_contains(type##Vector vec, type value);

#define DefineVectorDeriveEquals(type)                                                                                 \
                                                                                                                       \
	bool type##Vector_equals(type##Vector* vec1, type##Vector* vec2) {                                                 \
		if (vec1->size != vec2->size) {                                                                                \
			return false;                                                                                              \
		}                                                                                                              \
		for (size_t i = 0; i < vec1->size; i++) {                                                                      \
			if (!type##_equals(&vec1->array[i], &vec2->array[i])) {                                                    \
				return false;                                                                                          \
			}                                                                                                          \
		}                                                                                                              \
		return true;                                                                                                   \
	}                                                                                                                  \
                                                                                                                       \
	size_t type##Vector_find(type##Vector vec, type value) {                                                           \
		for (size_t i = 0; i < vec.size; i++) {                                                                        \
			if (type##_equals(&vec.array[i], &value)) {                                                                \
				return i;                                                                                              \
			}                                                                                                          \
		}                                                                                                              \
		return vec.size;                                                                                               \
	}                                                                                                                  \
                                                                                                                       \
	bool type##Vector_contains(type##Vector vec, type value) {                                                         \
		return type##Vector_find(vec, value) < vec.size;                                                               \
	}

/// Derives functions to the vector, if the elements can be compared
/// This is a superset of VectorDeriveEquals, so you should prefer this one if you need both
#define DeclareVectorDeriveOrdered(type) void type##Vector_sort(type##Vector* vec);

#define DefineVectorDeriveOrdered(type)                                                                                \
	void type##Vector_sort(type##Vector* vec) {                                                                        \
		qsort(vec->array, vec->size, sizeof(type), (int (*)(const void*, const void*))type##_compare);                 \
	}

#define DeclareVectorDeriveToString(type) String type##Vector_to_string(type##Vector* vec);

#define DefineVectorDeriveToString(type)                                                                               \
	String type##Vector_to_string(type##Vector* vec) {                                                                 \
		String str = String_from_duplicate("[ ");                                                                      \
		for (size_t i = 0; i < vec->size; i++) {                                                                       \
			String elem_str = type##_to_string(&vec->array[i]);                                                        \
			String_concat_copy(&str, &elem_str);                                                                       \
			String_destroy(elem_str);                                                                                  \
			if (i < vec->size - 1) {                                                                                   \
				String_push_str(&str, ", ");                                                                           \
			}                                                                                                          \
		}                                                                                                              \
		String_push_str(&str, " ]");                                                                                   \
		return str;                                                                                                    \
	}

#endif // VECTOR_H