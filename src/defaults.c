#include "defaults.h"
#include "generic_data_structures/binary_heap.h"
#include "utils.h"
#include <stddef.h>

NO_FREE(size_t);
DEFAULT_COMPARE(size_t);
DEFAULT_EQUALS(size_t);
DEFAULT_TO_STRING(size_t, "%zu");
DefineFullDerivedArrayList(size_t);

size_t size_t_hash(size_t* value) {
	return *value;
}

DefineFullDerivedHashset(size_t);
DefineBinaryHeap(size_t);

NO_FREE(int);
DEFAULT_COMPARE(int);
DEFAULT_EQUALS(int);
DEFAULT_TO_STRING(int, "%d");
DefineFullDerivedArrayList(int);

size_t int_hash(int* value) {
	return *value;
}

DefineFullDerivedHashset(int);
DefineBinaryHeap(int);
