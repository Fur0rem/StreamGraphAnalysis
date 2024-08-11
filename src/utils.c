#include "utils.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
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
	for (size_t i = 0; i < string->size; i++) {
		hash = ((hash * 31) + str[i]) ^ (str[i / 2] | str[i / 4]);
	}
	return hash;
}

char* read_file(const char* filename) {
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Error opening file %s\n", filename);
		exit(1);
	}
	fseek(file, 0, SEEK_END);
	size_t length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* buffer = malloc(length + 1);
	if (buffer == NULL) {
		fprintf(stderr, "Error allocating memory for file %s\n", filename);
		exit(1);
	}
	size_t read = fread(buffer, 1, length, file);
	if (read != length) {
		fprintf(stderr, "Error reading file %s\n", filename);
		exit(1);
	}
	buffer[length] = '\0';
	fclose(file);
	return buffer;
}

void String_append_formatted(String* string, const char* format, ...) {
	va_list args;
	va_start(args, format);
	size_t len = vsnprintf(NULL, 0, format, args);
	va_end(args);
	char* buffer = malloc(len + 1);
	va_start(args, format);
	vsnprintf(buffer, len + 1, format, args);
	va_end(args);
	String_push_str(string, buffer);
	free(buffer);
}