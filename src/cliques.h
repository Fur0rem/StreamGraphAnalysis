#ifndef CLIQUES_H
#define CLIQUES_H

#include "stream.h"
#include "units.h"
#include "utils.h"
#include "vector.h"
#include <stdbool.h>

typedef struct {
	TimeId time_start;
	TimeId time_end;
	NodeId* nodes;
	size_t nb_nodes;
} Clique;

char* char_to_string(char* c);
bool char_equals(char c1, char c2);

char* Clique_to_string(Clique* c);
bool Clique_equals(Clique c1, Clique c2);

DefVector(Clique, NO_FREE(Clique));

CliqueVector maximal_cliques(Stream* st);

#endif // CLIQUES_H