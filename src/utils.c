#include "utils.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

String String_from_owned(char* str) {
	size_t len    = strlen(str);
	String string = (String){
	    .size     = len,
	    .capacity = len + 1,
	    .data     = str,
	};
	return string;
}

void String_reserve_extra(String* self, size_t extra) {
	if (self->size + extra >= self->capacity) {
		while (self->size + extra >= self->capacity) {
			self->capacity += (self->capacity + 2) / 2;
		}
		self->data = realloc(self->data, self->capacity);
	}
}

String String_from_duplicate(const char* str) {
	size_t len = strlen(str);
	char* data = MALLOC(len + 1);
	memcpy(data, str, len + 1);
	String string = (String){
	    .size     = len,
	    .capacity = len + 1,
	    .data     = data,
	};
	return string;
}

String String_from_file(const char* filename) {
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Error opening file %s\n", filename);
		exit(1);
	}
	fseek(file, 0, SEEK_END);
	size_t length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* buffer = MALLOC(length + 1);
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
	return String_from_owned(buffer);
}

String String_clone(const String* str) {
	char* data = MALLOC(str->size);
	memcpy(data, str->data, str->size);
	String string = (String){
	    .size     = str->size,
	    .capacity = str->size,
	    .data     = data,
	};
	return string;
}

void String_push(String* self, char character) {
	String_reserve_extra(self, 1);
	self->data[self->size] = character;
	self->size++;
	self->data[self->size] = '\0';
}

void String_push_str(String* self, const char* str) {
	size_t len = strlen(str);
	String_reserve_extra(self, len);
	memcpy(self->data + self->size, str, len + 1);
	self->size += len;
}

void String_concat_copy(String* self, const String* with) {
	String_reserve_extra(self, with->size);
	memcpy(self->data + self->size, with->data, with->size);
	self->size += with->size;
}

void String_concat_consume(String* self, String with) {
	String_concat_copy(self, &with);
	String_destroy(with);
}

void String_destroy(String self) {
	free(self.data);
}

bool String_equals(const String* left, const String* right) {
	if (left->size != right->size) {
		return false;
	}
	return strncmp(left->data, right->data, left->size) == 0;
}

int String_compare(const String* left, const String* right) {
	size_t min_size = left->size;
	if (right->size < min_size) {
		min_size = right->size;
	}
	bool cmp_until_len = strncmp(left->data, right->data, min_size);
	if (cmp_until_len != 0) {
		return cmp_until_len;
	}
	return left->size - right->size;
}

// FIXME: slow hash function
int String_hash(const String* self) {
	int hash  = 0;
	char* str = self->data;
	for (size_t i = 0; i < self->size; i++) {
		hash = ((hash * 32) + str[i]) ^ (str[i / 2] | str[i / 4]);
	}
	return hash;
}

// char* read_file(const char* filename) {
// 	FILE* file = fopen(filename, "r");
// 	if (file == NULL) {
// 		fprintf(stderr, "Error opening file %s\n", filename);
// 		exit(1);
// 	}
// 	fseek(file, 0, SEEK_END);
// 	size_t length = ftell(file);
// 	fseek(file, 0, SEEK_SET);
// 	char* buffer = malloc(length + 1);
// 	if (buffer == NULL) {
// 		fprintf(stderr, "Error allocating memory for file %s\n", filename);
// 		exit(1);
// 	}
// 	size_t read = fread(buffer, 1, length, file);
// 	if (read != length) {
// 		fprintf(stderr, "Error reading file %s\n", filename);
// 		exit(1);
// 	}
// 	buffer[length] = '\0';
// 	fclose(file);
// 	return buffer;
// }

void String_append_formatted(String* self, const char* format, ...) {
	va_list args;
	va_start(args, format);
	size_t len = vsnprintf(NULL, 0, format, args);
	va_end(args);
	char* buffer = MALLOC(len + 1);
	va_start(args, format);
	vsnprintf(buffer, len + 1, format, args);
	va_end(args);
	String_push_str(self, buffer);
	free(buffer);
}

void String_pop_n(String* self, size_t n) {
	if (n > self->size) {
		n = self->size;
	}
	self->size -= n;
}

void String_pop(String* self) {
	ASSERT(self->size > 0);
	self->size--;
}

String String_with_capacity(size_t capacity) {
	ASSERT(capacity > 0);
	return (String){
	    .size     = 0,
	    .capacity = capacity,
	    .data     = MALLOC(capacity),
	};
}

String String_to_string(const String* self) {
	String cstr = String_clone(self);
	String_push(&cstr, '\0');
	return cstr;
}

void String_write_to_file(const String* self, const char* filename) {
	FILE* file = fopen(filename, "w");
	if (file == NULL) {
		fprintf(stderr, "Error opening file %s\n", filename);
		exit(1);
	}
	fwrite(self->data, 1, self->size, file);
	fclose(file);
}