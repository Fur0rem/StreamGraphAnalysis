#ifndef CLIQUES_H
#define CLIQUES_H

#include "stream.h"
#include "stream_graph/links_set.h"
#include "units.h"
#include "utils.h"
#include "vector.h"
#include <stdbool.h>

typedef struct Clique Clique;
struct Clique {
	TimeId time_start;
	TimeId time_end;
	NodeId* nodes;
	size_t nb_nodes;
};

typedef struct {
	int b;
	int e;
	int u;
	int v;
} MyLink;
bool MyLink_equals(MyLink l1, MyLink l2);
char* MyLink_to_string(MyLink* l);
DeclareVector(MyLink);
DeclareVectorDeriveEquals(MyLink);
DeclareVectorDeriveToString(MyLink);
DeclareVectorDeriveRemove(MyLink);

DeclareToString(Clique);
DeclareEquals(Clique);

DeclareVector(Clique);
DeclareVectorDeriveEquals(Clique);
DeclareVectorDeriveToString(Clique);
DeclareVectorDeriveRemove(Clique);

CliqueVector Stream_maximal_cliques(Stream* st);

#endif // CLIQUES_H