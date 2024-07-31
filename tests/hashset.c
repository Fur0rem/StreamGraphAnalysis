#include "../src/defaults.h"
#include "../src/utils.h"
#include "test.h"

DeclareVector(String);
DefineVector(String);
DeclareHashset(String);
DefineHashset(String);
DefineVectorDeriveRemove(String, String_destroy);
DefineHashsetDeriveRemove(String, String_destroy);

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
		&(Test){"insert",			  test_insert			 },
		&(Test){"insert_unique",		 test_insert_unique	   },
		&(Test){"find_present",		test_find_present		 },
		&(Test){"find_not_present",	test_find_not_present	 },
		&(Test){"remove_present",	  test_remove_present	 },
		&(Test){"remove_not_present", test_remove_not_present},
		NULL,
	};
	return test("Hashset", tests);
}