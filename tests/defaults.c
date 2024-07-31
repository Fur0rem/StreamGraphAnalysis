#include "../src/defaults.h"
#include "../src/utils.h"
#include "../src/vector.h"
#include "test.h"

bool test_create() {
	size_tVector v = size_tVector_with_capacity(5);
	return EXPECT_EQ(v.size, 0) && EXPECT_EQ(v.capacity, 5);
}

bool test_push_1() {
	size_tVector v = size_tVector_with_capacity(5);
	size_tVector_push(&v, 1);
	size_tVector_push(&v, 2);
	size_tVector_push(&v, 3);
	size_tVector_push(&v, 4);
	size_tVector_push(&v, 5);
	return EXPECT_ALL(EXPECT_EQ(v.size, 5), EXPECT_EQ(v.capacity, 5), EXPECT_EQ(v.array[0], 1),
					  EXPECT_EQ(v.array[1], 2), EXPECT_EQ(v.array[2], 3), EXPECT_EQ(v.array[3], 4),
					  EXPECT_EQ(v.array[4], 5));
}

bool test_push_2() {
	size_tVector v = size_tVector_with_capacity(5);
	size_tVector_push(&v, 1);
	size_tVector_push(&v, 2);
	size_tVector_push(&v, 3);
	size_tVector_push(&v, 4);
	size_tVector_push(&v, 5);
	size_tVector_push(&v, 6);
	return v.size == 6 && v.capacity == 10 && v.array[0] == 1 && v.array[1] == 2 && v.array[2] == 3 &&
		   v.array[3] == 4 && v.array[4] == 5 && v.array[5] == 6;
}

bool test_destroy() {
	size_tVector v = size_tVector_with_capacity(5);
	size_tVector_push(&v, 1);
	size_tVector_push(&v, 2);
	size_tVector_push(&v, 3);
	size_tVector_push(&v, 4);
	size_tVector_push(&v, 5);
	size_tVector_destroy(v);
	return true;
}

bool test_remove_and_swap() {
	size_tVector v = size_tVector_with_capacity(5);
	size_tVector_push(&v, 1);
	size_tVector_push(&v, 2);
	size_tVector_push(&v, 3);
	size_tVector_push(&v, 4);
	size_tVector_push(&v, 5);
	size_tVector_remove_and_swap(&v, 2);
	return EXPECT_ALL(EXPECT_EQ(v.size, 4), EXPECT_EQ(v.capacity, 5), EXPECT_EQ(v.array[0], 1),
					  EXPECT_EQ(v.array[1], 2), EXPECT_EQ(v.array[2], 5), EXPECT_EQ(v.array[3], 4));
}

DeclareVector(String);
DefineVector(String);
DeclareHashset(String);
DefineHashset(String);
DefineVectorDeriveRemove(String, String_destroy);
DefineHashsetDeriveRemove(String, String_destroy);

// typedef struct {
// 	String* array;
// 	size_t size;
// 	size_t capacity;
// } StringVector;
// StringVector StringVector_with_capacity(size_t capacity);
// StringVector StringVector_new();
// void StringVector_push(StringVector* vec, String value);
// String* StringVector_get(StringVector* vec, size_t idx);
// size_t StringVector_len(StringVector* vec);
// _Bool StringVector_is_empty(StringVector vec);
// void StringVector_append(StringVector* vec, const String* values, size_t nb_values);
// void StringVector_reverse(StringVector* vec);
// ;
// StringVector StringVector_with_capacity(size_t capacity) {
// 	StringVector vec = {.array = (String*)malloc(sizeof(String) * capacity), .size = 0, .capacity = capacity};
// 	return vec;
// }
// StringVector StringVector_new() {
// 	return StringVector_with_capacity(2);
// }
// void StringVector_push(StringVector* vec, String value) {
// 	if (vec->size == vec->capacity) {
// 		vec->capacity *= 2;
// 		String* new_array = (String*)malloc(sizeof(String) * vec->capacity);
// 		memcpy(new_array, vec->array, sizeof(String) * vec->size);
// 		free(vec->array);
// 		vec->array = new_array;
// 	}
// 	vec->array[vec->size] = value;
// 	vec->size++;
// }
// String* StringVector_get(StringVector* vec, size_t idx) {
// 	__builtin_expect((idx < vec->size), 1);
// 	return &vec->array[idx];
// }
// size_t StringVector_len(StringVector* vec) {
// 	return vec->size;
// }
// _Bool StringVector_is_empty(StringVector vec) {
// 	return vec.size == 0;
// }
// void StringVector_reverse(StringVector* vec) {
// 	for (size_t i = 0; i < vec->size / 2; i++) {
// 		String tmp = vec->array[i];
// 		vec->array[i] = vec->array[vec->size - i - 1];
// 		vec->array[vec->size - i - 1] = tmp;
// 	}
// }
// void StringVector_append(StringVector* vec, const String* values, size_t nb_values) {
// 	if (vec->size + nb_values > vec->capacity) {
// 		vec->capacity = vec->size + nb_values;
// 		String* new_array = (String*)malloc(sizeof(String) * vec->capacity);
// 		memcpy(new_array, vec->array, sizeof(String) * vec->size);
// 		free(vec->array);
// 		vec->array = new_array;
// 	}
// 	memcpy(vec->array + vec->size, values, sizeof(String) * nb_values);
// 	vec->size += nb_values;
// };
// typedef struct T_StringHashset {
// 	StringVector* buckets;
// 	size_t capacity;
// } StringHashset;
// StringHashset StringHashset_with_capacity(size_t capacity);
// _Bool StringHashset_insert(StringHashset* s, String value);
// _Bool StringHashset_contains(StringHashset s, String value);
// size_t StringHashset_nb_elems(StringHashset* s);
// ;
// StringHashset StringHashset_with_capacity(size_t capacity) {
// 	StringHashset s = {.buckets = (StringVector*)malloc(sizeof(StringVector) * capacity), .capacity = capacity};
// 	for (size_t i = 0; i < capacity; i++) {
// 		s.buckets[i] = StringVector_with_capacity(10);
// 	}
// 	return s;
// }
// _Bool StringHashset_insert(StringHashset* s, String value) {
// 	size_t index = String_hash(&value) % s->capacity;
// 	for (size_t i = 0; i < s->buckets[index].size; i++) {
// 		if (String_equals(&s->buckets[index].array[i], &value)) {
// 			return 0;
// 		}
// 	}
// 	StringVector_push(&s->buckets[index], value);
// 	return 1;
// }
// _Bool StringHashset_contains(StringHashset s, String value) {
// 	size_t index = String_hash(&value) % s.capacity;
// 	for (size_t i = 0; i < s.buckets[index].size; i++) {
// 		if (String_equals(&s.buckets[index].array[i], &value)) {
// 			return 1;
// 		}
// 	}
// 	return 0;
// }
// size_t StringHashset_nb_elems(StringHashset* s) {
// 	size_t size = 0;
// 	for (size_t i = 0; i < s->capacity; i++) {
// 		size += s->buckets[i].size;
// 	}
// 	return size;
// }
// const String* StringHashset_find(StringHashset s, String value) {
// 	size_t index = String_hash(&value) % s.capacity;
// 	for (size_t i = 0; i < s.buckets[index].size; i++) {
// 		if (String_equals(&s.buckets[index].array[i], &value)) {
// 			return &s.buckets[index].array[i];
// 		}
// 	}
// 	return ((void*)0);
// };
// void StringVector_remove_and_swap(StringVector* vec, size_t idx) {
// 	if (String_destroy) {
// 		String_destroy(vec->array[idx]);
// 	}
// 	vec->array[idx] = vec->array[vec->size - 1];
// 	vec->size--;
// }
// void StringVector_remove(StringVector* vec, size_t idx) {
// 	if (String_destroy) {
// 		String_destroy(vec->array[idx]);
// 	}
// 	for (size_t i = idx; i < vec->size - 1; i++) {
// 		vec->array[i] = vec->array[i + 1];
// 	}
// 	vec->size--;
// }
// void StringVector_destroy(StringVector vec) {
// 	if (String_destroy) {
// 		for (size_t i = 0; i < vec.size; i++) {
// 			String_destroy(vec.array[i]);
// 		}
// 	}
// 	free(vec.array);
// }
// void StringVector_pop_last(StringVector* vec) {
// 	if (String_destroy) {
// 		String_destroy(vec->array[vec->size - 1]);
// 	}
// 	vec->size--;
// }
// void StringVector_pop_first(StringVector* vec) {
// 	if (String_destroy) {
// 		String_destroy(vec->array[0]);
// 	}
// 	for (size_t i = 0; i < vec->size - 1; i++) {
// 		vec->array[i] = vec->array[i + 1];
// 	}
// 	vec->size--;
// };
// void StringHashset_destroy(StringHashset s) {
// 	for (size_t i = 0; i < s.capacity; i++) {
// 		StringVector_destroy(s.buckets[i]);
// 	}
// 	free(s.buckets);
// }
// _Bool StringHashset_remove(StringHashset* s, String value) {
// 	size_t index = String_hash(&value) % s->capacity;
// 	for (size_t i = 0; i < s->buckets[index].size; i++) {
// 		if (String_equals(&s->buckets[index].array[i], &value)) {
// 			String_destroy(s->buckets[index].array[i]);
// 			StringVector_remove_and_swap(&s->buckets[index], i);
// 			return 1;
// 		}
// 	}
// 	return 0;
// };

bool test_insert() {
	StringHashset s = StringHashset_with_capacity(10);
	String str = String_from_duplicate("hello");
	bool success = StringHashset_insert(&s, str);
	EXPECT(success);
	success &= EXPECT(StringHashset_contains(s, String_from_duplicate("hello")));
	StringHashset_destroy(s);
	return success;
}

bool test_insert_unique() {
	StringHashset s = StringHashset_with_capacity(10);
	String str = String_from_duplicate("hello");
	bool success = StringHashset_insert(&s, str);
	EXPECT(success);
	success &= !StringHashset_insert(&s, str);
	StringHashset_destroy(s);
	return success;
}

bool test_find_present() {
	StringHashset s = StringHashset_with_capacity(10);
	String str = String_from_duplicate("hello");
	StringHashset_insert(&s, str);
	const String* found = StringHashset_find(s, str);
	return EXPECT(found != NULL && String_equals(found, &str));
}

bool test_find_not_present() {
	StringHashset s = StringHashset_with_capacity(10);
	String str = String_from_duplicate("hello");
	const String* found = StringHashset_find(s, str);
	return EXPECT(found == NULL);
}

bool test_remove_present() {
	StringHashset s = StringHashset_with_capacity(10);
	String str = String_from_duplicate("hello");
	bool success = EXPECT(StringHashset_insert(&s, str));
	success &= EXPECT(StringHashset_contains(s, str));
	success &= EXPECT(StringHashset_remove(&s, str));
	success &= EXPECT(!StringHashset_contains(s, str));
	return success;
}

bool test_remove_not_present() {
	StringHashset s = StringHashset_with_capacity(10);
	String str = String_from_duplicate("hello");
	return EXPECT(!StringHashset_remove(&s, str));
}

int main() {
	Test* tests[] = {
		&(Test){"create",		  test_create		 },
		&(Test){"push_1",		  test_push_1		 },
		&(Test){"push_2",		  test_push_2		 },
		&(Test){"destroy",		   test_destroy		   },
		&(Test){"remove_and_swap", test_remove_and_swap},
		NULL
	};

	bool vec = test("Vector", tests);

	Test* tests2[] = {
		&(Test){"insert",			  test_insert			 },
		&(Test){"insert_unique",		 test_insert_unique	   },
		&(Test){"find_present",		test_find_present		 },
		&(Test){"find_not_present",	test_find_not_present	 },
		&(Test){"remove_present",	  test_remove_present	 },
		&(Test){"remove_not_present", test_remove_not_present},
		NULL
	};

	bool hash = test("Hashset", tests2);

	return vec && hash;
}
