#include "../src/bit_array.h"
#include "../src/utils.h"
#include "test.h"

#include <stdint.h>
#include <stdlib.h>

bool test_create() {
	BitArray bit_array = BitArray_with_n_bits(10);
	return EXPECT_EQ(bit_array.nb_bits, 10);
}

bool test_set_one() {
	BitArray bit_array = BitArray_with_n_bits(10);
	BitArray_set_one(bit_array, 5);
	char* str = BitArray_to_string(bit_array);
	return EXPECT(bit_array.bits[0] == (1 << 5));
}

bool test_set_zero() {
	BitArray bit_array = BitArray_with_n_bits(10);
	BitArray_set_one(bit_array, 5);
	BitArray_set_zero(bit_array, 5);
	return EXPECT(bit_array.bits[0] == 0);
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

bool test_all_zeros_small() {
	BitArray bit_array = BitArray_n_zeros(5);
	for (size_t i = 0; i < 5; i++) {
		EXPECT(!BitArray_is_one(bit_array, i));
	}
	return true;
}

bool test_all_ones_small() {
	BitArray bit_array = BitArray_n_ones(5);
	for (size_t i = 0; i < 5; i++) {
		EXPECT(BitArray_is_one(bit_array, i));
	}
	return true;
}

bool test_all_zeros_big() {
	BitArray bit_array = BitArray_n_zeros(90);
	for (size_t i = 0; i < 90; i++) {
		EXPECT(!BitArray_is_one(bit_array, i));
	}
	return true;
}

bool test_all_ones_big() {
	BitArray bit_array = BitArray_n_ones(90);
	for (size_t i = 0; i < 90; i++) {
		EXPECT(BitArray_is_one(bit_array, i));
	}
	return true;
}

int main() {
	Test* tests[] = {
		&(Test){"create",		  test_create		 },
		&(Test){"set_one",		   test_set_one		   },
		&(Test){"set_zero",		test_set_zero		 },
		&(Test){"is_one",		  test_is_one		 },
		&(Test){"is_zero",		   test_is_zero		   },
		&(Test){"all_zeros_small", test_all_zeros_small},
		&(Test){"all_ones_small",  test_all_ones_small },
		&(Test){"all_zeros_big",	 test_all_zeros_big  },
		&(Test){"all_ones_big",	test_all_ones_big	 },
		NULL
	};

	return test("BitArray", tests);
}