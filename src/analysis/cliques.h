#ifndef CLIQUES_H
#define CLIQUES_H

#include "../generic_data_structures/arraylist.h"
#include "../stream.h"
#include "../stream_graph/links_set.h"
#include "../units.h"
#include "../utils.h"
#include <stdbool.h>

typedef struct Clique Clique;
struct Clique {
	TimeId time_start;
	TimeId time_end;
	NodeId* nodes;
	size_t nb_nodes;
};

DeclareToString(Clique);
DeclareEquals(Clique);

DeclareArrayList(Clique);
DeclareArrayListDeriveEquals(Clique);
DeclareArrayListDeriveToString(Clique);
DeclareArrayListDeriveRemove(Clique);

CliqueArrayList Stream_maximal_cliques(Stream* st);

#endif // CLIQUES_H