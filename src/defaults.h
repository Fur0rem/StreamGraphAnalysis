#ifndef DEFAULTS_H
#define DEFAULTS_H

#include "hashset.h"
#include "utils.h"
#include "vector.h"
#include <stddef.h>

DeclareCompare(size_t);
DeclareToString(size_t);
DeclareVector(size_t);
DeclareVectorDeriveRemove(size_t, NO_FREE(size_t));
DeclareVectorDeriveOrdered(size_t);
DeclareVectorDeriveToString(size_t);
DeclareHashset(size_t);
DeclareHashsetDeriveRemove(size_t, NO_FREE(size_t));
DeclareHashsetDeriveToString(size_t);
DeclareHashsetDeriveEquals(size_t);

#endif // DEFAULTS_H