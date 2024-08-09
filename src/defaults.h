#ifndef DEFAULTS_H
#define DEFAULTS_H

#include "hashset.h"
#include "utils.h"
#include "vector.h"
#include <stddef.h>

#define DeclareFullDerivedVector(T)                                                                                    \
	DeclareVector(T);                                                                                                  \
	DeclareVectorDeriveRemove(T);                                                                                      \
	DeclareVectorDeriveOrdered(T);                                                                                     \
	DeclareVectorDeriveToString(T);

#define DefineFullDerivedVector(T, free_func)                                                                          \
	DefineVector(T);                                                                                                   \
	DefineVectorDeriveRemove(T, free_func);                                                                            \
	DefineVectorDeriveOrdered(T);                                                                                      \
	DefineVectorDeriveToString(T);

#define DeclareFullDerivedHashset(T)                                                                                   \
	DeclareHashset(T);                                                                                                 \
	DeclareHashsetDeriveRemove(T);                                                                                     \
	DeclareHashsetDeriveToString(T);                                                                                   \
	DeclareHashsetDeriveEquals(T);

#define DefineFullDerivedHashset(T, free_func)                                                                         \
	DefineHashset(T);                                                                                                  \
	DefineHashsetDeriveRemove(T, free_func);                                                                           \
	DefineHashsetDeriveToString(T);                                                                                    \
	DefineHashsetDeriveEquals(T);

DeclareCompare(size_t);
DeclareToString(size_t);
DeclareFullDerivedVector(size_t);
DeclareFullDerivedHashset(size_t);

DeclareCompare(int);
DeclareToString(int);
DeclareFullDerivedVector(int);
DeclareFullDerivedHashset(int);

#endif // DEFAULTS_H