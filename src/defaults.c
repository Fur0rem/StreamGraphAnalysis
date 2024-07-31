#include "defaults.h"
#include "hashset.h"
#include "utils.h"
#include "vector.h"
#include <stddef.h>

DEFAULT_COMPARE(size_t);
DEFAULT_EQUALS(size_t);
DEFAULT_TO_STRING(size_t, "%zu");
DefineVector(size_t);
DefineVectorDeriveRemove(size_t, NO_FREE(size_t));
DefineVectorDeriveOrdered(size_t);
String size_tVector_to_string(size_tVector* vec) {
	String str = String_from_duplicate("[ ");
	for (size_t i = 0; i < vec->size; i++) {
		String elem_str = size_t_to_string(&vec->array[i]);
		String_concat(&str, &elem_str);
		String_destroy(elem_str);
		if (i < vec->size - 1) {
			String_push_str(&str, ", ");
		}
	}
	String_push_str(&str, " ]");
	return str;
};

size_t size_t_hash(size_t* value) {
	return *value;
}

DefineHashset(size_t);
DefineHashsetDeriveRemove(size_t, NO_FREE(size_t));
DefineHashsetDeriveEquals(size_t);
DefineHashsetDeriveToString(size_t);