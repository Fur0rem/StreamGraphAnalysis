#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "../src/utils.h"

DONT_OPTIMISE void benchmark(void (*function)(void), const char* name, int iterations);

#endif // BENCHMARK_H