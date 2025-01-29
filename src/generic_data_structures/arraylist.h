#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include "../utils.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: maybe change push to push_back and array to elems?
// TODO: add macro for elem in array?

/// Declare a ArrayList of a given type, no other functions are defined
#define DeclareArrayList(type)                                                                                                             \
	typedef struct {                                                                                                                   \
		type* array;                                                                                                               \
		size_t length;                                                                                                             \
		size_t capacity;                                                                                                           \
	} type##ArrayList;                                                                                                                 \
                                                                                                                                           \
	type##ArrayList type##ArrayList_with_capacity(size_t capacity);                                                                    \
	type##ArrayList type##ArrayList_new();                                                                                             \
	type##ArrayList type##ArrayList_from(size_t nb_elems, ...);                                                                        \
	type##ArrayList type##ArrayList_from_c_array_owned(size_t nb_elems, type* array);                                                  \
	type##ArrayList type##ArrayList_from_c_array_cloned(size_t nb_elems, const type* array);                                           \
	type##ArrayList type##ArrayList_clone(const type##ArrayList* list);                                                                \
	type##ArrayList type##ArrayList_empty();                                                                                           \
	void type##ArrayList_push(type##ArrayList* list, type value);                                                                      \
	void type##ArrayList_push_unchecked(type##ArrayList* list, type value);                                                            \
	size_t type##ArrayList_len(const type##ArrayList* list);                                                                           \
	bool type##ArrayList_is_empty(const type##ArrayList list);                                                                         \
	void type##ArrayList_append(type##ArrayList* list, const type* values, size_t nb_values);                                          \
	void type##ArrayList_reverse(type##ArrayList* list);                                                                               \
	type type##ArrayList_pop_first(type##ArrayList* list);                                                                             \
	type type##ArrayList_pop_last(type##ArrayList* list);                                                                              \
	type type##ArrayList_pop_nth(type##ArrayList* list, size_t idx);                                                                   \
	type type##ArrayList_pop_nth_swap(type##ArrayList* list, size_t idx);                                                              \
	void type##ArrayList_swap(type##ArrayList* list, size_t idx1, size_t idx2);

#define DefineArrayList(type)                                                                                                              \
                                                                                                                                           \
	type##ArrayList type##ArrayList_with_capacity(size_t capacity) {                                                                   \
		type##ArrayList list = {.array = (type*)MALLOC(sizeof(type) * capacity), .length = 0, .capacity = capacity};               \
		return list;                                                                                                               \
	}                                                                                                                                  \
                                                                                                                                           \
	type##ArrayList type##ArrayList_new() {                                                                                            \
		return type##ArrayList_with_capacity(10);                                                                                  \
	}                                                                                                                                  \
                                                                                                                                           \
	type##ArrayList type##ArrayList_empty() {                                                                                          \
		return type##ArrayList_with_capacity(0);                                                                                   \
	}                                                                                                                                  \
                                                                                                                                           \
	type##ArrayList type##ArrayList_from(size_t nb_elems, ...) {                                                                       \
		va_list args;                                                                                                              \
		va_start(args, nb_elems);                                                                                                  \
		type##ArrayList list = type##ArrayList_with_capacity(nb_elems);                                                            \
		for (size_t i = 0; i < nb_elems; i++) {                                                                                    \
			type value = va_arg(args, type);                                                                                   \
			type##ArrayList_push_unchecked(&list, value);                                                                      \
		}                                                                                                                          \
		va_end(args);                                                                                                              \
		return list;                                                                                                               \
	}                                                                                                                                  \
                                                                                                                                           \
	void type##ArrayList_push(type##ArrayList* list, type value) {                                                                     \
		if (list->length == list->capacity) {                                                                                      \
			list->capacity += (list->capacity / 2) + 1;                                                                        \
			type* new_list = (type*)MALLOC(sizeof(type) * list->capacity);                                                     \
			memcpy(new_list, list->array, sizeof(type) * list->length);                                                        \
			free(list->array);                                                                                                 \
			list->array = new_list;                                                                                            \
		}                                                                                                                          \
		list->array[list->length] = value;                                                                                         \
		list->length++;                                                                                                            \
	}                                                                                                                                  \
                                                                                                                                           \
	void type##ArrayList_push_unchecked(type##ArrayList* list, type value) {                                                           \
		ASSERT(list->length < list->capacity);                                                                                     \
		list->array[list->length] = value;                                                                                         \
		list->length++;                                                                                                            \
	}                                                                                                                                  \
                                                                                                                                           \
	size_t type##ArrayList_len(const type##ArrayList* list) {                                                                          \
		return list->length;                                                                                                       \
	}                                                                                                                                  \
                                                                                                                                           \
	bool type##ArrayList_is_empty(const type##ArrayList list) {                                                                        \
		return list.length == 0;                                                                                                   \
	}                                                                                                                                  \
                                                                                                                                           \
	type##ArrayList type##ArrayList_clone(const type##ArrayList* list) {                                                               \
		type##ArrayList new_list = type##ArrayList_with_capacity(list->length);                                                    \
		memcpy(new_list.array, list->array, sizeof(type) * list->length);                                                          \
		new_list.length = list->length;                                                                                            \
		return new_list;                                                                                                           \
	}                                                                                                                                  \
                                                                                                                                           \
	void type##ArrayList_reverse(type##ArrayList* list) {                                                                              \
		for (size_t i = 0; i < list->length / 2; i++) {                                                                            \
			type tmp			  = list->array[i];                                                                \
			list->array[i]			  = list->array[list->length - i - 1];                                             \
			list->array[list->length - i - 1] = tmp;                                                                           \
		}                                                                                                                          \
	}                                                                                                                                  \
                                                                                                                                           \
	void type##ArrayList_append(type##ArrayList* list, const type* values, size_t nb_values) {                                         \
		if (list->length + nb_values > list->capacity) {                                                                           \
			list->capacity	= list->length + nb_values;                                                                        \
			type* new_array = (type*)malloc(sizeof(type) * list->capacity);                                                    \
			memcpy(new_array, list->array, sizeof(type) * list->length);                                                       \
			free(list->array);                                                                                                 \
			list->array = new_array;                                                                                           \
		}                                                                                                                          \
		memcpy(list->array + list->length, values, sizeof(type) * nb_values);                                                      \
		list->length += nb_values;                                                                                                 \
	}                                                                                                                                  \
                                                                                                                                           \
	type type##ArrayList_pop_first(type##ArrayList* list) {                                                                            \
		ASSERT(list->length > 0);                                                                                                  \
		type value = list->array[0];                                                                                               \
		for (size_t i = 0; i < list->length - 1; i++) {                                                                            \
			list->array[i] = list->array[i + 1];                                                                               \
		}                                                                                                                          \
		list->length--;                                                                                                            \
		return value;                                                                                                              \
	}                                                                                                                                  \
                                                                                                                                           \
	type type##ArrayList_pop_last(type##ArrayList* list) {                                                                             \
		ASSERT(list->length > 0);                                                                                                  \
		list->length--;                                                                                                            \
		return list->array[list->length];                                                                                          \
	}                                                                                                                                  \
                                                                                                                                           \
	type type##ArrayList_pop_nth(type##ArrayList* list, size_t idx) {                                                                  \
		ASSERT(idx < list->length);                                                                                                \
		type value = list->array[idx];                                                                                             \
		for (size_t i = idx; i < list->length - 1; i++) {                                                                          \
			list->array[i] = list->array[i + 1];                                                                               \
		}                                                                                                                          \
		list->length--;                                                                                                            \
		return value;                                                                                                              \
	}                                                                                                                                  \
                                                                                                                                           \
	type type##ArrayList_pop_nth_swap(type##ArrayList* list, size_t idx) {                                                             \
		ASSERT(idx < list->length);                                                                                                \
		type value	 = list->array[idx];                                                                                       \
		list->array[idx] = list->array[list->length - 1];                                                                          \
		list->length--;                                                                                                            \
		return value;                                                                                                              \
	}                                                                                                                                  \
                                                                                                                                           \
	void type##ArrayList_swap(type##ArrayList* list, size_t idx1, size_t idx2) {                                                       \
		ASSERT(idx1 < list->length);                                                                                               \
		ASSERT(idx2 < list->length);                                                                                               \
		type tmp	  = list->array[idx1];                                                                                     \
		list->array[idx1] = list->array[idx2];                                                                                     \
		list->array[idx2] = tmp;                                                                                                   \
	}                                                                                                                                  \
                                                                                                                                           \
	type##ArrayList type##ArrayList_from_c_array_owned(size_t nb_elems, type* array) {                                                 \
		return (type##ArrayList){                                                                                                  \
		    .array    = array,                                                                                                     \
		    .length   = nb_elems,                                                                                                  \
		    .capacity = nb_elems,                                                                                                  \
		};                                                                                                                         \
	}                                                                                                                                  \
                                                                                                                                           \
	type##ArrayList type##ArrayList_from_c_array_cloned(size_t nb_elems, const type* array) {                                          \
		type##ArrayList list = type##ArrayList_with_capacity(nb_elems);                                                            \
		type##ArrayList_append(&list, array, nb_elems);                                                                            \
		return list;                                                                                                               \
	}

/// Derives functions to the ArrayList to remove elements, given a way to free the elements
#define DeclareArrayListDeriveRemove(type)                                                                                                 \
	void type##ArrayList_remove_and_swap(type##ArrayList* list, size_t idx);                                                           \
	void type##ArrayList_remove(type##ArrayList* list, size_t idx);                                                                    \
	void type##ArrayList_destroy(type##ArrayList list);                                                                                \
	void type##ArrayList_clear(type##ArrayList* list);

#define DefineArrayListDeriveRemove(type)                                                                                                  \
                                                                                                                                           \
	void type##ArrayList_remove_and_swap(type##ArrayList* list, size_t idx) {                                                          \
		type##_destroy(list->array[idx]);                                                                                          \
		list->array[idx] = list->array[list->length - 1];                                                                          \
		list->length--;                                                                                                            \
	}                                                                                                                                  \
                                                                                                                                           \
	void type##ArrayList_remove(type##ArrayList* list, size_t idx) {                                                                   \
		type##_destroy(list->array[idx]);                                                                                          \
		for (size_t i = idx; i < list->length - 1; i++) {                                                                          \
			list->array[i] = list->array[i + 1];                                                                               \
		}                                                                                                                          \
		list->length--;                                                                                                            \
	}                                                                                                                                  \
                                                                                                                                           \
	void type##ArrayList_destroy(type##ArrayList list) {                                                                               \
		for (size_t i = 0; i < list.length; i++) {                                                                                 \
			type##_destroy(list.array[i]);                                                                                     \
		}                                                                                                                          \
		free(list.array);                                                                                                          \
	}                                                                                                                                  \
                                                                                                                                           \
	void type##ArrayList_clear(type##ArrayList* list) {                                                                                \
		for (size_t i = 0; i < list->length; i++) {                                                                                \
			type##_destroy(list->array[i]);                                                                                    \
		}                                                                                                                          \
		list->length = 0;                                                                                                          \
	}

/// Derives functions to the ArrayList, if the elements can be said equal or not equal
/// You should prefer using ArrayListDeriveCompare if all of the elements can be full ordered
#define DeclareArrayListDeriveEquals(type)                                                                                                 \
	bool type##ArrayList_equals(const type##ArrayList* list1, const type##ArrayList* list2);                                           \
	size_t type##ArrayList_find(const type##ArrayList list, type value);                                                               \
	bool type##ArrayList_contains(const type##ArrayList list, type value);

#define DefineArrayListDeriveEquals(type)                                                                                                  \
                                                                                                                                           \
	bool type##ArrayList_equals(const type##ArrayList* list1, const type##ArrayList* list2) {                                          \
		if (list1->length != list2->length) {                                                                                      \
			return false;                                                                                                      \
		}                                                                                                                          \
		for (size_t i = 0; i < list1->length; i++) {                                                                               \
			if (!type##_equals(&list1->array[i], &list2->array[i])) {                                                          \
				return false;                                                                                              \
			}                                                                                                                  \
		}                                                                                                                          \
		return true;                                                                                                               \
	}                                                                                                                                  \
                                                                                                                                           \
	size_t type##ArrayList_find(const type##ArrayList list, type value) {                                                              \
		for (size_t i = 0; i < list.length; i++) {                                                                                 \
			if (type##_equals(&list.array[i], &value)) {                                                                       \
				return i;                                                                                                  \
			}                                                                                                                  \
		}                                                                                                                          \
		return list.length;                                                                                                        \
	}                                                                                                                                  \
                                                                                                                                           \
	bool type##ArrayList_contains(const type##ArrayList list, type value) {                                                            \
		return type##ArrayList_find(list, value) < list.length;                                                                    \
	}

/// Derives functions to the ArrayList, if the elements can be compared
/// This is a superset of ArrayListDeriveEquals, so you should prefer this one if you need both
#define DeclareArrayListDeriveOrdered(type)                                                                                                \
	void type##ArrayList_sort_stable(type##ArrayList* list);                                                                           \
	void type##ArrayList_sort_unstable(type##ArrayList* list);

#define MIN(a, b) ((a) < (b) ? (a) : (b))

// TODO: inline a faster version of this
#define DefineArrayListDeriveOrdered(type)                                                                                                 \
	/** Timsort */                                                                                                                     \
	void type##ArrayList_insertion_sort(type##ArrayList* list, size_t left, size_t right) {                                            \
		for (size_t i = left + 1; i <= right; i++) {                                                                               \
			type key = list->array[i];                                                                                         \
			size_t j = i - 1;                                                                                                  \
			while (j >= left && type##_compare(&list->array[j], &key) > 0) {                                                   \
				list->array[j + 1] = list->array[j];                                                                       \
				j--;                                                                                                       \
			}                                                                                                                  \
			list->array[j + 1] = key;                                                                                          \
		}                                                                                                                          \
	}                                                                                                                                  \
                                                                                                                                           \
	void type##ArrayList_merge(type##ArrayList* list, size_t l, size_t m, size_t r) {                                                  \
		size_t len1 = m - l + 1;                                                                                                   \
		size_t len2 = r - m;                                                                                                       \
		type* L	    = (type*)MALLOC(sizeof(type) * len1);                                                                          \
		type* R	    = (type*)MALLOC(sizeof(type) * len2);                                                                          \
		memcpy(L, &list->array[l], sizeof(type) * len1);                                                                           \
		memcpy(R, &list->array[m + 1], sizeof(type) * len2);                                                                       \
		size_t i = 0;                                                                                                              \
		size_t j = 0;                                                                                                              \
		size_t k = l;                                                                                                              \
		while (i < len1 && j < len2) {                                                                                             \
			if (type##_compare(&L[i], &R[j]) <= 0) {                                                                           \
				list->array[k] = L[i];                                                                                     \
				i++;                                                                                                       \
			}                                                                                                                  \
			else {                                                                                                             \
				list->array[k] = R[j];                                                                                     \
				j++;                                                                                                       \
			}                                                                                                                  \
			k++;                                                                                                               \
		}                                                                                                                          \
		while (i < len1) {                                                                                                         \
			list->array[k] = L[i];                                                                                             \
			i++;                                                                                                               \
			k++;                                                                                                               \
		}                                                                                                                          \
		while (j < len2) {                                                                                                         \
			list->array[k] = R[j];                                                                                             \
			j++;                                                                                                               \
			k++;                                                                                                               \
		}                                                                                                                          \
		free(L);                                                                                                                   \
		free(R);                                                                                                                   \
	}                                                                                                                                  \
                                                                                                                                           \
	void type##ArrayList_sort_stable(type##ArrayList* list) {                                                                          \
		size_t n = list->length;                                                                                                   \
		for (size_t i = 0; i < n; i += 32) {                                                                                       \
			type##ArrayList_insertion_sort(list, i, MIN(i + 31, n - 1));                                                       \
		}                                                                                                                          \
		for (size_t size = 32; size < n; size = 2 * size) {                                                                        \
			for (size_t left = 0; left < n; left += 2 * size) {                                                                \
				size_t mid   = left + size - 1;                                                                            \
				size_t right = MIN(left + 2 * size - 1, n - 1);                                                            \
				type##ArrayList_merge(list, left, mid, right);                                                             \
			}                                                                                                                  \
		}                                                                                                                          \
	}                                                                                                                                  \
                                                                                                                                           \
	void type##ArrayList_sort_unstable(type##ArrayList* list) {                                                                        \
		qsort(list->array, list->length, sizeof(type), (int (*)(const void*, const void*))type##_compare);                         \
	}

#define DeclareArrayListDeriveToString(type) String type##ArrayList_to_string(const type##ArrayList* list);

#define DefineArrayListDeriveToString(type)                                                                                                \
	String type##ArrayList_to_string(const type##ArrayList* list) {                                                                    \
		String str = String_from_duplicate("[ ");                                                                                  \
		for (size_t i = 0; i < list->length; i++) {                                                                                \
			String elem_str = type##_to_string(&list->array[i]);                                                               \
			String_concat_copy(&str, &elem_str);                                                                               \
			String_destroy(elem_str);                                                                                          \
			if (i < list->length - 1) {                                                                                        \
				String_push_str(&str, ", ");                                                                               \
			}                                                                                                                  \
		}                                                                                                                          \
		String_push_str(&str, " ]");                                                                                               \
		return str;                                                                                                                \
	}

#endif // ARRAYLIST_H