#ifndef SGA_FRACTION_H
#define SGA_FRACTION_H

#include "defaults.h"
#include "utils.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
	size_t num;
	size_t den;
} SGA_Fraction;

SGA_Fraction SGA_Fraction_from(size_t num, size_t den);
SGA_Fraction SGA_Fraction_add(SGA_Fraction a, SGA_Fraction b);
SGA_Fraction SGA_Fraction_sub(SGA_Fraction a, SGA_Fraction b);
SGA_Fraction SGA_Fraction_mul(SGA_Fraction a, SGA_Fraction b);
SGA_Fraction SGA_Fraction_div(SGA_Fraction a, SGA_Fraction b);
SGA_Fraction SGA_Fraction_simplify(SGA_Fraction a);

DeclareEquals(SGA_Fraction);
DeclareToString(SGA_Fraction);
DeclareCompare(SGA_Fraction);

SGA_Fraction SGA_Fraction_neg(SGA_Fraction a);
SGA_Fraction SGA_Fraction_abs(SGA_Fraction a);
SGA_Fraction SGA_Fraction_inv(SGA_Fraction a);
size_t SGA_Fraction_to_size_t(SGA_Fraction a);
double SGA_Fraction_to_double(SGA_Fraction a);

#endif // SGA_FRACTION_H