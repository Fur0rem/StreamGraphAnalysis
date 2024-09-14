#ifndef DEFAULTS_H
#define DEFAULTS_H

#include "generic_data_structures/arraylist.h"
#include "generic_data_structures/binary_heap.h"
#include "generic_data_structures/hashset.h"
#include "utils.h"
#include <stddef.h>

#define DeclareFullDerivedArrayList(T)                                                                                                     \
	DeclareArrayList(T);                                                                                                                   \
	DeclareArrayListDeriveRemove(T);                                                                                                       \
	DeclareArrayListDeriveEquals(T);                                                                                                       \
	DeclareArrayListDeriveOrdered(T);                                                                                                      \
	DeclareArrayListDeriveToString(T);

#define DefineFullDerivedArrayList(T)                                                                                                      \
	DefineArrayList(T);                                                                                                                    \
	DefineArrayListDeriveRemove(T);                                                                                                        \
	DefineArrayListDeriveEquals(T);                                                                                                        \
	DefineArrayListDeriveOrdered(T);                                                                                                       \
	DefineArrayListDeriveToString(T);

#define DeclareFullDerivedHashset(T)                                                                                                       \
	DeclareHashset(T);                                                                                                                     \
	DeclareHashsetDeriveRemove(T);                                                                                                         \
	DeclareHashsetDeriveToString(T);                                                                                                       \
	DeclareHashsetDeriveEquals(T);

#define DefineFullDerivedHashset(T)                                                                                                        \
	DefineHashset(T);                                                                                                                      \
	DefineHashsetDeriveRemove(T);                                                                                                          \
	DefineHashsetDeriveToString(T);                                                                                                        \
	DefineHashsetDeriveEquals(T);

DeclareCompare(size_t);
DeclareToString(size_t);
DeclareFullDerivedArrayList(size_t);
DeclareFullDerivedHashset(size_t);
DeclareBinaryHeap(size_t);

DeclareCompare(int);
DeclareToString(int);
DeclareFullDerivedArrayList(int);
DeclareFullDerivedHashset(int);
DeclareBinaryHeap(int);

#endif // DEFAULTS_H