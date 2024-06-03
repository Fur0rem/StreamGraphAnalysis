#ifndef UTILS_H
#define UTILS_H

#define TEXT_BOLD "\033[1m"
#define TEXT_RED "\033[31m"
#define TEXT_GREEN "\033[32m"
#define TEXT_RESET "\033[0m"

#include <stdbool.h>
#include <math.h>

#define F_EQUALS(a, b) (fabs((a) - (b)) < 0.0001)

#endif