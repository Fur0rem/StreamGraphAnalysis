#include "../src/hashset.h"
#include "test.h"
#include <string.h>

int hash_string(const char* str) {
	int hash = 0;
	for (int i = 0; str[i] != '\0'; i++) {
		hash = ((hash * 31) + str[i]) ^ (str[i / 2] | str[i / 4]);
	}
	return hash;
}

typedef char* String;

char* String_to_string(String* str) {
	return strdup(*str);
}

bool String_equals(String str1, String str2) {
	return !strcmp(str1, str2);
}

DefHashset(String, hash_string, free);

bool test_insert() {
	StringHashset s = StringHashset_with_capacity(10);
	String str = strdup("hello");
	bool success = StringHashset_insert(&s, str);
	EXPECT(success);
	success &= EXPECT(StringHashset_contains(s, "hello"));
	StringHashset_destroy(s);
	return success;
}

bool test_insert_unique() {
	StringHashset s = StringHashset_with_capacity(10);
	String str = strdup("hello");
	bool success = StringHashset_insert(&s, str);
	EXPECT(success);
	success &= !StringHashset_insert(&s, str);
	StringHashset_destroy(s);
	return success;
}

bool test_find_present() {
	StringHashset s = StringHashset_with_capacity(10);
	String str = strdup("hello");
	StringHashset_insert(&s, str);
	String* found = StringHashset_find(s, str);
	return EXPECT_EQ(*found, str);
}

bool test_find_not_present() {
	StringHashset s = StringHashset_with_capacity(10);
	String str = strdup("hello");
	String* found = StringHashset_find(s, str);
	return EXPECT(found == NULL);
}

bool test_remove_present() {
	StringHashset s = StringHashset_with_capacity(10);
	String str = strdup("hello");
	bool success = EXPECT(StringHashset_insert(&s, str));
	success &= EXPECT(StringHashset_contains(s, str));
	success &= EXPECT(StringHashset_remove(&s, str));
	success &= EXPECT(!StringHashset_contains(s, str));
	return success;
}

bool test_remove_not_present() {
	StringHashset s = StringHashset_with_capacity(10);
	String str = strdup("hello");
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