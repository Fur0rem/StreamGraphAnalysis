#include "bit_array.h"
#include "utils.h"
#include <stddef.h>
#include <stdio.h>

// First byte is used to store the number of bits in the array

BitArray BitArray_with_n_bits(size_t nb_bits) {

	size_t size = (nb_bits / sizeof(size_t)) + 1;
	BitArray bit_array = (BitArray){.nb_bits = nb_bits, .bits = MALLOC(size * sizeof(size_t))};
	return bit_array;
}

BitArray BitArray_n_zeros(size_t nb_bits) {
	BitArray bit_array = BitArray_with_n_bits(nb_bits);
	for (size_t i = 0; i < (nb_bits / sizeof(size_t)); i++) {
		bit_array.bits[i] = 0;
	}
	return bit_array;
}

BitArray BitArray_n_ones(size_t nb_bits) {
	BitArray bit_array = BitArray_with_n_bits(nb_bits);
	for (size_t i = 0; i < (nb_bits / sizeof(size_t)); i++) {
		bit_array.bits[i] = ~0;
	}
	return bit_array;
}

const size_t BYTE_SIZE = 8;
size_t byte_index(size_t index) {
	return (index / (sizeof(size_t) * BYTE_SIZE));
}

size_t bit_index(size_t index) {
	return index % (sizeof(size_t) * BYTE_SIZE);
}

bool BitArray_is_one(BitArray array, size_t index) {
	return (array.bits[byte_index(index)] & (1 << bit_index(index))) != 0;
}

void BitArray_set_one(BitArray array, size_t index) {
	array.bits[byte_index(index)] |= (1 << bit_index(index));
}

void BitArray_set_zero(BitArray array, size_t index) {
	array.bits[byte_index(index)] &= ~(1 << bit_index(index));
}

void BitArray_destroy(BitArray array) {
	free(array.bits);
}

char* BitArray_to_string(BitArray array) {
	size_t n_bits = array.nb_bits;
	char* str = MALLOC((n_bits + 1) * sizeof(char));
	for (size_t i = 0; i < n_bits; i++) {
		str[i] = BitArray_is_one(array, i) ? '1' : '0';
	}
	str[n_bits] = '\0';
	return str;
}