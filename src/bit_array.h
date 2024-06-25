#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
	size_t nb_bits;
	int* bits;
} BitArray;

BitArray BitArray_with_n_bits(size_t nb_bits);
BitArray BitArray_n_zeros(size_t nb_bits);
BitArray BitArray_n_ones(size_t nb_bits);

bool BitArray_is_one(BitArray array, size_t index);
bool BitArray_is_zero(BitArray array, size_t index);
void BitArray_set_one(BitArray array, size_t index);
void BitArray_set_zero(BitArray array, size_t index);

void BitArray_and_bit(BitArray array, size_t index, int value);
void BitArray_or_bit(BitArray array, size_t index, int value);
BitArray BitArray_and_array(BitArray array1, BitArray array2);
BitArray BitArray_or_array(BitArray array1, BitArray array2);

void BitArray_destroy(BitArray array);
char* BitArray_to_string(BitArray array);

size_t BitArray_leading_zeros_from(BitArray array, size_t index);

#endif