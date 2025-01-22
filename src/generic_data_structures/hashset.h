#ifndef HASHSET_H
#define HASHSET_H

#include "../utils.h"
#include "arraylist.h"
#include <stdio.h>

#define DeclareHashset(type)                                                                                                               \
                                                                                                                                           \
	typedef struct T_##type##Hashset {                                                                                                 \
		type##ArrayList* buckets;                                                                                                  \
		size_t capacity;                                                                                                           \
	} type##Hashset;                                                                                                                   \
                                                                                                                                           \
	type##Hashset type##Hashset_with_capacity(size_t capacity);                                                                        \
	bool type##Hashset_insert(type##Hashset* s, type value);                                                                           \
	bool type##Hashset_contains(type##Hashset s, type value);                                                                          \
	size_t type##Hashset_nb_elems(type##Hashset* s);

#define DefineHashset(type)                                                                                                                \
                                                                                                                                           \
	type##Hashset type##Hashset_with_capacity(size_t capacity) {                                                                       \
		type##Hashset s = {.buckets = (type##ArrayList*)malloc(sizeof(type##ArrayList) * capacity), .capacity = capacity};         \
		for (size_t i = 0; i < capacity; i++) {                                                                                    \
			s.buckets[i] = type##ArrayList_with_capacity(10);                                                                  \
		}                                                                                                                          \
		return s;                                                                                                                  \
	}                                                                                                                                  \
                                                                                                                                           \
	bool type##Hashset_insert(type##Hashset* s, type value) {                                                                          \
		size_t index = type##_hash(&value) % s->capacity;                                                                          \
		for (size_t i = 0; i < s->buckets[index].length; i++) {                                                                    \
			if (type##_equals(&s->buckets[index].array[i], &value)) {                                                          \
				return false;                                                                                              \
			}                                                                                                                  \
		}                                                                                                                          \
		type##ArrayList_push(&s->buckets[index], value);                                                                           \
		return true;                                                                                                               \
	}                                                                                                                                  \
                                                                                                                                           \
	bool type##Hashset_contains(type##Hashset s, type value) {                                                                         \
		size_t index = type##_hash(&value) % s.capacity;                                                                           \
		for (size_t i = 0; i < s.buckets[index].length; i++) {                                                                     \
			if (type##_equals(&s.buckets[index].array[i], &value)) {                                                           \
				return true;                                                                                               \
			}                                                                                                                  \
		}                                                                                                                          \
		return false;                                                                                                              \
	}                                                                                                                                  \
                                                                                                                                           \
	size_t type##Hashset_nb_elems(type##Hashset* s) {                                                                                  \
		size_t size = 0;                                                                                                           \
		for (size_t i = 0; i < s->capacity; i++) {                                                                                 \
			size += s->buckets[i].length;                                                                                      \
		}                                                                                                                          \
		return size;                                                                                                               \
	}                                                                                                                                  \
                                                                                                                                           \
	const type* type##Hashset_find(type##Hashset s, type value) {                                                                      \
		size_t index = type##_hash(&value) % s.capacity;                                                                           \
		for (size_t i = 0; i < s.buckets[index].length; i++) {                                                                     \
			if (type##_equals(&s.buckets[index].array[i], &value)) {                                                           \
				return &s.buckets[index].array[i];                                                                         \
			}                                                                                                                  \
		}                                                                                                                          \
		return NULL;                                                                                                               \
	}

// DeclareArrayListDeriveEquals(type)
// DeclareArrayListDeriveCompare(type)
// DeclareArrayListDeriveToString(type)
// DeclareArrayListDeriveRemove(type, free_fn)
#define DeclareHashsetDeriveRemove(type)                                                                                                   \
	void type##Hashset_destroy(type##Hashset s);                                                                                       \
	bool type##Hashset_remove(type##Hashset* s, type value);                                                                           \
	void type##Hashset_clear(type##Hashset* s);

#define DefineHashsetDeriveRemove(type)                                                                                                    \
	void type##Hashset_destroy(type##Hashset s) {                                                                                      \
		for (size_t i = 0; i < s.capacity; i++) {                                                                                  \
			type##ArrayList_destroy(s.buckets[i]);                                                                             \
		}                                                                                                                          \
		free(s.buckets);                                                                                                           \
	}                                                                                                                                  \
                                                                                                                                           \
	bool type##Hashset_remove(type##Hashset* s, type value) {                                                                          \
		size_t index = type##_hash(&value) % s->capacity;                                                                          \
		for (size_t i = 0; i < s->buckets[index].length; i++) {                                                                    \
			if (type##_equals(&s->buckets[index].array[i], &value)) {                                                          \
				type##ArrayList_remove_and_swap(&s->buckets[index], i);                                                    \
				return true;                                                                                               \
			}                                                                                                                  \
		}                                                                                                                          \
		return false;                                                                                                              \
	}                                                                                                                                  \
                                                                                                                                           \
	void type##Hashset_clear(type##Hashset* s) {                                                                                       \
		for (size_t i = 0; i < s->capacity; i++) {                                                                                 \
			type##ArrayList_clear(&s->buckets[i]);                                                                             \
		}                                                                                                                          \
	}

#define DeclareHashsetDeriveToString(type) String type##Hashset_to_string(const type##Hashset* s);
#define DefineHashsetDeriveToString(type)                                                                                                  \
	String type##Hashset_to_string(const type##Hashset* s) {                                                                           \
		String str = String_from_duplicate("{ ");                                                                                  \
		for (size_t i = 0; i < s->capacity; i++) {                                                                                 \
			if (s->buckets[i].length == 0) {                                                                                   \
				continue;                                                                                                  \
			}                                                                                                                  \
			for (size_t j = 0; j < s->buckets[i].length; j++) {                                                                \
				String str2 = type##_to_string(&s->buckets[i].array[j]);                                                   \
				String_concat_copy(&str, &str2);                                                                           \
				String_push(&str, ' ');                                                                                    \
				String_destroy(str2);                                                                                      \
			}                                                                                                                  \
		}                                                                                                                          \
		String_push(&str, '}');                                                                                                    \
		return str;                                                                                                                \
	}

#define DeclareHashsetDeriveEquals(type) bool type##Hashset_equals(type##Hashset* a, type##Hashset* b);
#define DefineHashsetDeriveEquals(type)                                                                                                    \
	bool type##Hashset_equals(type##Hashset* a, type##Hashset* b) {                                                                    \
		for (size_t i = 0; i < a->capacity; i++) {                                                                                 \
			for (size_t j = 0; j < a->buckets[i].length; j++) {                                                                \
				if (!type##Hashset_contains(*b, a->buckets[i].array[j])) {                                                 \
					return false;                                                                                      \
				}                                                                                                          \
			}                                                                                                                  \
		}                                                                                                                          \
		for (size_t i = 0; i < b->capacity; i++) {                                                                                 \
			for (size_t j = 0; j < b->buckets[i].length; j++) {                                                                \
				if (!type##Hashset_contains(*a, b->buckets[i].array[j])) {                                                 \
					return false;                                                                                      \
				}                                                                                                          \
			}                                                                                                                  \
		}                                                                                                                          \
		return true;                                                                                                               \
	}

#endif // HASHSET_H