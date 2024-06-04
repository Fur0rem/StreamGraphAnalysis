#include <string.h>
#include "../src/hashset.h"
#include "test.h"

int hash_string(const char* str) {
    int hash = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        hash = ((hash * 31) + str[i]) ^ (str[i/2] | str[i/4]);
    }
    return hash;
}

int eq_string(const char* str1, const char* str2) {
    return !strcmp(str1, str2);
}

typedef char* String;

char* string_tostring(String str) {
    return strdup(str);
}

DefHashset(String, hash_string, eq_string, string_tostring, free)

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
    return test("Hashset", 
        &(Test) { "insert", test_insert },
        &(Test) { "insert unique", test_insert_unique },
        &(Test) { "find present", test_find_present },
        &(Test) { "find not present", test_find_not_present },
        &(Test) { "remove present", test_remove_present },
        &(Test) { "remove not present", test_remove_not_present },
        NULL
    ) ? 0 : 1;
}