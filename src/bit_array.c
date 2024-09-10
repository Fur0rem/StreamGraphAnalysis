#include "bit_array.h"
#include "utils.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// Using an int because it is the biggest type that can be used with bitwise shifting operations that doesn't fuck up
const size_t SLICE_SIZE_IN_BYTES = sizeof(BitArraySlice);
const size_t BYTE_SIZE			 = 8;

#define _1	   ((BitArraySlice)1)
#define _0	   ((BitArraySlice)0)
#define ALL_1s (~((BitArraySlice)0))

size_t nb_slices(size_t nb_bits) {
	return (nb_bits / (SLICE_SIZE_IN_BYTES)) + 1;
}

BitArray BitArray_with_n_bits(size_t nb_bits) {
	BitArray bit_array = (BitArray){.nb_bits = nb_bits, .bits = MALLOC(nb_slices(nb_bits) * SLICE_SIZE_IN_BYTES)};
	return bit_array;
}

BitArray BitArray_n_zeros(size_t nb_bits) {
	BitArray bit_array = BitArray_with_n_bits(nb_bits);
	for (size_t i = 0; i < nb_slices(nb_bits); i++) {
		bit_array.bits[i] = _0;
	}
	return bit_array;
}

BitArray BitArray_n_ones(size_t nb_bits) {
	BitArray bit_array = BitArray_with_n_bits(nb_bits);
	for (size_t i = 0; i < nb_slices(nb_bits); i++) {
		bit_array.bits[i] = ALL_1s;
	}
	return bit_array;
}

size_t byte_index(size_t index) {
	return (index / (SLICE_SIZE_IN_BYTES * BYTE_SIZE));
}

size_t bit_index(size_t index) {
	return index % (SLICE_SIZE_IN_BYTES * BYTE_SIZE);
}

bool BitArray_is_one(BitArray array, size_t index) {
	return (array.bits[byte_index(index)] & (_1 << bit_index(index))) != _0;
}

bool BitArray_is_zero(BitArray array, size_t index) {
	return !BitArray_is_one(array, index);
}

void BitArray_set_one(BitArray array, size_t index) {
	array.bits[byte_index(index)] |= (_1 << bit_index(index));
}

void BitArray_set_zero(BitArray array, size_t index) {
	array.bits[byte_index(index)] &= ~(_1 << bit_index(index));
}

void BitArray_and_bit(BitArray array, size_t index, int value) {
	array.bits[byte_index(index)] &= value ? (_1 << bit_index(index)) : ~(_1 << bit_index(index));
}

void BitArray_or_bit(BitArray array, size_t index, int value) {
	array.bits[byte_index(index)] |= value ? (_1 << bit_index(index)) : ~(_1 << bit_index(index));
}

BitArray BitArray_and_array(BitArray array1, BitArray array2) {
	BitArray result = BitArray_with_n_bits(array1.nb_bits);
	for (size_t i = 0; i < nb_slices(array1.nb_bits); i++) {
		result.bits[i] = array1.bits[i] & array2.bits[i];
	}
	return result;
}

BitArray BitArray_or_array(BitArray array1, BitArray array2) {
	BitArray result = BitArray_with_n_bits(array1.nb_bits);
	for (size_t i = 0; i < nb_slices(array1.nb_bits); i++) {
		result.bits[i] = array1.bits[i] | array2.bits[i];
	}
	return result;
}

void BitArray_destroy(BitArray array) {
	free(array.bits);
}

const char HEXA_TO_BIN_CHARS[16][sizeof(uint32_t) + 1] = {
	"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
	"1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111",
};

String BitArray_to_string(const BitArray* array) {

	if (nb_slices(array->nb_bits) <= 1) {
		char* str = MALLOC(array->nb_bits + 1);
		for (size_t i = 0; i < array->nb_bits; i++) {
			str[i] = BitArray_is_one(*array, i) ? '1' : '0';
		}
		str[array->nb_bits] = '\0';
		return (String){
			.data	  = str,
			.size	  = array->nb_bits,
			.capacity = array->nb_bits + 1,
		};
	}

	const size_t copy_step = sizeof(uint32_t);
	const size_t capacity  = (nb_slices(array->nb_bits) * BYTE_SIZE) + 1;
	const size_t size	   = array->nb_bits + 1;
	char* str			   = aligned_alloc(copy_step, capacity); // Aligned to the copy size
	for (size_t i = 0; i < nb_slices(array->nb_bits); i++) {
		// gcc t'es con MASK_SIZE c'est sizeof(BitArraySlice) mais il veut pas sinon "VaRiABle LeNGtH ArRAyS" meme si
		// MASK_SIZE est const
		const unsigned char bytes[sizeof(BitArraySlice)] = {
			(array->bits[i] >> 28) & 0xF, (array->bits[i] >> 24) & 0xF, (array->bits[i] >> 20) & 0xF,
			(array->bits[i] >> 16) & 0xF, (array->bits[i] >> 12) & 0xF, (array->bits[i] >> 8) & 0xF,
			(array->bits[i] >> 4) & 0xF,  array->bits[i] & 0xF,
		};
		for (size_t j = 0; j < SLICE_SIZE_IN_BYTES; j += copy_step) {
			size_t offset = i * SLICE_SIZE_IN_BYTES + j;
			// Copy the 4 bytes at once
			uint32_t* ptr = (uint32_t*)(str + offset);
			*ptr		  = *(uint32_t*)HEXA_TO_BIN_CHARS[bytes[j]];
		}
	}
	str[size - 1] = '\0';
	return (String){
		.data	  = str,
		.size	  = size,
		.capacity = capacity,
	};
}

bool BitArray_equals(const BitArray* a, const BitArray* b) {
	if (a->nb_bits != b->nb_bits) {
		return false;
	}

	// Compare all slices except the last one
	if (nb_slices(a->nb_bits) > 1) {
		for (size_t i = 0; i < nb_slices(a->nb_bits) - 1; i++) {
			if (a->bits[i] != b->bits[i]) {
				return false;
			}
		}
		for (size_t i = (nb_slices(a->nb_bits) - 1) * SLICE_SIZE_IN_BYTES; i < a->nb_bits; i++) {
			if (BitArray_is_one(*a, i) != BitArray_is_one(*b, i)) {
				return false;
			}
		}
	}
	else {
		for (size_t i = 0; i < a->nb_bits; i++) {
			if (BitArray_is_one(*a, i) != BitArray_is_one(*b, i)) {
				return false;
			}
		}
	}
	return true;
}

// Returns the number of leading zeros in the bit array starting from the given index
// NOTE: GCC and Clang are smart enough to optimise this function using the lzcnt instruction (benchmarked with -O3)
size_t BitArray_leading_zeros_from(BitArray array, size_t index) {
	size_t leading_zeros = 0;
	while (index < array.nb_bits) {
		if (BitArray_is_one(array, index)) {
			return leading_zeros;
		}
		leading_zeros++;
		index++;
	}
	return leading_zeros;
}

void BitArray_all_to_one(BitArray array) {
	for (size_t i = 0; i < nb_slices(array.nb_bits); i++) {
		array.bits[i] = ALL_1s;
	}
}

void BitArray_all_to_zero(BitArray array) {
	for (size_t i = 0; i < nb_slices(array.nb_bits); i++) {
		array.bits[i] = _0;
	}
}

// OPTIMISE: pop count instruction
size_t BitArray_count_ones(const BitArray* array) {
	size_t nb_ones = 0;
	for (size_t i = 0; i < array->nb_bits; i++) {
		if (BitArray_is_one(*array, i)) {
			nb_ones++;
		}
	}
	return nb_ones;
}