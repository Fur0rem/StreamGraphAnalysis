#include "../src/bit_array.h"
#include "benchmark.h"
#include <stddef.h>
#include <stdio.h>

BitArray ba1;
BitArray ba2;

void and_array() {
	BitArray result = BitArray_and_array(ba1, ba2);
	BitArray_destroy(result);
}

void to_string() {
	String string = BitArray_to_string(&ba1);
	String_destroy(string);
}

void set_one() {
	for (size_t i = 0; i < ba1.nb_bits; i += 5) {
		BitArray_set_one(ba1, i);
	}
}

void leading_zeros() {
	for (size_t i = 0; i < ba1.nb_bits; i += 5) {
		BitArray_leading_zeros_from(ba1, i);
	}
}

int main() {
	ba1 = BitArray_n_zeros(1 << 29);
	ba2 = BitArray_n_ones(1 << 29);
	benchmark(and_array, "and_array", 10);
	benchmark(to_string, "to_string", 10);
	benchmark(set_one, "set_one", 10);
	benchmark(leading_zeros, "leading_zeros", 10);
}