#include <stdbool.h>
#include <stddef.h>

typedef size_t* SGA_BitArray; // The first element contains the number of elements in the array,
							  // then the elements

SGA_BitArray SGA_BitArray_with_n_bits(size_t n_bits);
SGA_BitArray SGA_BitArray_n_zeros(size_t n_bits);
SGA_BitArray SGA_BitArray_n_ones(size_t n_bits);
bool SGA_BitArray_is_one(SGA_BitArray array, size_t index);
void SGA_BitArray_set_one(SGA_BitArray array, size_t index);
void SGA_BitArray_set_zero(SGA_BitArray array, size_t index);
size_t SGA_BitArray_size(SGA_BitArray array);
void SGA_BitArray_destroy(SGA_BitArray array);
char* SGA_BitArray_to_string(SGA_BitArray array);