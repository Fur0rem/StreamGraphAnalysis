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
	BitArray bit_array = BitArray_n_zeros(10);
	BitArray_set_one(bit_array, 5);
	return EXPECT(bit_array.bits[0] == (1 << 5));
}

bool test_set_zero() {
	BitArray bit_array = BitArray_n_zeros(10);
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
	BitArray bit_array = BitArray_n_zeros(10);
	return EXPECT(!BitArray_is_one(bit_array, 5));
}

bool test_all_zeros_small() {
	bool result = true;

	BitArray bit_array = BitArray_n_zeros(5);
	for (size_t i = 0; i < 5; i++) {
		result &= EXPECT(!BitArray_is_one(bit_array, i));
	}

	return result;
}

bool test_all_ones_small() {
	bool result = true;

	BitArray bit_array = BitArray_n_ones(5);
	for (size_t i = 0; i < 5; i++) {
		result &= EXPECT(BitArray_is_one(bit_array, i));
	}

	return result;
}

bool test_all_zeros_big() {
	bool result = true;

	BitArray bit_array = BitArray_n_zeros(90);
	for (size_t i = 0; i < 90; i++) {
		result &= EXPECT(!BitArray_is_one(bit_array, i));
	}

	return result;
}

bool test_all_ones_big() {
	bool result = true;

	BitArray bit_array = BitArray_n_ones(90);
	for (size_t i = 0; i < 90; i++) {
		result &= EXPECT(BitArray_is_one(bit_array, i));
	}

	return result;
}

bool test_to_string() {
	bool result = true;

	BitArray bit_array = BitArray_n_zeros(5);
	String str		   = BitArray_to_string(&bit_array);
	result &= EXPECT(strcmp(str.data, "00000") == 0);
	String_destroy(str);

	BitArray_set_one(bit_array, 1);
	BitArray_set_one(bit_array, 3);
	str = BitArray_to_string(&bit_array);
	result &= EXPECT(strcmp(str.data, "01010") == 0);
	String_destroy(str);

	BitArray_destroy(bit_array);

	bit_array = BitArray_n_ones(100);
	str		  = BitArray_to_string(&bit_array);
	char expected[101];
	for (size_t i = 0; i < 100; i++) {
		expected[i] = '1';
	}
	expected[100] = '\0';
	result &= EXPECT(strcmp(str.data, expected) == 0);
	String_destroy(str);

	BitArray_set_zero(bit_array, 99);
	str			 = BitArray_to_string(&bit_array);
	expected[99] = '0';
	result &= EXPECT(strcmp(str.data, expected) == 0);
	printf("%s\n", str.data);
	String_destroy(str);

	bit_array = BitArray_n_ones(1 << 27);
	str		  = BitArray_to_string(&bit_array);

	char* expected2 = malloc(1 << 27 + 1);
	for (size_t i = 0; i < (1 << 27); i++) {
		expected2[i] = '1';
	}
	expected2[1 << 27] = '\0';
	result &= EXPECT(strcmp(str.data, expected2) == 0);
	String_destroy(str);
	free(expected2);

	return result;
}

bool test_leading_zeros_small_zeros() {
	bool result = true;

	BitArray bit_array = BitArray_n_zeros(5);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 0), 5);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 1), 4);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 2), 3);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 3), 2);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 4), 1);

	return result;
}

bool test_leading_zeros_small_ones() {
	bool result = true;

	BitArray bit_array = BitArray_n_ones(5);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 0), 0);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 1), 0);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 2), 0);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 3), 0);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 4), 0);

	return result;
}

bool test_leading_zeros_small_1() {
	bool result = true;

	BitArray bit_array = BitArray_n_zeros(5);
	BitArray_set_one(bit_array, 3);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 0), 3);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 1), 2);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 2), 1);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 3), 0);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 4), 1);

	return result;
}

bool test_leading_zeros_big_zeros() {
	bool result = true;

	BitArray bit_array = BitArray_n_zeros(90);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 0), 90);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 1), 89);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 2), 88);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 3), 87);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 4), 86);

	return result;
}

bool test_leading_zeros_big_ones() {
	bool result = true;

	BitArray bit_array = BitArray_n_ones(90);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 0), 0);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 1), 0);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 2), 0);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 3), 0);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 4), 0);

	return result;
}

bool test_leading_zeros_big_1() {
	bool result = true;

	BitArray bit_array = BitArray_n_zeros(90);
	BitArray_set_one(bit_array, 3);

	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 0), 3);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 1), 2);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 2), 1);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 3), 0);
	result &= EXPECT_EQ(BitArray_leading_zeros_from(bit_array, 4), 86);

	return result;
}

int main() {
	Test* tests[] = {
		TEST(test_create),
		TEST(test_set_one),
		TEST(test_set_zero),
		TEST(test_is_one),
		TEST(test_is_zero),
		TEST(test_all_zeros_small),
		TEST(test_all_ones_small),
		TEST(test_all_zeros_big),
		TEST(test_all_ones_big),
		TEST(test_leading_zeros_small_zeros),
		TEST(test_leading_zeros_small_ones),
		TEST(test_leading_zeros_small_1),
		TEST(test_leading_zeros_big_zeros),
		TEST(test_leading_zeros_big_ones),
		TEST(test_leading_zeros_big_1),
		TEST(test_to_string),
		NULL,
	};

	return test("BitArray", tests);
}