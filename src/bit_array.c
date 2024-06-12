#include "bit_array.h"
#include "utils.h"
#include <stddef.h>
#include <stdio.h>

// First byte is used to store the number of bits in the array

SGA_BitArray SGA_BitArray_with_n_bits(size_t n_bits) {

	size_t size = (n_bits / sizeof(size_t)) + 1;
	SGA_BitArray bit_array = MALLOC(size * sizeof(size_t)); // The first element contains the number of
															// elements in the array, then the elements
	bit_array[0] = n_bits;
	return bit_array;
}

SGA_BitArray SGA_BitArray_n_zeros(size_t n_bits) {
	SGA_BitArray bit_array = SGA_BitArray_with_n_bits(n_bits);
	for (size_t i = 1; i < 1 + (n_bits / sizeof(size_t)); i++) {
		bit_array[i] = 0;
	}
	return bit_array;
}

SGA_BitArray SGA_BitArray_n_ones(size_t n_bits) {
	SGA_BitArray bit_array = SGA_BitArray_with_n_bits(n_bits);
	for (size_t i = 1; i < 1 + (n_bits / sizeof(size_t)); i++) {
		bit_array[i] = (size_t)-1;
	}
	return bit_array;
}

const size_t BYTE_SIZE = 8;
size_t byte_index(size_t index) {
	return 1 + (index / (sizeof(size_t) * BYTE_SIZE));
}

size_t bit_index(size_t index) {
	return index % (sizeof(size_t) * BYTE_SIZE);
}

bool SGA_BitArray_is_one(SGA_BitArray array, size_t index) {
	return (array[byte_index(index)] & (1 << bit_index(index))) != 0;
}

void SGA_BitArray_set_one(SGA_BitArray array, size_t index) {
	array[byte_index(index)] |= (1 << bit_index(index));
}

void SGA_BitArray_set_zero(SGA_BitArray array, size_t index) {
	array[byte_index(index)] &= ~(1 << bit_index(index));
}

size_t SGA_BitArray_size(SGA_BitArray array) {
	return array[0];
}

void SGA_BitArray_destroy(SGA_BitArray array) {
	free(array);
}

char* SGA_BitArray_to_string(SGA_BitArray array) {
	size_t n_bits = SGA_BitArray_size(array);
	char* str = MALLOC((n_bits + 1) * sizeof(char));
	for (size_t i = 0; i < n_bits; i++) {
		str[i] = SGA_BitArray_is_one(array, i) ? '1' : '0';
	}
	str[n_bits] = '\0';
	return str;
}