#include "defaults.h"
#include "hashset.h"
#include "utils.h"
#include "vector.h"
#include <stddef.h>

DEFAULT_COMPARE(size_t);
DEFAULT_EQUALS(size_t);
DEFAULT_TO_STRING(size_t, "%zu");
DefineFullDerivedVector(size_t, NO_FREE(size_t));

size_t size_t_hash(size_t* value) {
	return *value;
}

DefineFullDerivedHashset(size_t, NO_FREE(size_t));

DEFAULT_COMPARE(int);
DEFAULT_EQUALS(int);
DEFAULT_TO_STRING(int, "%d");
DefineFullDerivedVector(int, NO_FREE(int));

size_t int_hash(int* value) {
	return *value;
}

DefineFullDerivedHashset(int, NO_FREE(int));