#include "fraction.h"

SGA_Fraction SGA_Fraction_from(size_t num, size_t den) {
	return (SGA_Fraction){.num = num, .den = den};
}

SGA_Fraction SGA_Fraction_add(SGA_Fraction a, SGA_Fraction b) {
	return SGA_Fraction_from(a.num * b.den + b.num * a.den, a.den * b.den);
}

SGA_Fraction SGA_Fraction_sub(SGA_Fraction a, SGA_Fraction b) {
	return SGA_Fraction_from(a.num * b.den - b.num * a.den, a.den * b.den);
}

SGA_Fraction SGA_Fraction_mul(SGA_Fraction a, SGA_Fraction b) {
	return SGA_Fraction_from(a.num * b.num, a.den * b.den);
}

SGA_Fraction SGA_Fraction_div(SGA_Fraction a, SGA_Fraction b) {
	return SGA_Fraction_from(a.num * b.den, a.den * b.num);
}

SGA_Fraction SGA_Fraction_simplify(SGA_Fraction a) {
	// Find the greatest common divisor
	size_t num = a.num;
	size_t den = a.den;
	size_t temp;
	while (den != 0) {
		temp = den;
		den	 = num % den;
		num	 = temp;
	}
	return SGA_Fraction_from(a.num / num, a.den / num);
}

bool SGA_Fraction_equals(const SGA_Fraction* a, const SGA_Fraction* b) {
	return a->num == b->num && a->den == b->den;
}

int SGA_Fraction_compare(const SGA_Fraction* a, const SGA_Fraction* b) {
	return a->num * b->den - b->num * a->den;
}

SGA_Fraction SGA_Fraction_neg(SGA_Fraction a) {
	return SGA_Fraction_from(-a.num, a.den);
}

SGA_Fraction SGA_Fraction_abs(SGA_Fraction a) {
	return SGA_Fraction_from(a.num < 0 ? -a.num : a.num, a.den < 0 ? -a.den : a.den);
}

SGA_Fraction SGA_Fraction_inv(SGA_Fraction a) {
	return SGA_Fraction_from(a.den, a.num);
}

size_t SGA_Fraction_to_size_t(SGA_Fraction a) {
	return a.num / a.den;
}

double SGA_Fraction_to_double(SGA_Fraction a) {
	return (double)a.num / (double)a.den;
}