#include "cliques.h"
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
#include <stdio.h>
#include <stdlib.h>

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

DefineVector(MyLink);
DefineVectorDeriveRemove(MyLink, NO_FREE(MyLink));

void swap(int* tab, int i, int j) {
	int tmp = tab[i];
	tab[i] = tab[j];
	tab[j] = tmp;
}

typedef struct {
	// int n;      // number of nodes
	int* XPnode;  // nodes of X,P structure
	int* XPindex; // index of nodes in X,P structure
	int* beginX;
	int* beginP;
	int* endP;
	int** timeEndRNode; // timeEndRNode[depth][w] : end time of R U {w}
	intVector R;
	intVector* candidates_to_iterate;

	// FOR ITERATIVE CODE
	int* i;			// i[depth] = indice courant dans candidates_to_iterate[depth]
	int* e;			// e[depth] temps de fin de R à depth
	bool* R_is_max; // R_is_max[depth] = vaut true à la fin du parcours de
					// candidate_to_iterate[depth] si R est maximal
} XPR;

XPR* allocXPR(int n, int depthMax) {
	int i, depth;
	XPR* xpr = MALLOC(sizeof(XPR));

	// FOR ITERATIVE CODE
	xpr->i = (int*)calloc(depthMax, sizeof(int)); // TODO : safe calloc?
	xpr->e = MALLOC(depthMax * sizeof(int));
	xpr->R_is_max = MALLOC(depthMax * sizeof(bool)); // TODO : bit mask?

	// xpr->n = 0;
	xpr->XPnode = MALLOC(n * sizeof(int));
	xpr->XPindex = MALLOC(n * sizeof(int));

	for (i = 0; i < n; i++) {
		xpr->XPnode[i] = i;
		xpr->XPindex[i] = i;
	}
	xpr->beginX = MALLOC(depthMax * sizeof(int));
	xpr->beginP = MALLOC(depthMax * sizeof(int));
	xpr->endP = MALLOC(depthMax * sizeof(int));
	xpr->timeEndRNode = MALLOC(depthMax * sizeof(int*));

	xpr->R = intVector_with_capacity(depthMax);
	xpr->candidates_to_iterate = MALLOC(depthMax * sizeof(intVector));

	for (depth = 0; depth < depthMax; depth++) {
		xpr->timeEndRNode[depth] = MALLOC(n * sizeof(int));
		xpr->candidates_to_iterate[depth] = intVector_with_capacity(depthMax);
	}
	return xpr;
}

bool is_in_P(XPR* xpr, int w, int depth) {
	return xpr->XPindex[w] >= xpr->beginP[depth] && xpr->XPindex[w] < xpr->endP[depth];
}

bool is_in_X(XPR* xpr, int w, int depth) {
	return xpr->XPindex[w] >= xpr->beginX[depth] && xpr->XPindex[w] < xpr->beginP[depth];
}

bool is_in_PUX(XPR* xpr, int w, int depth) {
	return xpr->XPindex[w] >= xpr->beginX[depth] && xpr->XPindex[w] < xpr->endP[depth];
}

bool is_in_R(XPR* xpr, int w) {
	return intVector_contains(xpr->R, w);
}

void add_to_P(XPR* xpr, int w, int depth) {

	int iw = xpr->XPindex[w];
	int u = xpr->XPnode[xpr->endP[depth]];

	swap(xpr->XPnode, iw, xpr->endP[depth]);
	swap(xpr->XPindex, w, u);
	xpr->endP[depth]++;
}

void swap_from_P_to_X(XPR* xpr, int w, int depth) {
	ASSERT(is_in_P(xpr, w, depth));

	int iw = xpr->XPindex[w];
	int iu = xpr->beginP[depth];
	int u = xpr->XPnode[iu];
	swap(xpr->XPnode, iw, iu);
	swap(xpr->XPindex, w, u);
	xpr->beginP[depth]++;
}

void swap_from_X_to_P(XPR* xpr, int w, int depth) {
	ASSERT(is_in_X(xpr, w, depth));

	xpr->beginP[depth]--;
	int iw = xpr->XPindex[w];
	int iu = xpr->beginP[depth];
	int u = xpr->XPnode[iu];
	swap(xpr->XPnode, iw, iu);
	swap(xpr->XPindex, w, u);
}

void add_to_X0(XPR* xpr, int w, int depth) {
	add_to_P(xpr, w, depth);
	swap_from_P_to_X(xpr, w, depth);
}

void keep_in_X(XPR* xpr, int w, int depth) {
	int iw = xpr->XPindex[w];
	int u = xpr->XPnode[--xpr->beginX[depth]];
	swap(xpr->XPnode, iw, xpr->beginX[depth]);
	swap(xpr->XPindex, w, u);
}

void swap_P_node_position(XPR* xpr, int u, int iv) {
	int iu = xpr->XPindex[u];
	int v = xpr->XPnode[iv];
	swap(xpr->XPnode, iu, iv);
	swap(xpr->XPindex, u, v);
}

int min(int a, int b) {
	return a < b ? a : b;
}

void init_timeEndRNode(XPR* xpr, int w, int e1, int e2, int e3, int depth) {
	xpr->timeEndRNode[depth][w] = min(min(e1, e2), e3);
}

void update_timeEndRNode(XPR* xpr, int v, int e, int depth) {
	int e1 = xpr->timeEndRNode[depth - 1][v];
	xpr->timeEndRNode[depth][v] = min(e, e1);
}

void print_P(XPR* xpr, int depth) {
	printf("P = ");
	for (int i = xpr->beginP[depth]; i < xpr->endP[depth]; i++) {
		printf("%d ", xpr->XPnode[i]);
	}
	printf("\n");
}

void print_X(XPR* xpr, int depth) {
	printf("X = ");
	for (int i = xpr->beginX[depth]; i < xpr->beginP[depth]; i++) {
		printf("%d ", xpr->XPnode[i]);
	}
	printf("\n");
}

void print_R(XPR* xpr) {
	// cerr << "R = ";
	printf("R = ");
	String str = intVector_to_string(&xpr->R);
	printf("%s\n", str.data);
	String_destroy(str);
}

typedef struct {
	int* n;	   // number of neighbors of subgraph at depth
	int* node; // list of neighbors
			   // int *beginX; // in [depth] from beginX to beginP-1 : neighbors INTER X
			   // int *beginP; // in [depth] from beginP to endP-1 : neighbors INTER P
			   // int *endP;
} NeighborList;

NeighborList* alloc_NeighborList(int degreeMax, int depthMax) {
	NeighborList* Nl = MALLOC(sizeof(NeighborList));
	Nl->n = MALLOC(depthMax * sizeof(int));
	Nl->n[0] = 0;
	Nl->node = MALLOC(degreeMax * sizeof(int));
	return Nl;
}

void add_NeighborList(NeighborList* Nl, int u) {
	// Les tailles sont allouées à l'avance : ne peut pas dépasser
	// ajout d'un noeud seulement pour depth == 0
	Nl->node[Nl->n[0]] = u;
	Nl->n[0]++;
}

void remove_ind_NeighborList(NeighborList* Nl, int i, int depth) {
	// ASSERT(i < Nl->n[depth]);
	swap(Nl->node, i, --Nl->n[depth]);
}

void clear_NeighborList(NeighborList* Nl) {
	// clear seulement pour depth == 0
	Nl->n[0] = 0;
}

bool is_in_NeighborList(int u, NeighborList* Nl, int depth) {
	// check if there is u between indexes i and j
	for (int i = 0; i < Nl->n[depth]; i++) {
		if (Nl->node[i] == u) {
			return true;
		}
	}
	return false;
}

void keep_in_NeighborList(int i, NeighborList* Nl, int depth) {
	ASSERT(i >= Nl->n[depth]);
	swap(Nl->node, i, Nl->n[depth]);
	Nl->n[depth]++;
}

void reorderS_P(XPR* xpr, NeighborList** S, int depth) {
	int i, j, u, v;

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

void print_NeighborList_nodes(NeighborList* Nl, int depth) {
	for (int i = 0; i < Nl->n[depth]; i++) {
		printf("%d", Nl->node[i]);
	}
	printf("\n");
}

typedef struct {
	int* n;	   // number of neighbors of subgraph at depth
	int* node; // list of neighbors
	int* end;  // list of end time of neighbors
} NeighborListEnd;

NeighborListEnd* alloc_NeighborListEnd(int degreeMax, int depthMax) {
	NeighborListEnd* Nle = MALLOC(sizeof(NeighborListEnd));
	Nle->n = MALLOC(degreeMax * sizeof(int));
	Nle->n[0] = 0;
	Nle->node = MALLOC(degreeMax * sizeof(int));
	Nle->end = MALLOC(degreeMax * sizeof(int));
	return Nle;
}

void add_NeighborListEnd(NeighborListEnd* Nle, int u, int e) {
	// ajout seulement pour depth == 0
	Nle->node[Nle->n[0]] = u;
	Nle->end[Nle->n[0]] = e;
	Nle->n[0]++;
}

void remove_node_NeighborListEnd(NeighborListEnd* Nle, int u) {
	// suppression d'un noeud seulement pour depth == 0
	for (int i = 0; i < Nle->n[0]; i++) {
		if (Nle->node[i] == u) {
			Nle->n[0]--;
			Nle->node[i] = Nle->node[Nle->n[0]];
			Nle->end[i] = Nle->end[Nle->n[0]];
			return;
		}
	}
}

void remove_ind_NeighborListEnd(NeighborListEnd* Nle, int i, int depth) {
	ASSERT(i < Nle->n[depth]);
	swap(Nle->node, i, --Nle->n[depth]);
	swap(Nle->end, i, Nle->n[depth]);
}

int ind_in_NeighborListEnd(int u, NeighborListEnd* Nle, int depth) {
	for (int i = 0; i < Nle->n[depth]; i++) {
		if (Nle->node[i] == u) {
			return i;
		}
	}
	return -1;
}

void keep_in_NeighborListEnd(int i, NeighborListEnd* Nle, int depth) {
	ASSERT(i >= Nle->n[depth]);
	swap(Nle->node, i, Nle->n[depth]);
	swap(Nle->end, i, Nle->n[depth]);
	Nle->n[depth]++;
}

void reorderN_XUP(XPR* xpr, NeighborListEnd** N, int depth) {
	for (int i = xpr->beginX[depth]; i < xpr->endP[depth]; i++) {

		int u = xpr->XPnode[i];
		ASSERT(xpr->XPnode[xpr->XPindex[u]] == u);

		N[u]->n[depth] = 0;

		for (int j = 0; j < N[u]->n[depth - 1]; j++) {
			int v = N[u]->node[j];
			if (is_in_PUX(xpr, v, depth)) {
				keep_in_NeighborListEnd(j, N[u], depth);
			}
		}
	}
}

void print_NeighborListEnd_nodes(NeighborListEnd* Nle, int depth) {
	// affiche les voisins entre les indices i et j
	for (int i = 0; i < Nle->n[depth]; i++) {
		printf("%d ", Nle->node[i]);
	}
	printf("\n");
}

typedef struct {
	int n; // number of elements in set
	// int nlmax; // maximum number of element in set
	int* list; // elements in set

	// int ntmax; // size of tab
	bool* tab; // tab[i]==1 iff i is in list
} MySet;

MySet* allocMySet(int init_size) {
	MySet* s = MALLOC(sizeof(MySet));
	s->n = 0;
	s->list = MALLOC(init_size * sizeof(int));
	s->tab = (bool*)calloc(init_size, sizeof(bool)); // TODO: safe calloc?
	return s;
}

bool isInMySet(MySet* s, int x) {
	return s->tab[x];
}

void addToMySet(MySet* s, int x) {
	if (!s->tab[x]) {
		s->list[s->n] = x;
		s->n += 1;
		s->tab[x] = true;
	}
}

void clearMySet(MySet* s) {
	for (int i = 0; i < s->n; i++) {
		s->tab[s->list[i]] = false;
	}
	s->n = 0;
}

typedef struct {
	int m;
	MyLink* link;
	intVector timesteps;
} MyLinkStream;

int endlink_sorter(const MyLink* l1, const MyLink* l2) {
	return l1->e - l2->e;
}

MyLinkStream* allocLinkStream_end_from_links(MyLinkVector links, int m, int ntimestep) {
	int i, b, old_b, im, e, u, v;
	int ia, id, ba, ea, ua, va, ed, ud, vd;

	MyLinkStream* ls_end = MALLOC(sizeof(MyLinkStream));

	// Init link stream data structures
	ls_end->m = m;
	ls_end->link = MALLOC(2 * m * sizeof(MyLink)); // double to add end

	MyLink* add_link = MALLOC(m * sizeof(MyLink));
	MyLink* del_link = MALLOC(m * sizeof(MyLink));

	ls_end->timesteps = intVector_with_capacity(ntimestep);
	im = 0;

	old_b = -1;
	for (size_t l = 0; l < links.size; l++) {
		if (b > old_b) {
			intVector_push_unchecked(&ls_end->timesteps, links.array[l].b);
		}
		old_b = b;

		add_link[im] = links.array[l];

		del_link[im] = links.array[l];
		del_link[im].b = -1;

		im++;
	}

	ASSERT(im == m);

	qsort(del_link, m, sizeof(MyLink), (int (*)(const void*, const void*)) & endlink_sorter);

	ia = 0;
	id = 0;
	i = 0;
	while (ia < m) {
		if (add_link[ia].b <= del_link[id].e) {
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
	unsigned long int n;
	unsigned long int m;
	unsigned int ntimestep;
	unsigned int degmax;
	MyLinkStream* ls_end;
	NeighborListEnd** N; // Neighbors of each node
	NeighborList** S;	 // Seen edges which must not belong to max cliques anymore
	MySet* Snodes;		 // Nodes of seen edges
	intVector NewEdges;	 // Edges at a given time : have to start max cliques at this time
	XPR* xpr;			 // X P R bron kerbosh datastructure
} Datastructure;

Datastructure* allocDatastrucure_from_links(MyLinkVector links) {
	NeighborListEnd** N;
	NeighborList** S;
	MySet* Snodes;
	intVector NewEdges;
	XPR* xpr;

	int b, u, v;
	int n = 0, m = 0, ntimestep = 0;
	int nmin = -1;
	int nreal = 0;
	int mt = 0, mtmax = 0; // nombre max d'arêtes par unité de temps
	int old_b;
	int *degreeT, *degreeMax;
	int dmax, dsum;
	bool first = true;

	for (int i = 0; i < links.size; i++) {
		MyLink* link = &links.array[i];
		m++;
		b = link->b;

		if (first) {
			old_b = b;
			ntimestep++;
			first = false;
		}

		ASSERT(b >= old_b);

		if (b > old_b) {
			ntimestep++;
			if (mt > mtmax) {
				mtmax = mt;
			}
			mt = 1;
		}
		else {
			mt++;
		}

		old_b = b;

		u = link->u;
		v = link->v;
		ASSERT(u < v);

		// node max / min
		if (nmin == -1) {
			nmin = u;
		}
		if (u < nmin) {
			nmin = u;
		}
		if (v < nmin) {
			nmin = v;
		}
		if (u > n) {
			n = u;
		}
		if (v > n) {
			n = v;
		}
	}

	ASSERT(b == old_b);
	mt++;

	if (mt > mtmax) {
		mtmax = mt;
	}

	// Init data
	degreeT = calloc(n + 1, sizeof(int));
	degreeMax = calloc(n + 1, sizeof(int));
	N = MALLOC((n + 1) * sizeof(NeighborListEnd*));
	S = MALLOC((n + 1) * sizeof(NeighborList*));
	Snodes = allocMySet(n + 1);
	NewEdges = intVector_with_capacity(3 * mtmax);

	MyLinkStream* ls_end = allocLinkStream_end_from_links(links, m, ntimestep);

	ASSERT(ls_end->timesteps.size == ntimestep);

	// Fill degree
	for (int i = 0; i < links.size; i++) {
		b = links.array[i].b;
		u = links.array[i].u;
		v = links.array[i].v;
		if (b == -1) // end link
		{
			degreeT[u]--;
			degreeT[v]--;
		}
		else // new link
		{
			degreeT[u]++;
			degreeT[v]++;
			if (degreeT[u] > degreeMax[u]) {
				degreeMax[u] = degreeT[u];
			}
			if (degreeT[v] > degreeMax[v]) {
				degreeMax[v] = degreeT[v];
			}
		}
	}

	// Init N and S
	dmax = 0;
	dsum = 0;
	for (u = 0; u <= n; u++) {
		if (degreeMax[u] > 0) {
			N[u] = alloc_NeighborListEnd(degreeMax[u], degreeMax[u]);
			S[u] = alloc_NeighborList(degreeMax[u], degreeMax[u]);
			dsum += degreeMax[u];
			nreal += 1;
			if (degreeMax[u] > dmax) {
				dmax = degreeMax[u];
			}
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

int choose_pivot(XPR* xpr, NeighborListEnd** N, int depth) {
	int p, pmax, i, ip, u, epR, euR, eup, dp;
	int dmax = -1;

	// for (ip = xpr->beginP[depth]; ip < xpr->endP[depth]; ip++)
	for (ip = xpr->beginX[depth]; ip < xpr->endP[depth]; ip++) {
		p = xpr->XPnode[ip];
		dp = 0;
		epR = xpr->timeEndRNode[depth][p];

		// compter les noeuds à enlever

		for (i = 0; i < N[p]->n[depth]; i++) {
			u = N[p]->node[i];
			if (is_in_P(xpr, u, depth)) {
				eup = N[p]->end[i];
				euR = xpr->timeEndRNode[depth][u];

				if (euR <= min(epR, eup)) {
					dp++;
				}
			}
		}

		if (dp > dmax) {
			pmax = p;
			dmax = dp;
		}
	}

	return pmax;
}

typedef struct {
	unsigned int maxCliqueSize;
	unsigned long long int nMaxCliques;
	unsigned long long int nTimeMaxCliques;
	unsigned long long int nLeafGood;
	unsigned long long int nLeafBad;
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

void BKtemporal(XPR* xpr, int b, int e, NeighborListEnd** N, NeighborList** S,
				// int *cpt,
				MyCounter* mc, int depth, CliqueVector* cliques) {

	// Node of search tree
	mc->nTimeMaxCliques++;

	int i, j, k, u, v, e_new;
	bool R_is_max = true;
	bool BK_not_called = true;

	if (xpr->beginP[depth] < xpr->endP[depth]) {
		// Intern node of the search tree

		int p, epR, euR, eup;
		int np = xpr->endP[depth];

		// pivot
		p = choose_pivot(xpr, N, depth);
		epR = xpr->timeEndRNode[depth][p];

		// mettre les noeuds à enlever à la fin de P
		for (i = 0; i < N[p]->n[depth]; i++) {
			u = N[p]->node[i];
			if (is_in_P(xpr, u, depth)) {
				eup = N[p]->end[i];
				euR = xpr->timeEndRNode[depth][u];

				// TODO : == au lieu de <= ??
				if (euR <= min(epR, eup)) {
					np--;
					swap_P_node_position(xpr, u, np);
				}
			}
		}

		const int next_depth = depth + 1;
		intVector_clear(&xpr->candidates_to_iterate[depth]);

		// noeuds sur lesquels faire un appel récursif
		for (i = xpr->beginP[depth]; i < np; i++) {
			intVector_push_unchecked(&xpr->candidates_to_iterate[depth], xpr->XPnode[i]);
		}

		// parcours des noeuds sur lesquels faire un appel récursif
		for (i = 0; i < xpr->candidates_to_iterate[depth].size; i++) {
			u = xpr->candidates_to_iterate[depth].array[i];

			// Reduced clique duration
			e_new = min(xpr->timeEndRNode[depth][u], e);

			// If time is not reduced then R is not max in time
			R_is_max &= e_new < e;

			// Calcul de P et X à depth+1
			xpr->beginX[next_depth] = xpr->beginP[depth];
			xpr->beginP[next_depth] = xpr->beginP[depth];
			xpr->endP[next_depth] = xpr->beginP[depth];

			for (j = 0; j < N[u]->n[depth]; j++) {
				v = N[u]->node[j];

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
			for (j = 0; j < S[u]->n[depth]; j++) {
				v = S[u]->node[j];

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
			intVector_push_unchecked(&xpr->R, u);

			// Appel Récursif
			BKtemporal(xpr, b, e_new, N, S, mc, next_depth, cliques);
			BK_not_called = false;

			// Enlever de X les éléments de S[u]
			for (j = 0; j < S[u]->n[depth]; j++) {
				v = S[u]->node[j];
				ASSERT(is_in_X(xpr, v, next_depth));
				swap_from_X_to_P(xpr, v, next_depth);
			}

			// Enlever u de R
			intVector_pop_last(&xpr->R);

			// Enlever u de P et le mettre dans X, à depth
			swap_from_P_to_X(xpr, u, depth);
		}

		// Réordonner X pour remettre à la bonne place le X du début
		for (i = 0; i < xpr->candidates_to_iterate[depth].size; i++) {
			u = xpr->candidates_to_iterate[depth].array[i];
			swap_from_X_to_P(xpr, u, depth);
		}
	}

	// Imprimer R si R est maximal
	if (R_is_max) {
		// If there is a link to X which does not reduce time,
		// then R is not maximal
		for (i = xpr->beginX[depth]; i < xpr->beginP[depth]; i++) {
			u = xpr->XPnode[i];
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
		for (i = 0; i < xpr->R.size; i++) {
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

void MaxCliquesFromEdges(const intVector NewEdges, NeighborList** S, MySet* Snodes, int b, NeighborListEnd** N,
						 XPR* xpr, MyCounter* mc, CliqueVector* cliques) {

	int i, j, u, v, w, e, w1, iu, iv;
	const int depth = 0;
	const int next_depth = depth + 1;

	ASSERT(NewEdges.size % 3 == 0);
	for (i = 0; i < NewEdges.size; i += 3) {
		u = NewEdges.array[i];
		v = NewEdges.array[i + 1];
		e = NewEdges.array[i + 2];

		ASSERT(xpr->XPnode[xpr->XPindex[u]] == u);
		ASSERT(xpr->XPnode[xpr->XPindex[v]] == v);

		// Calculer X,P,R et faire l'appel récursif
		xpr->beginX[next_depth] = 0;
		xpr->beginP[next_depth] = 0;
		xpr->endP[next_depth] = 0;

		for (iu = 0; iu < N[u]->n[depth]; iu++) {
			w = N[u]->node[iu];

			ASSERT(xpr->XPnode[xpr->XPindex[w]] == w);

			iv = ind_in_NeighborListEnd(w, N[v], depth);
			if (iv != -1) // w in N[u] INTER N[v]
			{
				if (is_in_NeighborList(w, S[u], depth)) {
					// Ajouter wu à X
					add_to_X0(xpr, w, next_depth);
				}
				else if (is_in_NeighborList(w, S[v], depth)) {
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
		intVector_clear(&xpr->R);
		intVector_push_unchecked(&xpr->R, u);
		intVector_push_unchecked(&xpr->R, v);

		// Appel du calcul des cliques max
		BKtemporal(xpr, b, e, N, S, mc, next_depth, cliques);

		// Ajout de l'arête {u,v} comme arête déjà traitée
		add_NeighborList(S[u], v);
		add_NeighborList(S[v], u);

		addToMySet(Snodes, u);
		addToMySet(Snodes, v);
	}

	// Clear S et Snodes
	for (j = 0; j < Snodes->n; j++) {
		w = Snodes->list[j];
		clear_NeighborList(S[w]);
	}
	clearMySet(Snodes);
}

CliqueVector cliques_sequential(const MyLinkStream* ls_end, Datastructure* d, MyCounter* mc) {
	int i, b, e, u, v;
	int old_b = -1;
	NeighborListEnd** N = d->N;
	NeighborList** S = d->S;
	MySet* Snodes = d->Snodes;
	XPR* xpr = d->xpr;
	CliqueVector result = CliqueVector_with_capacity(100);

	for (i = 0; i < ls_end->m; i++) {

		b = ls_end->link[i].b;
		e = ls_end->link[i].e;
		u = ls_end->link[i].u;
		v = ls_end->link[i].v;

		if (b == -1) { // end link

			if (e >= old_b && d->NewEdges.size > 0) {
				MaxCliquesFromEdges(d->NewEdges, S, Snodes, old_b, N, xpr, mc, &result);
				intVector_clear(&d->NewEdges);
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

			if (old_b == -1) {
				old_b = b;
			}
			if (b > old_b) {
				MaxCliquesFromEdges(d->NewEdges, S, Snodes, old_b, N, xpr, mc, &result);
				intVector_clear(&d->NewEdges);
				old_b = b;
			}

			// normalement (u,v) n'est pas une arête de N
			ASSERT(ind_in_NeighborListEnd(v, N[u], 0) == -1);
			ASSERT(ind_in_NeighborListEnd(u, N[v], 0) == -1);

			add_NeighborListEnd(N[u], v, e);
			add_NeighborListEnd(N[v], u, e);

			intVector_push_unchecked(&d->NewEdges, u);
			intVector_push_unchecked(&d->NewEdges, v);
			intVector_push_unchecked(&d->NewEdges, e);
		}
	}

	if (d->NewEdges.size > 0) {
		MaxCliquesFromEdges(d->NewEdges, S, Snodes, old_b, N, xpr, mc, &result);
	}

	return result;
}

bool MyLink_equals(MyLink l1, MyLink l2) {
	return l1.b == l2.b && l1.e == l2.e && l1.u == l2.u && l1.v == l2.v;
}

char* MyLink_to_string(MyLink* l) {
	char* str = (char*)malloc(100 * sizeof(char));
	sprintf(str, "b = %zu, e = %zu, u = %zu, v = %zu", l->b, l->e, l->u, l->v);
	return str;
}

int MyLink_compare(const MyLink* l1, const MyLink* l2) {
	return l1->b - l2->b;
}

DefineVectorDeriveOrdered(MyLink);

CliqueVector Stream_maximal_cliques(Stream* st) {
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	MyLinkVector links = MyLinkVector_new();
	LinksIterator links_set = funcs.links_set(st->stream_data);
	FOR_EACH_LINK(link_id, links_set) {
		Link link = funcs.nth_link(st->stream_data, link_id);
		for (size_t i = 0; i < link.presence.nb_intervals; i++) {
			MyLink l = (MyLink){.b = link.presence.intervals[i].start,
								.e = link.presence.intervals[i].end,
								.u = link.nodes[0],
								.v = link.nodes[1]};
			MyLinkVector_push(&links, l);
		}
	}

	MyLinkVector_sort(&links);
	Datastructure* d = allocDatastrucure_from_links(links);
	MyCounter* mc = alloc_MyCounter();
	CliqueVector v = cliques_sequential(d->ls_end, d, mc);
	MyLinkVector_destroy(links);

	return v;
}