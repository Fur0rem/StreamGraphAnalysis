#include "../src/defaults.h"
#include "../src/generic_data_structures/arraylist.h"
#include "../src/generic_data_structures/binary_heap.h"
#include "../src/generic_data_structures/hashset.h"
#include "../src/utils.h"
#include "test.h"
#include <stdbool.h>

bool test_create() {
	size_tArrayList v = size_tArrayList_with_capacity(5);
	return EXPECT_EQ(v.length, 0) && EXPECT_EQ(v.capacity, 5);
}

bool test_push_1() {
	size_tArrayList v = size_tArrayList_with_capacity(5);
	size_tArrayList_push(&v, 1);
	size_tArrayList_push(&v, 2);
	size_tArrayList_push(&v, 3);
	size_tArrayList_push(&v, 4);
	size_tArrayList_push(&v, 5);
	return EXPECT_ALL(EXPECT_EQ(v.length, 5),
					  EXPECT_EQ(v.capacity, 5),
					  EXPECT_EQ(v.array[0], 1),
					  EXPECT_EQ(v.array[1], 2),
					  EXPECT_EQ(v.array[2], 3),
					  EXPECT_EQ(v.array[3], 4),
					  EXPECT_EQ(v.array[4], 5));
}

bool test_push_2() {
	size_tArrayList v = size_tArrayList_with_capacity(5);
	size_tArrayList_push(&v, 1);
	size_tArrayList_push(&v, 2);
	size_tArrayList_push(&v, 3);
	size_tArrayList_push(&v, 4);
	size_tArrayList_push(&v, 5);
	size_tArrayList_push(&v, 6);
	return v.length == 6 && v.capacity == 10 && v.array[0] == 1 && v.array[1] == 2 && v.array[2] == 3 && v.array[3] == 4 &&
		   v.array[4] == 5 && v.array[5] == 6;
}

bool test_destroy() {
	size_tArrayList v = size_tArrayList_with_capacity(5);
	size_tArrayList_push(&v, 1);
	size_tArrayList_push(&v, 2);
	size_tArrayList_push(&v, 3);
	size_tArrayList_push(&v, 4);
	size_tArrayList_push(&v, 5);
	size_tArrayList_destroy(v);
	return true;
}

bool test_remove_and_swap() {
	size_tArrayList v = size_tArrayList_with_capacity(5);
	size_tArrayList_push(&v, 1);
	size_tArrayList_push(&v, 2);
	size_tArrayList_push(&v, 3);
	size_tArrayList_push(&v, 4);
	size_tArrayList_push(&v, 5);
	size_tArrayList_remove_and_swap(&v, 2);
	return EXPECT_ALL(EXPECT_EQ(v.length, 4),
					  EXPECT_EQ(v.capacity, 5),
					  EXPECT_EQ(v.array[0], 1),
					  EXPECT_EQ(v.array[1], 2),
					  EXPECT_EQ(v.array[2], 5),
					  EXPECT_EQ(v.array[3], 4));
}

DeclareArrayList(String);
DefineArrayList(String);
DeclareHashset(String);
DefineHashset(String);
DefineArrayListDeriveRemove(String);
DefineHashsetDeriveRemove(String);

bool test_insert() {
	StringHashset s = StringHashset_with_capacity(10);
	String str		= String_from_duplicate("hello");
	bool success	= StringHashset_insert(&s, str);
	EXPECT(success);
	success &= EXPECT(StringHashset_contains(s, String_from_duplicate("hello")));
	StringHashset_destroy(s);
	return success;
}

bool test_insert_unique() {
	StringHashset s = StringHashset_with_capacity(10);
	String str		= String_from_duplicate("hello");
	bool success	= StringHashset_insert(&s, str);
	EXPECT(success);
	success &= !StringHashset_insert(&s, str);
	StringHashset_destroy(s);
	return success;
}

bool test_find_present() {
	StringHashset s = StringHashset_with_capacity(10);
	String str		= String_from_duplicate("hello");
	StringHashset_insert(&s, str);
	const String* found = StringHashset_find(s, str);
	return EXPECT(found != NULL && String_equals(found, &str));
}

bool test_find_not_present() {
	StringHashset s		= StringHashset_with_capacity(10);
	String str			= String_from_duplicate("hello");
	const String* found = StringHashset_find(s, str);
	return EXPECT(found == NULL);
}

bool test_remove_present() {
	StringHashset s = StringHashset_with_capacity(10);
	String str		= String_from_duplicate("hello");
	bool success	= EXPECT(StringHashset_insert(&s, str));
	success &= EXPECT(StringHashset_contains(s, str));
	success &= EXPECT(StringHashset_remove(&s, str));
	success &= EXPECT(!StringHashset_contains(s, str));
	return success;
}

bool test_remove_not_present() {
	StringHashset s = StringHashset_with_capacity(10);
	String str		= String_from_duplicate("hello");
	return EXPECT(!StringHashset_remove(&s, str));
}

bool test_binary_heap() {
	int array[10]	   = {5, 3, 7, 1, 2, 9, 4, 6, 8, 0};
	intBinaryHeap heap = intBinaryHeap_with_capacity(10);
	for (size_t i = 0; i < 10; i++) {
		intBinaryHeap_insert(&heap, array[i]);
	}
	bool success = true;
	for (size_t i = 0; i < 10; i++) {
		success &= EXPECT_EQ(intBinaryHeap_pop(&heap), i);
	}
	return success;
}

int main() {
	Test* tests[] = {
		TEST(test_create),
		TEST(test_push_1),
		TEST(test_push_2),
		TEST(test_destroy),
		TEST(test_remove_and_swap),
		NULL,
	};
	bool vec = test("ArrayList", tests);

	Test* tests2[] = {
		TEST(test_insert),
		TEST(test_insert_unique),
		TEST(test_find_present),
		TEST(test_find_not_present),
		TEST(test_remove_present),
		TEST(test_remove_not_present),
		NULL,
	};
	bool hash = test("Hashset", tests2);

	Test* tests3[] = {
		TEST(test_binary_heap),
		NULL,
	};
	bool heap = test("Binary Heap", tests3);

	return vec && hash && heap;
}
