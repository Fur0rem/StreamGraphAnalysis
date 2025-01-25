#define SGA_INTERNAL

#include "bit_array.h"
#include "utils.h"
#include <stddef.h>
#include <stdint.h>

// Using an int because it is the biggest type that can be used with bitwise
// shifting operations that doesn't fuck up
const size_t SLICE_SIZE_IN_BYTES = sizeof(BitArraySlice);
const size_t BYTE_SIZE		 = 8;
const size_t SLICE_SIZE_IN_BITS	 = SLICE_SIZE_IN_BYTES * BYTE_SIZE;

#define ONE    ((BitArraySlice)1)
#define ZERO   ((BitArraySlice)0)
#define ALL_1s (~((BitArraySlice)0))

size_t BitArray_nb_slices(size_t nb_bits) {
	return (nb_bits / (SLICE_SIZE_IN_BYTES * BYTE_SIZE)) + 1;
}

size_t BitArray_nb_bytes(size_t nb_bits) {
	return (nb_bits / SLICE_SIZE_IN_BYTES) + 1;
}

BitArray BitArray_with_n_bits(size_t nb_bits) {
	BitArray bit_array = (BitArray){.nb_bits = nb_bits, .bits = MALLOC(BitArray_nb_slices(nb_bits) * SLICE_SIZE_IN_BYTES)};
	return bit_array;
}

BitArray BitArray_n_zeros(size_t nb_bits) {
	BitArray bit_array = BitArray_with_n_bits(nb_bits);
	for (size_t i = 0; i < BitArray_nb_slices(nb_bits); i++) {
		bit_array.bits[i] = ZERO;
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
	return (array.bits[byte_index(index)] & (ONE << bit_index(index))) != ZERO;
}

bool BitArray_is_zero(BitArray array, size_t index) {
	return !BitArray_is_one(array, index);
}

void BitArray_set_one(BitArray array, size_t index) {
	array.bits[byte_index(index)] |= (ONE << bit_index(index));
}

void BitArray_set_zero(BitArray array, size_t index) {
	array.bits[byte_index(index)] &= ~(ONE << bit_index(index));
}

void BitArray_and_bit(BitArray array, size_t index, int value) {
	array.bits[byte_index(index)] &= value ? (ONE << bit_index(index)) : ~(ONE << bit_index(index));
}

void BitArray_or_bit(BitArray array, size_t index, int value) {
	array.bits[byte_index(index)] |= value ? (ONE << bit_index(index)) : ~(ONE << bit_index(index));
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

static const char HEXA_TO_BIN_CHARS[16][sizeof(uint32_t)] __attribute__((aligned(4))) = {
    {'0', '0', '0', '0'},
    {'1', '0', '0', '0'},
    {'0', '1', '0', '0'},
    {'1', '1', '0', '0'},
    {'0', '0', '1', '0'},
    {'1', '0', '1', '0'},
    {'0', '1', '1', '0'},
    {'1', '1', '1', '0'},
    {'0', '0', '0', '1'},
    {'1', '0', '0', '1'},
    {'0', '1', '0', '1'},
    {'1', '1', '0', '1'},
    {'0', '0', '1', '1'},
    {'1', '0', '1', '1'},
    {'0', '1', '1', '1'},
    {'1', '1', '1', '1'},
};

String BitArray_to_string(const BitArray* self) {

	const size_t copy_step = sizeof(uint32_t);

	// TODO: Add arch verification functions
	ASSERT((SLICE_SIZE_IN_BITS / copy_step) == 16);
	ASSERT((int)true == 1);
	ASSERT((int)false == 0);
	ASSERT(('0' + 1) == '1');

	size_t capacity = (BitArray_nb_slices(self->nb_bits) * SLICE_SIZE_IN_BITS) + 1;
	// round capacity up to nearest multiple of copy_step
	capacity += copy_step - (capacity % copy_step);
	char* str = aligned_alloc(copy_step, capacity);

	for (size_t i = 0; i < BitArray_nb_slices(self->nb_bits) - 1; i++) {
		// gcc t'es con MASK_SIZE c'est sizeof(BitArraySlice) mais il veut pas sinon
		// "VaRiABle LeNGtH ArRAyS" meme si MASK_SIZE est const
		//[SLICE_SIZE_IN_BITS / copy_step];
		const unsigned char bytes[16] = {
		    self->bits[i] & 0xF,
		    (self->bits[i] >> 4) & 0xF,
		    (self->bits[i] >> 8) & 0xF,
		    (self->bits[i] >> 12) & 0xF,
		    (self->bits[i] >> 16) & 0xF,
		    (self->bits[i] >> 20) & 0xF,
		    (self->bits[i] >> 24) & 0xF,
		    (self->bits[i] >> 28) & 0xF,
		    (self->bits[i] >> 32) & 0xF,
		    (self->bits[i] >> 36) & 0xF,
		    (self->bits[i] >> 40) & 0xF,
		    (self->bits[i] >> 44) & 0xF,
		    (self->bits[i] >> 48) & 0xF,
		    (self->bits[i] >> 52) & 0xF,
		    (self->bits[i] >> 56) & 0xF,
		    (self->bits[i] >> 60) & 0xF,
		};

		for (size_t j = 0; j < SLICE_SIZE_IN_BITS / copy_step; j++) {
			size_t offset = i * SLICE_SIZE_IN_BITS + j * copy_step;
			uint32_t* ptr = (uint32_t*)(str + offset);
			*ptr	      = *(uint32_t*)HEXA_TO_BIN_CHARS[bytes[j]];
		}
	}

	// Do the last slice
	const size_t left_off = ((BitArray_nb_slices(self->nb_bits) - 1) * SLICE_SIZE_IN_BITS);
	for (size_t i = left_off; i < self->nb_bits; i++) {
		str[i] = (char)(BitArray_is_one(*self, i) + '0');
	}

	str[self->nb_bits] = '\0';

	return (String){
	    .data     = str,
	    .size     = self->nb_bits,
	    .capacity = capacity,
	};
}

bool BitArray_equals(const BitArray* left, const BitArray* right) {
	if (left->nb_bits != right->nb_bits) {
		return false;
	}

	// Compare all slices except the last one
	size_t nb_slices = BitArray_nb_slices(left->nb_bits);
	if (nb_slices > 1) {
		for (size_t i = 0; i < nb_slices - 1; i++) {
			if (left->bits[i] != right->bits[i]) {
				return false;
			}
		}
		for (size_t i = (nb_slices - 1) * SLICE_SIZE_IN_BYTES; i < left->nb_bits; i++) {
			if (BitArray_is_one(*left, i) != BitArray_is_one(*right, i)) {
				return false;
			}
		}
	}
	else {
		for (size_t i = 0; i < left->nb_bits; i++) {
			if (BitArray_is_one(*left, i) != BitArray_is_one(*right, i)) {
				return false;
			}
		}
	}
	return true;
}

// Returns the number of leading zeros in the bit array starting from the given
// index NOTE: GCC and Clang are smart enough to optimise this function using
// the lzcnt instruction (benchmarked with -O3)
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
		array.bits[i] = ZERO;
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