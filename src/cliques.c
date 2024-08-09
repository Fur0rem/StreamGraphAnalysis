#include "cliques.h"
#include "bit_array.h"
#include "defaults.h"
#include "hashset.h"
#include "interval.h"
#include "iterators.h"
#include "stream.h"
#include "stream/chunk_stream.h"
#include "stream/chunk_stream_small.h"
#include "stream/full_stream_graph.h"
#include "stream/link_stream.h"
#include "stream_functions.h"
#include "stream_graph/links_set.h"
#include "units.h"
#include "utils.h"
#include "vector.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct LinkPresence LinkPresence;
struct LinkPresence {
	size_t start;
	size_t end;
	size_t nodes[2];
};

DeclareVector(LinkPresence);
DefineVector(LinkPresence);
DefineVectorDeriveRemove(LinkPresence, NO_FREE(LinkPresence));

bool LinkPresence_equals(LinkPresence* l1, LinkPresence* l2) {
	return l1->start == l2->start && l1->end == l2->end && l1->nodes[0] == l2->nodes[0] && l1->nodes[1] == l2->nodes[1];
}
DefineVectorDeriveEquals(LinkPresence);

int LinkPresence_compare(const LinkPresence* l1, const LinkPresence* l2) {
	return l1->start - l2->start;
}

DefineVectorDeriveOrdered(LinkPresence);

bool Clique_equals(const Clique* c1, const Clique* c2) {
	if (c1->time_start != c2->time_start || c1->time_end != c2->time_end || c1->nb_nodes != c2->nb_nodes) {
		return false;
	}
	for (size_t i = 0; i < c1->nb_nodes; i++) {
		if (c1->nodes[i] != c2->nodes[i]) {
			return false;
		}
	}
	return true;
}

String Clique_to_string(const Clique* c) {
	String str = String_from_duplicate("Clique [");
	char time_str[20];
	sprintf(time_str, "%zu -> %zu] ", c->time_start, c->time_end);
	String_push_str(&str, time_str);
	String_push(&str, '[');
	for (size_t i = 0; i < c->nb_nodes; i++) {
		char node_str[20];
		sprintf(node_str, "%zu", c->nodes[i]);
		String_push_str(&str, node_str);
		if (i < c->nb_nodes - 1) {
			String_push(&str, ',');
			String_push(&str, ' ');
		}
	}
	String_push(&str, ']');
	return str;
}

DefineVector(Clique);
DefineVectorDeriveRemove(Clique, NO_FREE(Clique));
DefineVectorDeriveEquals(Clique);
DefineVectorDeriveToString(Clique);

void swap(size_t* tab, size_t i, size_t j) {
	size_t tmp = tab[i];
	tab[i] = tab[j];
	tab[j] = tmp;
}

typedef struct {
	// int n;      // number of nodes
	size_t* XPnode;	 // nodes of X,P structure
	size_t* XPindex; // index of nodes in X,P structure
	size_t* beginX;
	size_t* beginP;
	size_t* endP;
	size_t** timeEndRNode; // timeEndRNode[depth][w] : end time of R U {w}
	size_tVector R;
	size_tVector* candidates_to_iterate;

	// FOR ITERATIVE CODE
	size_t* i; // i[depth] = indice courant dans candidates_to_iterate[depth]
	size_t* e; // e[depth] temps de fin de R à depth
} XPR;

XPR* allocXPR(size_t n, size_t depthMax) {
	XPR* xpr = MALLOC(sizeof(XPR));

	// FOR ITERATIVE CODE
	xpr->i = (size_t*)calloc(depthMax, sizeof(size_t)); // TODO : safe calloc?
	xpr->e = MALLOC(depthMax * sizeof(size_t));

	// xpr->n = 0;
	xpr->XPnode = MALLOC(n * sizeof(size_t));
	xpr->XPindex = MALLOC(n * sizeof(size_t));

	for (size_t i = 0; i < n; i++) {
		xpr->XPnode[i] = i;
		xpr->XPindex[i] = i;
	}
	xpr->beginX = MALLOC(depthMax * sizeof(size_t));
	xpr->beginP = MALLOC(depthMax * sizeof(size_t));
	xpr->endP = MALLOC(depthMax * sizeof(size_t));
	xpr->timeEndRNode = MALLOC(depthMax * sizeof(size_t*));

	xpr->R = size_tVector_with_capacity(depthMax);
	xpr->candidates_to_iterate = MALLOC(depthMax * sizeof(size_tVector));

	for (size_t depth = 0; depth < depthMax; depth++) {
		xpr->timeEndRNode[depth] = MALLOC(n * sizeof(size_t));
		xpr->candidates_to_iterate[depth] = size_tVector_with_capacity(depthMax);
	}
	return xpr;
}

bool is_in_P(XPR* xpr, size_t w, size_t depth) {
	return xpr->XPindex[w] >= xpr->beginP[depth] && xpr->XPindex[w] < xpr->endP[depth];
}

bool is_in_X(XPR* xpr, size_t w, size_t depth) {
	return xpr->XPindex[w] >= xpr->beginX[depth] && xpr->XPindex[w] < xpr->beginP[depth];
}

bool is_in_PUX(XPR* xpr, size_t w, size_t depth) {
	return xpr->XPindex[w] >= xpr->beginX[depth] && xpr->XPindex[w] < xpr->endP[depth];
}

bool is_in_R(XPR* xpr, size_t w) {
	return size_tVector_contains(xpr->R, w);
}

void add_to_P(XPR* xpr, size_t w, size_t depth) {

	size_t iw = xpr->XPindex[w];
	size_t u = xpr->XPnode[xpr->endP[depth]];

	swap(xpr->XPnode, iw, xpr->endP[depth]);
	swap(xpr->XPindex, w, u);
	xpr->endP[depth]++;
}

void swap_from_P_to_X(XPR* xpr, size_t w, size_t depth) {
	ASSERT(is_in_P(xpr, w, depth));

	size_t iw = xpr->XPindex[w];
	size_t iu = xpr->beginP[depth];
	size_t u = xpr->XPnode[iu];
	swap(xpr->XPnode, iw, iu);
	swap(xpr->XPindex, w, u);
	xpr->beginP[depth]++;
}

void swap_from_X_to_P(XPR* xpr, size_t w, size_t depth) {
	ASSERT(is_in_X(xpr, w, depth));

	xpr->beginP[depth]--;
	size_t iw = xpr->XPindex[w];
	size_t iu = xpr->beginP[depth];
	size_t u = xpr->XPnode[iu];
	swap(xpr->XPnode, iw, iu);
	swap(xpr->XPindex, w, u);
}

void add_to_X0(XPR* xpr, size_t w, size_t depth) {
	add_to_P(xpr, w, depth);
	swap_from_P_to_X(xpr, w, depth);
}

void keep_in_X(XPR* xpr, size_t w, size_t depth) {
	size_t iw = xpr->XPindex[w];
	size_t u = xpr->XPnode[--xpr->beginX[depth]];
	swap(xpr->XPnode, iw, xpr->beginX[depth]);
	swap(xpr->XPindex, w, u);
}

void swap_P_node_position(XPR* xpr, size_t u, size_t iv) {
	size_t iu = xpr->XPindex[u];
	size_t v = xpr->XPnode[iv];
	swap(xpr->XPnode, iu, iv);
	swap(xpr->XPindex, u, v);
}

size_t min(size_t a, size_t b) {
	return a < b ? a : b;
}

size_t max(size_t a, size_t b) {
	return a > b ? a : b;
}

void init_timeEndRNode(XPR* xpr, size_t w, size_t e1, size_t e2, size_t e3, size_t depth) {
	xpr->timeEndRNode[depth][w] = min(min(e1, e2), e3);
}

void update_timeEndRNode(XPR* xpr, size_t v, size_t e, size_t depth) {
	size_t e1 = xpr->timeEndRNode[depth - 1][v];
	xpr->timeEndRNode[depth][v] = min(e, e1);
}

void print_P(XPR* xpr, size_t depth) {
	printf("P = ");
	for (size_t i = xpr->beginP[depth]; i < xpr->endP[depth]; i++) {
		printf("%zu ", xpr->XPnode[i]);
	}
	printf("\n");
}

void print_X(XPR* xpr, size_t depth) {
	printf("X = ");
	for (size_t i = xpr->beginX[depth]; i < xpr->beginP[depth]; i++) {
		printf("%zu ", xpr->XPnode[i]);
	}
	printf("\n");
}

void print_R(XPR* xpr) {
	// cerr << "R = ";
	printf("R = ");
	String str = size_tVector_to_string(&xpr->R);
	printf("%s\n", str.data);
	String_destroy(str);
}

typedef struct {
	size_t* n;	  // number of neighbors of subgraph at depth
	size_t* node; // list of neighbors
				  // int *beginX; // in [depth] from beginX to beginP-1 : neighbors INTER X
				  // int *beginP; // in [depth] from beginP to endP-1 : neighbors INTER P
				  // int *endP;
} NeighborList;

NeighborList* alloc_NeighborList(size_t degreeMax, size_t depthMax) {
	NeighborList* Nl = MALLOC(sizeof(NeighborList));
	Nl->n = MALLOC(depthMax * sizeof(size_t));
	Nl->n[0] = 0;
	Nl->node = MALLOC(degreeMax * sizeof(size_t));
	return Nl;
}

void add_NeighborList(NeighborList* Nl, size_t u) {
	// Les tailles sont allouées à l'avance : ne peut pas dépasser
	// ajout d'un noeud seulement pour depth == 0
	Nl->node[Nl->n[0]] = u;
	Nl->n[0]++;
}

void remove_ind_NeighborList(NeighborList* Nl, size_t i, size_t depth) {
	// ASSERT(i < Nl->n[depth]);
	swap(Nl->node, i, --Nl->n[depth]);
}

void clear_NeighborList(NeighborList* Nl) {
	// clear seulement pour depth == 0
	Nl->n[0] = 0;
}

bool is_in_NeighborList(size_t u, NeighborList* Nl, size_t depth) {
	// check if there is u between indexes i and j
	for (size_t i = 0; i < Nl->n[depth]; i++) {
		if (Nl->node[i] == u) {
			return true;
		}
	}
	return false;
}

void keep_in_NeighborList(size_t i, NeighborList* Nl, size_t depth) {
	ASSERT(i >= Nl->n[depth]);
	swap(Nl->node, i, Nl->n[depth]);
	Nl->n[depth]++;
}

void reorderS_P(XPR* xpr, NeighborList** S, size_t depth) {
	size_t i, j, u, v;

	for (i = xpr->beginP[depth]; i < xpr->endP[depth]; i++) {
		u = xpr->XPnode[i];
		ASSERT(xpr->XPnode[xpr->XPindex[u]] == u);

		S[u]->n[depth] = 0;

		for (j = 0; j < S[u]->n[depth - 1]; j++) {
			v = S[u]->node[j];
			if (is_in_P(xpr, v, depth)) {
				keep_in_NeighborList(j, S[u], depth);
			}
		}
	}
}

void print_NeighborList_nodes(NeighborList* Nl, size_t depth) {
	for (size_t i = 0; i < Nl->n[depth]; i++) {
		printf("%zu", Nl->node[i]);
	}
	printf("\n");
}

typedef struct {
	size_t* n;	  // number of neighbors of subgraph at depth
	size_t* node; // list of neighbors
	size_t* end;  // list of end time of neighbors
} NeighborListEnd;

NeighborListEnd* alloc_NeighborListEnd(size_t degreeMax, size_t depthMax) {
	NeighborListEnd* Nle = MALLOC(sizeof(NeighborListEnd));
	Nle->n = MALLOC(degreeMax * sizeof(size_t));
	Nle->n[0] = 0;
	Nle->node = MALLOC(degreeMax * sizeof(size_t));
	Nle->end = MALLOC(degreeMax * sizeof(size_t));
	return Nle;
}

void add_NeighborListEnd(NeighborListEnd* Nle, size_t u, size_t e) {
	// ajout seulement pour depth == 0
	Nle->node[Nle->n[0]] = u;
	Nle->end[Nle->n[0]] = e;
	Nle->n[0]++;
}

void remove_node_NeighborListEnd(NeighborListEnd* Nle, size_t u) {
	// suppression d'un noeud seulement pour depth == 0
	for (size_t i = 0; i < Nle->n[0]; i++) {
		if (Nle->node[i] == u) {
			Nle->n[0]--;
			Nle->node[i] = Nle->node[Nle->n[0]];
			Nle->end[i] = Nle->end[Nle->n[0]];
			return;
		}
	}
}

void remove_ind_NeighborListEnd(NeighborListEnd* Nle, size_t i, size_t depth) {
	ASSERT(i < Nle->n[depth]);
	swap(Nle->node, i, --Nle->n[depth]);
	swap(Nle->end, i, Nle->n[depth]);
}

size_t ind_in_NeighborListEnd(size_t u, NeighborListEnd* Nle, size_t depth) {
	for (size_t i = 0; i < Nle->n[depth]; i++) {
		if (Nle->node[i] == u) {
			return i;
		}
	}
	return SIZE_MAX;
}

void keep_in_NeighborListEnd(size_t i, NeighborListEnd* Nle, size_t depth) {
	ASSERT(i >= Nle->n[depth]);
	swap(Nle->node, i, Nle->n[depth]);
	swap(Nle->end, i, Nle->n[depth]);
	Nle->n[depth]++;
}

void reorderN_XUP(XPR* xpr, NeighborListEnd** N, size_t depth) {
	for (size_t i = xpr->beginX[depth]; i < xpr->endP[depth]; i++) {

		size_t u = xpr->XPnode[i];
		ASSERT(xpr->XPnode[xpr->XPindex[u]] == u);

		N[u]->n[depth] = 0;

		for (size_t j = 0; j < N[u]->n[depth - 1]; j++) {
			size_t v = N[u]->node[j];
			if (is_in_PUX(xpr, v, depth)) {
				keep_in_NeighborListEnd(j, N[u], depth);
			}
		}
	}
}

void print_NeighborListEnd_nodes(NeighborListEnd* Nle, size_t depth) {
	// affiche les voisins entre les indices i et j
	for (size_t i = 0; i < Nle->n[depth]; i++) {
		printf("%zu ", Nle->node[i]);
	}
	printf("\n");
}

typedef struct {
	size_t n;	  // number of elements in set
	size_t* list; // elements in set
	BitArray tab; // tab[i]==1 iff i is in list
} MySet;

MySet* allocMySet(size_t init_size) {
	MySet* s = MALLOC(sizeof(MySet));
	s->n = 0;
	s->list = MALLOC(init_size * sizeof(size_t));
	// s->tab = (bool*)calloc(init_size, sizeof(bool)); // TODO: safe calloc?
	s->tab = BitArray_n_zeros(init_size);
	return s;
}

bool isInMySet(MySet* s, size_t x) {
	return BitArray_is_one(s->tab, x);
}

void addToMySet(MySet* s, size_t x) {
	if (BitArray_is_zero(s->tab, x)) {
		s->list[s->n] = x;
		s->n += 1;
		BitArray_set_one(s->tab, x);
	}
}

void clearMySet(MySet* s) {
	BitArray_all_to_zero(s->tab);
	s->n = 0;
}

typedef struct {
	size_t m;
	LinkPresence* link;
	size_tVector timesteps;
} LinkPresenceStream;

int endlink_sorter(const LinkPresence* l1, const LinkPresence* l2) {
	return l1->end - l2->end;
}

LinkPresenceStream* allocLinkStream_end_from_links(LinkPresenceVector links, size_t m, size_t ntimestep) {

	LinkPresenceStream* ls_end = MALLOC(sizeof(LinkPresenceStream));

	// Init link stream data structures
	ls_end->m = m;
	ls_end->link = MALLOC(2 * m * sizeof(LinkPresence)); // double to add end

	LinkPresence* add_link = MALLOC(m * sizeof(LinkPresence));
	LinkPresence* del_link = MALLOC(m * sizeof(LinkPresence));

	ls_end->timesteps = size_tVector_with_capacity(ntimestep);
	size_t im = 0;

	size_t old_b = SIZE_MAX;
	for (size_t l = 0; l < links.size; l++) {
		size_t b = links.array[l].start;
		if (b > old_b || old_b == SIZE_MAX) {
			size_tVector_push_unchecked(&ls_end->timesteps, links.array[l].start);
		}
		old_b = b;

		add_link[im] = links.array[l];

		del_link[im] = links.array[l];
		del_link[im].start = SIZE_MAX;

		im++;
	}

	ASSERT(im == m);

	qsort(del_link, m, sizeof(LinkPresence), (int (*)(const void*, const void*)) & endlink_sorter);

	size_t ia = 0;
	size_t id = 0;
	size_t i = 0;
	while (ia < m) {
		if (add_link[ia].start <= del_link[id].end) {
			ls_end->link[i++] = add_link[ia++];
		}
		else {
			ls_end->link[i++] = del_link[id++];
		}
	}
	ls_end->m = i;

	free(add_link);
	free(del_link);

	return ls_end;
}

typedef struct {
	size_t n;
	size_t m;
	size_t ntimestep;
	size_t degmax;
	LinkPresenceStream* ls_end;
	NeighborListEnd** N;   // Neighbors of each node
	NeighborList** S;	   // Seen edges which must not belong to max cliques anymore
	MySet* Snodes;		   // Nodes of seen edges
	size_tVector NewEdges; // Edges at a given time : have to start max cliques at this time
	XPR* xpr;			   // X P R bron kerbosh datastructure
} Datastructure;

Datastructure* allocDatastrucure_from_links(LinkPresenceVector links) {
	NeighborListEnd** N;
	NeighborList** S;
	MySet* Snodes;
	size_tVector NewEdges;
	XPR* xpr;

	size_t b;
	size_t nmin = SIZE_MAX;
	size_t nreal = 0;
	size_t mt = 0, mtmax = 0; // nombre max d'arêtes par unité de temps
	size_t old_b;
	size_t *degreeT, *degreeMax;
	size_t dmax, dsum;
	bool first = true;

	size_t m = 0;
	size_t ntimestep = 0;
	size_t n = 0;
	for (size_t i = 0; i < links.size; i++) {
		LinkPresence* link = &links.array[i];
		m++;
		b = link->start;

		if (first) {
			old_b = b;
			ntimestep++;
			first = false;
		}

		ASSERT(b >= old_b);

		if (b > old_b) {
			ntimestep++;
			mtmax = max(mt, mtmax);
			mt = 1;
		}
		else {
			mt++;
		}

		old_b = b;

		size_t u = link->nodes[0];
		size_t v = link->nodes[1];
		ASSERT(u < v);

		// node max / min
		if (nmin == SIZE_MAX) {
			nmin = u;
		}
		nmin = min(min(u, v), nmin);
		n = max(max(u, v), n);
	}

	ASSERT(b == old_b);
	mt++;

	if (mt > mtmax) {
		mtmax = mt;
	}

	// Init data
	degreeT = calloc(n + 1, sizeof(size_t));
	degreeMax = calloc(n + 1, sizeof(size_t));
	N = MALLOC((n + 1) * sizeof(NeighborListEnd*));
	S = MALLOC((n + 1) * sizeof(NeighborList*));
	Snodes = allocMySet(n + 1);
	NewEdges = size_tVector_with_capacity(3 * mtmax);

	LinkPresenceStream* ls_end = allocLinkStream_end_from_links(links, m, ntimestep);

	ASSERT(ls_end->timesteps.size == ntimestep);

	// Fill degree
	for (size_t i = 0; i < links.size; i++) {
		b = links.array[i].start;
		size_t u = links.array[i].nodes[0];
		size_t v = links.array[i].nodes[1];
		if (b == SIZE_MAX) { // end link
			degreeT[u]--;
			degreeT[v]--;
		}
		else { // new link
			degreeT[u]++;
			degreeT[v]++;
			degreeMax[u] = max(degreeT[u], degreeMax[u]);
			degreeMax[v] = max(degreeT[v], degreeMax[v]);
		}
	}

	// Init N and S
	dmax = 0;
	dsum = 0;
	for (size_t u = 0; u <= n; u++) {
		if (degreeMax[u] > 0) {
			N[u] = alloc_NeighborListEnd(degreeMax[u], degreeMax[u]);
			S[u] = alloc_NeighborList(degreeMax[u], degreeMax[u]);
			dsum += degreeMax[u];
			nreal += 1;
			dmax = max(degreeMax[u], dmax);
		}
	}

	xpr = allocXPR(n + 1, dmax + 1);

	Datastructure* d = MALLOC(sizeof(Datastructure));

	d->n = n;
	d->m = m;
	d->ntimestep = ntimestep;
	d->degmax = dmax;
	d->ls_end = ls_end;
	d->N = N;
	d->S = S;
	d->Snodes = Snodes;
	d->NewEdges = NewEdges;
	d->xpr = xpr;

	return d;
}

size_t choose_pivot(XPR* xpr, NeighborListEnd** N, size_t depth) {
	size_t dmax = 0;
	size_t pmax;

	for (size_t ip = xpr->beginX[depth]; ip < xpr->endP[depth]; ip++) {
		size_t p = xpr->XPnode[ip];
		size_t dp = 0;
		size_t epR = xpr->timeEndRNode[depth][p];

		// compter les noeuds à enlever

		for (size_t i = 0; i < N[p]->n[depth]; i++) {
			size_t u = N[p]->node[i];
			if (is_in_P(xpr, u, depth)) {
				size_t eup = N[p]->end[i];
				size_t euR = xpr->timeEndRNode[depth][u];

				if (euR <= min(epR, eup)) {
					dp++;
				}
			}
		}

		if (dp >= dmax) {
			pmax = p;
			dmax = dp;
		}
	}

	return pmax;
}

typedef struct {
	size_t maxCliqueSize;
	size_t nMaxCliques;
	size_t nTimeMaxCliques;
	size_t nLeafGood;
	size_t nLeafBad;
} MyCounter;

MyCounter* alloc_MyCounter() {
	MyCounter* mc = (MyCounter*)malloc(sizeof(MyCounter));
	mc->maxCliqueSize = 0;
	mc->nMaxCliques = 0;
	mc->nTimeMaxCliques = 0;
	mc->nLeafBad = 0;
	mc->nLeafGood = 0;
	return mc;
}

void BKtemporal(XPR* xpr, size_t b, size_t e, NeighborListEnd** N, NeighborList** S, MyCounter* mc, size_t depth,
				CliqueVector* cliques) {

	// Node of search tree
	mc->nTimeMaxCliques++;

	bool R_is_max = true;
	bool BK_not_called = true;

	if (xpr->beginP[depth] < xpr->endP[depth]) {
		// Intern node of the search tree

		// size_t p, epR, euR, eup;
		size_t np = xpr->endP[depth];

		// pivot
		size_t p = choose_pivot(xpr, N, depth);
		size_t epR = xpr->timeEndRNode[depth][p];

		// mettre les noeuds à enlever à la fin de P
		for (size_t i = 0; i < N[p]->n[depth]; i++) {
			size_t u = N[p]->node[i];
			if (is_in_P(xpr, u, depth)) {
				size_t eup = N[p]->end[i];
				size_t euR = xpr->timeEndRNode[depth][u];

				// TODO : == au lieu de <= ??
				if (euR <= min(epR, eup)) {
					np--;
					swap_P_node_position(xpr, u, np);
				}
			}
		}

		const size_t next_depth = depth + 1;
		size_tVector_clear(&xpr->candidates_to_iterate[depth]);

		// noeuds sur lesquels faire un appel récursif
		for (size_t i = xpr->beginP[depth]; i < np; i++) {
			size_tVector_push_unchecked(&xpr->candidates_to_iterate[depth], xpr->XPnode[i]);
		}

		// parcours des noeuds sur lesquels faire un appel récursif
		for (size_t i = 0; i < xpr->candidates_to_iterate[depth].size; i++) {
			size_t u = xpr->candidates_to_iterate[depth].array[i];

			// Reduced clique duration
			size_t e_new = min(xpr->timeEndRNode[depth][u], e);

			// If time is not reduced then R is not max in time
			R_is_max &= e_new < e;

			// Calcul de P et X à depth+1
			xpr->beginX[next_depth] = xpr->beginP[depth];
			xpr->beginP[next_depth] = xpr->beginP[depth];
			xpr->endP[next_depth] = xpr->beginP[depth];

			for (size_t j = 0; j < N[u]->n[depth]; j++) {
				size_t v = N[u]->node[j];

				if (is_in_P(xpr, v, depth)) {
					add_to_P(xpr, v, next_depth);
					update_timeEndRNode(xpr, v, N[u]->end[j], next_depth);
				}
				else if (is_in_X(xpr, v, depth)) {
					keep_in_X(xpr, v, next_depth);
					update_timeEndRNode(xpr, v, N[u]->end[j], next_depth);
				}
				else {
					remove_ind_NeighborListEnd(N[u], j, depth);
				}
			}

			// Mettre dans X les voisin de u dans S
			for (size_t j = 0; j < S[u]->n[depth]; j++) {
				size_t v = S[u]->node[j];

				if (is_in_P(xpr, v, depth)) {
					swap_from_P_to_X(xpr, v, next_depth);
				}
				else {
					remove_ind_NeighborList(S[u], j, depth);
					j--; // car échange avec un élément à tester
				}
			}

			// Réordonner N et S à depth+1
			reorderN_XUP(xpr, N, next_depth);
			reorderS_P(xpr, S, next_depth);

			// Ajouter u à R
			size_tVector_push_unchecked(&xpr->R, u);

			// Appel Récursif
			BKtemporal(xpr, b, e_new, N, S, mc, next_depth, cliques);
			BK_not_called = false;

			// Enlever de X les éléments de S[u]
			for (size_t j = 0; j < S[u]->n[depth]; j++) {
				size_t v = S[u]->node[j];
				ASSERT(is_in_X(xpr, v, next_depth));
				swap_from_X_to_P(xpr, v, next_depth);
			}

			// Enlever u de R
			size_tVector_pop_last(&xpr->R);

			// Enlever u de P et le mettre dans X, à depth
			swap_from_P_to_X(xpr, u, depth);
		}

		// Réordonner X pour remettre à la bonne place le X du début
		for (size_t i = 0; i < xpr->candidates_to_iterate[depth].size; i++) {
			size_t u = xpr->candidates_to_iterate[depth].array[i];
			swap_from_X_to_P(xpr, u, depth);
		}
	}

	// Imprimer R si R est maximal
	if (R_is_max) {
		// If there is a link to X which does not reduce time,
		// then R is not maximal
		for (size_t i = xpr->beginX[depth]; i < xpr->beginP[depth]; i++) {
			size_t u = xpr->XPnode[i];
			if (xpr->timeEndRNode[depth][u] >= e) {

				if (xpr->beginP[depth] == xpr->endP[depth]) {
					// Leaf of search tree
					mc->nLeafBad++;
				}

				return;
			}
		}

		// ********** COUNT **********
		// Then R is maximal
		if (BK_not_called) {
			// And R max in node
			// mc->sumSizeMaxNodeCliques += xpr->R->n;
			if (xpr->R.size > mc->maxCliqueSize) {
				mc->maxCliqueSize = xpr->R.size;
			}
		}
		mc->nMaxCliques++;

		Clique c = {
			.time_start = b,
			.time_end = e,
			.nb_nodes = xpr->R.size,
			.nodes = malloc(xpr->R.size * sizeof(NodeId)),
		};
		for (size_t i = 0; i < xpr->R.size; i++) {
			c.nodes[i] = xpr->R.array[i];
		}
		CliqueVector_push(cliques, c);
	}

	if (xpr->beginP[depth] == xpr->endP[depth]) {
		// Leaf of search tree
		if (R_is_max) {
			mc->nLeafGood++;
		}
		else {
			mc->nLeafBad++;
		}
	}
}

void MaxCliquesFromEdges(const size_tVector NewEdges, NeighborList** S, MySet* Snodes, size_t b, NeighborListEnd** N,
						 XPR* xpr, MyCounter* mc, CliqueVector* cliques) {

	const size_t depth = 0;
	const size_t next_depth = depth + 1;

	ASSERT(NewEdges.size % 3 == 0);
	for (size_t i = 0; i < NewEdges.size; i += 3) {
		size_t u = NewEdges.array[i];
		size_t v = NewEdges.array[i + 1];
		size_t e = NewEdges.array[i + 2];

		ASSERT(xpr->XPnode[xpr->XPindex[u]] == u);
		ASSERT(xpr->XPnode[xpr->XPindex[v]] == v);

		// Calculer X,P,R et faire l'appel récursif
		xpr->beginX[next_depth] = 0;
		xpr->beginP[next_depth] = 0;
		xpr->endP[next_depth] = 0;

		for (size_t iu = 0; iu < N[u]->n[depth]; iu++) {
			size_t w = N[u]->node[iu];

			ASSERT(xpr->XPnode[xpr->XPindex[w]] == w);

			size_t iv = ind_in_NeighborListEnd(w, N[v], depth);
			if (iv != SIZE_MAX) { // w in N[u] INTER N[v]
				if (is_in_NeighborList(w, S[u], depth) || is_in_NeighborList(w, S[v], depth)) {
					// Ajouter wu à X
					add_to_X0(xpr, w, next_depth);
				}
				else {
					add_to_P(xpr, w, next_depth);
				}

				init_timeEndRNode(xpr, w, e, N[u]->end[iu], N[v]->end[iv], next_depth);
			}
		}

		// Ordonner les voisins des noeuds de P
		reorderN_XUP(xpr, N, next_depth);
		reorderS_P(xpr, S, next_depth);

		// Initialisation de R = {u,v}
		size_tVector_clear(&xpr->R);
		size_tVector_push_unchecked(&xpr->R, u);
		size_tVector_push_unchecked(&xpr->R, v);

		// Appel du calcul des cliques max
		BKtemporal(xpr, b, e, N, S, mc, next_depth, cliques);

		// Ajout de l'arête {u,v} comme arête déjà traitée
		add_NeighborList(S[u], v);
		add_NeighborList(S[v], u);

		addToMySet(Snodes, u);
		addToMySet(Snodes, v);
	}

	// Clear S et Snodes
	for (size_t j = 0; j < Snodes->n; j++) {
		size_t w = Snodes->list[j];
		clear_NeighborList(S[w]);
	}
	clearMySet(Snodes);
}

CliqueVector cliques_sequential(const LinkPresenceStream* ls_end, Datastructure* d, MyCounter* mc) {
	size_t i, b, e, u, v;
	size_t old_b = SIZE_MAX;
	NeighborListEnd** N = d->N;
	NeighborList** S = d->S;
	MySet* Snodes = d->Snodes;
	XPR* xpr = d->xpr;
	CliqueVector result = CliqueVector_with_capacity(100);

	for (i = 0; i < ls_end->m; i++) {

		b = ls_end->link[i].start;
		e = ls_end->link[i].end;
		u = ls_end->link[i].nodes[0];
		v = ls_end->link[i].nodes[1];

		if (b == SIZE_MAX) { // end link

			if (e >= old_b && d->NewEdges.size > 0) {
				MaxCliquesFromEdges(d->NewEdges, S, Snodes, old_b, N, xpr, mc, &result);
				size_tVector_clear(&d->NewEdges);
				old_b = e;
			}

			// Normalement l'arête existe forcément
			// TODO : asserts
			remove_node_NeighborListEnd(N[u], v);
			remove_node_NeighborListEnd(N[v], u);
		}
		else { // New link
			ASSERT(xpr->XPnode[xpr->XPindex[u]] == u);
			ASSERT(xpr->XPnode[xpr->XPindex[v]] == v);

			if (old_b == SIZE_MAX) {
				old_b = b;
			}
			if (b > old_b) {
				MaxCliquesFromEdges(d->NewEdges, S, Snodes, old_b, N, xpr, mc, &result);
				size_tVector_clear(&d->NewEdges);
				old_b = b;
			}

			// normalement (u,v) n'est pas une arête de N
			ASSERT(ind_in_NeighborListEnd(v, N[u], 0) == SIZE_MAX);
			ASSERT(ind_in_NeighborListEnd(u, N[v], 0) == SIZE_MAX);

			add_NeighborListEnd(N[u], v, e);
			add_NeighborListEnd(N[v], u, e);

			size_tVector_push_unchecked(&d->NewEdges, u);
			size_tVector_push_unchecked(&d->NewEdges, v);
			size_tVector_push_unchecked(&d->NewEdges, e);
		}
	}

	if (d->NewEdges.size > 0) {
		MaxCliquesFromEdges(d->NewEdges, S, Snodes, old_b, N, xpr, mc, &result);
	}

	return result;
}

CliqueVector Stream_maximal_cliques(Stream* st) {
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	LinkPresenceVector links = LinkPresenceVector_new();
	LinksIterator links_set = funcs.links_set(st->stream_data);
	FOR_EACH_LINK(link_id, links_set) {
		Link link = funcs.nth_link(st->stream_data, link_id);
		for (size_t i = 0; i < link.presence.nb_intervals; i++) {
			LinkPresence l = (LinkPresence){
				.start = link.presence.intervals[i].start,
				.end = link.presence.intervals[i].end,
				.nodes[0] = link.nodes[0],
				.nodes[1] = link.nodes[1],
			};
			LinkPresenceVector_push(&links, l);
		}
	}

	LinkPresenceVector_sort(&links);
	Datastructure* d = allocDatastrucure_from_links(links);
	MyCounter* mc = alloc_MyCounter();
	CliqueVector v = cliques_sequential(d->ls_end, d, mc);
	LinkPresenceVector_destroy(links);

	return v;
}