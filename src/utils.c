#include "utils.h"
#include <stddef.h>
#include <string.h>

String String_from_owned(char* str) {
	size_t len = strlen(str);
	String string = (String){
		.size = len,
		.capacity = len + 1,
		.data = str,
	};
	return string;
}

String String_from_duplicate(const char* str) {
	size_t len = strlen(str);
	char* data = malloc(len + 1);
	memcpy(data, str, len + 1);
	String string = (String){
		.size = len,
		.capacity = len + 1,
		.data = data,
	};
	return string;
}

void String_push(String* string, char c) {
	if (string->size + 1 >= string->capacity) {
		string->capacity *= 2;
		string->data = realloc(string->data, string->capacity);
	}
	string->data[string->size++] = c;
	string->data[string->size] = '\0';
}

void String_push_str(String* string, const char* str) {
	size_t len = strlen(str);
	if (string->size + len >= string->capacity) {
		while (string->size + len >= string->capacity) {
			string->capacity *= 2;
		}
		string->data = realloc(string->data, string->capacity);
	}
	memcpy(string->data + string->size, str, len + 1);
	string->size += len;
}

void String_concat(String* a, const String* b) {
	if (a->size + b->size >= a->capacity) {
		while (a->size + b->size >= a->capacity) {
			a->capacity *= 2;
		}
		a->data = realloc(a->data, a->capacity);
	}
	memcpy(a->data + a->size, b->data, b->size + 1);
	a->size += b->size;
}

void String_destroy(String string) {
	free(string.data);
}

bool String_equals(const String* a, const String* b) {
	return strcmp(a->data, b->data) == 0;
}

int String_compare(const void* a, const void* b) {
	return strcmp(((String*)a)->data, ((String*)b)->data);
}

int String_hash(const String* string) {
	int hash = 0;
	char* str = string->data;
	for (int i = 0; i < string->size; i++) {
		hash = ((hash * 31) + str[i]) ^ (str[i / 2] | str[i / 4]);
	}
	return hash;
}