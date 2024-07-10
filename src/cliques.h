#ifndef CLIQUES_H
#define CLIQUES_H

#include "stream.h"
#include "stream_graph/links_set.h"
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

typedef struct {
	int b;
	int e;
	int u;
	int v;
} MyLink;
bool MyLink_equals(MyLink l1, MyLink l2);
char* MyLink_to_string(MyLink* l);
DefVector(MyLink, NO_FREE(MyLink));

char* char_to_string(char* c);
bool char_equals(char c1, char c2);

char* Clique_to_string(Clique* c);
bool Clique_equals(Clique c1, Clique c2);

DefVector(Clique, NO_FREE(Clique));
DefVector(Link, NO_FREE(Link));
CliqueVector maximal_cliques(LinkVector ls);

#endif // CLIQUES_H