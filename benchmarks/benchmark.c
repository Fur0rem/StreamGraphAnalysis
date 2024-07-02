#include "benchmark.h"
#include <stdio.h>
#include <time.h>

DONT_OPTIMISE void benchmark(void (*function)(), const char* name, int iterations) {
	clock_t start, end;
	double cpu_time_used;
	start = clock();
	for (int i = 0; i < iterations; i++) {
		function();
	}
	end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("%s : %fs for %d iterations (%f per iteration)\n", name, cpu_time_used, iterations,
		   cpu_time_used / iterations);
}