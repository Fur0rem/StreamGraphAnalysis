#include "../src/bit_array.h"
#include "../src/utils.h"
#include "test.h"

#include <stdint.h>
#include <stdlib.h>

bool test_create() {
	BitArray bit_array = BitArray_with_n_bits(10);
	return EXPECT_EQ(BitArray_size(bit_array), 10);
}

bool test_set_one() {
	BitArray bit_array = BitArray_with_n_bits(10);
	BitArray_set_one(bit_array, 5);
	char* str = BitArray_to_string(bit_array);
	return EXPECT(bit_array[1] == (1 << 5));
}

bool test_set_zero() {
	BitArray bit_array = BitArray_with_n_bits(10);
	BitArray_set_one(bit_array, 5);
	BitArray_set_zero(bit_array, 5);
	return EXPECT(bit_array[1] == 0);
}

bool test_is_one() {
	BitArray bit_array = BitArray_with_n_bits(10);
	BitArray_set_one(bit_array, 5);
	return EXPECT(BitArray_is_one(bit_array, 5));
}

bool test_is_zero() {
	BitArray bit_array = BitArray_with_n_bits(10);
	return EXPECT(!BitArray_is_one(bit_array, 5));
}

int main() {
	Test* tests[] = {
		&(Test){"create",	  test_create	 },
		&(Test){"set_one",  test_set_one },
		  &(Test){"set_zero", test_set_zero},
		&(Test){"is_one",	  test_is_one	 },
		&(Test){"is_zero",  test_is_zero },
		  NULL
	  };

	return test("BitArray", tests);
}