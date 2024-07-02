#ifndef BENCHMARK_H
#define BENCHMARK_H

/// Macro to prevent the compiler from optimising the function
// If the compiler is Clang
#ifdef __clang__
#	define DONT_OPTIMISE __attribute__((optnone))
// Else, check if the compiler is GCC
#elif defined(__GNUC__)
#	define DONT_OPTIMISE __attribute__((optimize("O0")))
#else
#	define DONT_OPTIMISE
#endif

DONT_OPTIMISE void benchmark(void (*function)(void), const char* name, int iterations);

#endif // BENCHMARK_H