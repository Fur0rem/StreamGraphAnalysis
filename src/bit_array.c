#include "bit_array.h"
#include "utils.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// Using an int because it is the biggest type that can be used with bitwise shifting operations that doesn't fuck up
const size_t SLICE_SIZE_IN_BYTES = sizeof(BitArraySlice);
const size_t BYTE_SIZE			 = 8;
const size_t SLICE_SIZE_IN_BITS	 = SLICE_SIZE_IN_BYTES * BYTE_SIZE;

#define _1	   ((BitArraySlice)1)
#define _0	   ((BitArraySlice)0)
#define ALL_1s (~((BitArraySlice)0))

size_t BitArray_nb_slices(size_t nb_bits) {
	return (nb_bits / (SLICE_SIZE_IN_BYTES * BYTE_SIZE)) + 1;
}

size_t BitArray_nb_bytes(size_t nb_bits) {
	return (nb_bits / SLICE_SIZE_IN_BYTES) + 1;
}

BitArray BitArray_with_n_bits(size_t nb_bits) {
	BitArray bit_array =
		(BitArray){.nb_bits = nb_bits, .bits = MALLOC(BitArray_nb_slices(nb_bits) * SLICE_SIZE_IN_BYTES)};
	return bit_array;
}

BitArray BitArray_n_zeros(size_t nb_bits) {
	BitArray bit_array = BitArray_with_n_bits(nb_bits);
	for (size_t i = 0; i < BitArray_nb_slices(nb_bits); i++) {
		bit_array.bits[i] = _0;
	}
	return bit_array;
}

BitArray BitArray_n_ones(size_t nb_bits) {
	BitArray bit_array = BitArray_with_n_bits(nb_bits);
	for (size_t i = 0; i < BitArray_nb_slices(nb_bits); i++) {
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
	for (size_t i = 0; i < BitArray_nb_slices(array1.nb_bits); i++) {
		result.bits[i] = array1.bits[i] & array2.bits[i];
	}
	return result;
}

BitArray BitArray_or_array(BitArray array1, BitArray array2) {
	BitArray result = BitArray_with_n_bits(array1.nb_bits);
	for (size_t i = 0; i < BitArray_nb_slices(array1.nb_bits); i++) {
		result.bits[i] = array1.bits[i] | array2.bits[i];
	}
	return result;
}

void BitArray_destroy(BitArray array) {
	free(array.bits);
}

const char HEXA_TO_BIN_CHARS[16][sizeof(uint32_t) + 1] = {
	"0000", "1000", "0100", "1100", "0010", "1010", "0110", "1110",
	"0001", "1001", "0101", "1101", "0011", "1011", "0111", "1111",
};

String BitArray_to_string(const BitArray* array) {

	const size_t copy_step = sizeof(uint32_t);

	// TODO: Add arch verification functions
	ASSERT((SLICE_SIZE_IN_BITS / copy_step) == 16);
	ASSERT((int)true == 1);
	ASSERT((int)false == 0);
	ASSERT(('0' + 1) == '1');

	const size_t capacity = (BitArray_nb_slices(array->nb_bits) * SLICE_SIZE_IN_BITS) + 1;

// NOTE: Valgrind doesn't support aligned_alloc but I tried with malloc and it works
// And we need to align the memory to have it aligned on uint32_t to copy 4 bytes at once
#ifdef DEBUG
	char* str = MALLOC(capacity);
#else
	char* str = aligned_alloc(copy_step, capacity);
#endif

	for (size_t i = 0; i < BitArray_nb_slices(array->nb_bits) - 1; i++) {
		// gcc t'es con MASK_SIZE c'est sizeof(BitArraySlice) mais il veut pas sinon "VaRiABle LeNGtH ArRAyS" meme si
		// MASK_SIZE est const
		//[SLICE_SIZE_IN_BITS / copy_step];
		const unsigned char bytes[16] = {
			array->bits[i] & 0xF,		  (array->bits[i] >> 4) & 0xF,	(array->bits[i] >> 8) & 0xF,
			(array->bits[i] >> 12) & 0xF, (array->bits[i] >> 16) & 0xF, (array->bits[i] >> 20) & 0xF,
			(array->bits[i] >> 24) & 0xF, (array->bits[i] >> 28) & 0xF, (array->bits[i] >> 32) & 0xF,
			(array->bits[i] >> 36) & 0xF, (array->bits[i] >> 40) & 0xF, (array->bits[i] >> 44) & 0xF,
			(array->bits[i] >> 48) & 0xF, (array->bits[i] >> 52) & 0xF, (array->bits[i] >> 56) & 0xF,
			(array->bits[i] >> 60) & 0xF,
		};

		for (size_t j = 0; j < SLICE_SIZE_IN_BITS / copy_step; j++) {
			size_t offset = i * SLICE_SIZE_IN_BITS + j * copy_step;
#ifdef DEBUG
			memcpy(str + offset, HEXA_TO_BIN_CHARS[bytes[j]], copy_step);
#else
			uint32_t* ptr = (uint32_t*)(str + offset);
			*ptr		  = *(uint32_t*)HEXA_TO_BIN_CHARS[bytes[j]];
#endif
		}
	}

	// Do the last slice
	const size_t left_off = ((BitArray_nb_slices(array->nb_bits) - 1) * SLICE_SIZE_IN_BITS);
	for (size_t i = left_off; i < array->nb_bits; i++) {
		str[i] = (char)(BitArray_is_one(*array, i) + '0');
	}

	str[array->nb_bits] = '\0';

	return (String){
		.data	  = str,
		.size	  = array->nb_bits,
		.capacity = capacity,
	};
}

bool BitArray_equals(const BitArray* a, const BitArray* b) {
	if (a->nb_bits != b->nb_bits) {
		return false;
	}

	// Compare all slices except the last one
	size_t nb_slices = BitArray_nb_slices(a->nb_bits);
	if (nb_slices > 1) {
		for (size_t i = 0; i < nb_slices - 1; i++) {
			if (a->bits[i] != b->bits[i]) {
				return false;
			}
		}
		for (size_t i = (nb_slices - 1) * SLICE_SIZE_IN_BYTES; i < a->nb_bits; i++) {
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
	for (size_t i = 0; i < BitArray_nb_slices(array.nb_bits); i++) {
		array.bits[i] = ALL_1s;
	}
}

void BitArray_all_to_zero(BitArray array) {
	for (size_t i = 0; i < BitArray_nb_slices(array.nb_bits); i++) {
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