#include "cliques.h"
#include "hashset.h"
#include "interval.h"
#include "iterators.h"
#include "stream.h"
#include "stream/chunk_stream.h"
#include "stream/chunk_stream_small.h"
#include "stream/full_stream_graph.h"
#include "stream/link_stream.h"
#include "stream_functions.h"
#include "stream_graph.h"
#include "units.h"
#include "utils.h"
#include "vector.h"
#include <stddef.h>
#include <stdio.h>

DefVector(char, NO_FREE(char));

// TODO : merge this in one file
#define APPEND_CONST(str) str, sizeof(str) - 1

char* Clique_to_string(Clique* c) {
	charVector str = charVector_new();
	charVector_append(&str, APPEND_CONST("Clique ["));
	char time_str[20];
	sprintf(time_str, "%zu -> %zu] ", c->time_start, c->time_end);
	charVector_append(&str, APPEND_CONST(time_str));
	charVector_append(&str, APPEND_CONST("Nodes : ["));
	for (size_t i = 0; i < c->nb_nodes; i++) {
		char node_str[20];
		sprintf(node_str, "%zu", c->nodes[i]);
		charVector_append(&str, APPEND_CONST(node_str));
		if (i < c->nb_nodes - 1) {
			charVector_append(&str, APPEND_CONST(", "));
		}
	}
	charVector_push(&str, ']');
	charVector_push(&str, '\0');
	return str.array;
}

bool Clique_equals(Clique c1, Clique c2) {
	if (c1.time_start != c2.time_start || c1.time_end != c2.time_end || c1.nb_nodes != c2.nb_nodes) {
		return false;
	}
	for (size_t i = 0; i < c1.nb_nodes; i++) {
		if (c1.nodes[i] != c2.nodes[i]) {
			return false;
		}
	}
	return true;
}

bool is_space(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

void link_from_line(int* link, char* str) {
	char* tail;
	int next;
	// assert(*i == 0);
	int i = 0;

	while (1) {
		// Skip whitespace by hand, to detect the end.
		while (is_space(*str)) {
			str++;
		}

		if (*str == 0) {
			return;
		}

		// There is more nonwhitespace,
		// so it ought to be another number.
		// Parse it.
		next = (int)strtol(str, &tail, 0);
		link[i++] = next;
		// Advance past it.
		str = tail;
	}

	assert(i == 4);
}

/*__attribute__((always_inline))*/
void swap(int* tab, int i, int j) {
	// cout << "-- swap --" << endl;
	// cout << "i = " << i << " ; tab[i] = " << tab[i] << endl;
	// cout << "j = " << j << " ; tab[j] = " << tab[j] << endl;

	int tmp = tab[i];
	tab[i] = tab[j];
	tab[j] = tmp;

	// cout << "-- END swap --" << endl;
}

typedef struct {
	int n; // size of the vector
	// int nmax; // size of allocated memory
	int* tab; // list of the elements
} MyList;

MyList* allocMyList(int size) {
	MyList* mv = (MyList*)malloc(sizeof(MyList));
	if (mv == NULL) {
		exit(EXIT_FAILURE);
	}

	mv->n = 0;
	// mv->nmax = size;
	mv->tab = (int*)malloc(size * sizeof(int));
	if (mv->tab == NULL) {
		exit(EXIT_FAILURE);
	}

	return mv;
}

/*__attribute__((always_inline))*/
void pushback(MyList* mv, int x) {
	// Les tailles sont allouées à l'avance
	// if (mv->n == mv->nmax)
	// {
	//     mv->nmax *= 2;
	//     mv->tab = (int *)realloc(mv->tab, mv->nmax * sizeof(int));
	// }

	// cout << "mv->n = " << mv->n << " ; x = " << x << endl;
	mv->tab[mv->n++] = x;
}

/*__attribute__((always_inline))*/
void popback(MyList* mv) {
	mv->n--;
}

/*__attribute__((always_inline))*/
void removeFromMyList(MyList* mv, int x) {
	int i;
	for (i = 0; i < mv->n; i++) {
		if (mv->tab[i] == x) {
			mv->tab[i] = mv->tab[--mv->n];
			return;
		}
	}
}

/*__attribute__((always_inline))*/
void clearMyList(MyList* mv) {
	mv->n = 0;
}

void print_MyList(MyList* mv) {
	int i;
	for (i = 0; i < mv->n; i++) {
		// cerr << mv->tab[i] << " ";
		printf("%d ", mv->tab[i]);
	}
	// cerr << endl;
	printf("\n");
}

typedef struct {
	// int n;      // number of nodes
	int* XPnode;  // nodes of X,P structure
	int* XPindex; // index of nodes in X,P structure
	int* beginX;
	int* beginP;
	int* endP;
	int** timeEndRNode; // timeEndRNode[depth][w] : end time of R U {w}
	MyList* R;
	MyList** candidates_to_iterate;

	// FOR ITERATIVE CODE
	int* i;			// i[depth] = indice courant dans candidates_to_iterate[depth]
	int* e;			// e[depth] temps de fin de R à depth
	bool* R_is_max; // R_is_max[depth] = vaut true à la fin du parcours de
					// candidate_to_iterate[depth] si R est maximal
} XPR;

XPR* allocXPR(int n, int depthMax) {
	int i, depth;
	XPR* xpr = (XPR*)malloc(sizeof(XPR));
	if (xpr == NULL) {
		exit(EXIT_FAILURE);
	}

	// FOR ITERATIVE CODE
	xpr->i = (int*)calloc(depthMax, sizeof(int));
	xpr->e = (int*)malloc(depthMax * sizeof(int));
	xpr->R_is_max = (bool*)malloc(depthMax * sizeof(bool));

	// xpr->n = 0;
	xpr->XPnode = (int*)malloc(n * sizeof(int));
	if (xpr->XPnode == NULL) {
		exit(EXIT_FAILURE);
	}

	xpr->XPindex = (int*)malloc(n * sizeof(int));
	if (xpr->XPindex == NULL) {
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < n; i++) {
		xpr->XPnode[i] = i;
		xpr->XPindex[i] = i;
	}
	xpr->beginX = (int*)malloc(depthMax * sizeof(int));
	if (xpr->beginX == NULL) {
		exit(EXIT_FAILURE);
	}

	xpr->beginP = (int*)malloc(depthMax * sizeof(int));
	if (xpr->beginP == NULL) {
		exit(EXIT_FAILURE);
	}

	xpr->endP = (int*)malloc(depthMax * sizeof(int));
	if (xpr->endP == NULL) {
		exit(EXIT_FAILURE);
	}

	xpr->timeEndRNode = (int**)malloc(depthMax * sizeof(int*));
	if (xpr->timeEndRNode == NULL) {
		exit(EXIT_FAILURE);
	}

	xpr->R = allocMyList(depthMax);
	xpr->candidates_to_iterate = (MyList**)malloc(depthMax * sizeof(MyList));
	if (xpr->candidates_to_iterate == NULL) {
		exit(EXIT_FAILURE);
	}

	for (depth = 0; depth < depthMax; depth++) {
		xpr->timeEndRNode[depth] = (int*)malloc(n * sizeof(int));
		if (xpr->timeEndRNode[depth] == NULL) {
			exit(EXIT_FAILURE);
		}

		xpr->candidates_to_iterate[depth] = allocMyList(depthMax);
	}
	return xpr;
}

/*__attribute__((always_inline))*/
bool is_in_P(XPR* xpr, int w, int depth) {
	return xpr->XPindex[w] >= xpr->beginP[depth] && xpr->XPindex[w] < xpr->endP[depth];
}

/*__attribute__((always_inline))*/
bool is_in_X(XPR* xpr, int w, int depth) {
	// cout << "---" << endl;
	// cout << "w = " << w << endl;
	// cout << "xpr->XPindex[w] = " << xpr->XPindex[w]<< endl;;
	return xpr->XPindex[w] >= xpr->beginX[depth] && xpr->XPindex[w] < xpr->beginP[depth];
}

/*__attribute__((always_inline))*/
bool is_in_PUX(XPR* xpr, int w, int depth) {
	return xpr->XPindex[w] >= xpr->beginX[depth] && xpr->XPindex[w] < xpr->endP[depth];
}

/*__attribute__((always_inline))*/
bool is_in_R(XPR* xpr, int w) {
	for (int i = 0; i < xpr->R->n; i++) {
		if (xpr->R->tab[i] == w) {
			return true;
		}
	}
	return false;
}

/*__attribute__((always_inline))*/
void add_to_P(XPR* xpr, int w, int depth) {
	// cout << "----" << endl;

	int iw = xpr->XPindex[w];
	int u = xpr->XPnode[xpr->endP[depth]];

	// cout << "w = " << w << endl;
	// cout << "u = " << u << endl;
	// cout << "iw = " << iw << endl;
	// cout << "xpr->endP[depth] = " << xpr->endP[depth] << endl;

	swap(xpr->XPnode, iw, xpr->endP[depth]);
	swap(xpr->XPindex, w, u);
	xpr->endP[depth]++;

	// cout << "u = " << u << endl;

	// cout << xpr->XPindex[w] << endl;
	// cout << xpr->XPnode[xpr->endP[depth]-1] << endl;
	// cout << xpr->XPnode[iw] << endl;

	// cout << "--" << endl;
}

/*__attribute__((always_inline))*/
void swap_from_P_to_X(XPR* xpr, int w, int depth) {
	// assert(is_in_P(xpr, w, depth));
	int iw = xpr->XPindex[w];
	int iu = xpr->beginP[depth];
	int u = xpr->XPnode[iu];
	swap(xpr->XPnode, iw, iu);
	swap(xpr->XPindex, w, u);
	xpr->beginP[depth]++;
}

/*__attribute__((always_inline))*/
void swap_from_X_to_P(XPR* xpr, int w, int depth) {
	// assert(is_in_X(xpr, w, depth));
	xpr->beginP[depth]--;
	int iw = xpr->XPindex[w];
	int iu = xpr->beginP[depth];
	int u = xpr->XPnode[iu];
	swap(xpr->XPnode, iw, iu);
	swap(xpr->XPindex, w, u);
}

/*__attribute__((always_inline))*/
void add_to_X0(XPR* xpr, int w, int depth) {
	add_to_P(xpr, w, depth);
	swap_from_P_to_X(xpr, w, depth);
}

/*__attribute__((always_inline))*/
void keep_in_X(XPR* xpr, int w, int depth) {
	int iw = xpr->XPindex[w];
	int u = xpr->XPnode[--xpr->beginX[depth]];
	swap(xpr->XPnode, iw, xpr->beginX[depth]);
	swap(xpr->XPindex, w, u);
}

/*__attribute__((always_inline))*/
void swap_P_node_position(XPR* xpr, int u, int iv) {
	int iu = xpr->XPindex[u];
	int v = xpr->XPnode[iv];
	swap(xpr->XPnode, iu, iv);
	swap(xpr->XPindex, u, v);
}

/*__attribute__((always_inline))*/
void init_timeEndRNode(XPR* xpr, int w, int e1, int e2, int e3, int depth) {
	// on garde min(e1,e2,e3)
	if (e1 < e2) {
		if (e1 < e3) {
			xpr->timeEndRNode[depth][w] = e1;
		}
		else {
			xpr->timeEndRNode[depth][w] = e3;
		}
	}
	else {
		if (e2 < e3) {
			xpr->timeEndRNode[depth][w] = e2;
		}
		else {
			xpr->timeEndRNode[depth][w] = e3;
		}
	}
}

int min(int a, int b) {
	return a < b ? a : b;
}

/*__attribute__((always_inline))*/
void update_timeEndRNode(XPR* xpr, int v, int e, int depth) {
	int e1 = xpr->timeEndRNode[depth - 1][v];
	xpr->timeEndRNode[depth][v] = min(e, e1);
}

void print_P(XPR* xpr, int depth) {
	int i;
	// cerr << "P = ";
	printf("P = ");
	for (i = xpr->beginP[depth]; i < xpr->endP[depth]; i++) {
		// cerr << xpr->XPnode[i] << " ";
		printf("%d ", xpr->XPnode[i]);
	}
	// cerr << endl;
	printf("\n");
}

void print_X(XPR* xpr, int depth) {
	int i;
	// cerr << "X = ";
	printf("X = ");
	for (i = xpr->beginX[depth]; i < xpr->beginP[depth]; i++) {
		// cerr << xpr->XPnode[i] << " ";
		printf("%d ", xpr->XPnode[i]);
	}
	// cerr << endl;
	printf("\n");
}

void print_R(XPR* xpr) {
	// cerr << "R = ";
	printf("R = ");
	print_MyList(xpr->R);
}

typedef struct {
	int* n;	   // number of neighbors of subgraph at depth
	int* node; // list of neighbors
			   // int *beginX; // in [depth] from beginX to beginP-1 : neighbors INTER X
			   // int *beginP; // in [depth] from beginP to endP-1 : neighbors INTER P
			   // int *endP;
} NeighborList;

NeighborList* alloc_NeighborList(int degreeMax, int depthMax) {
	NeighborList* Nl = (NeighborList*)malloc(sizeof(NeighborList));
	if (Nl == NULL) {
		exit(EXIT_FAILURE);
	}

	Nl->n = (int*)malloc(depthMax * sizeof(int));
	if (Nl->n == NULL) {
		exit(EXIT_FAILURE);
	}

	Nl->n[0] = 0;
	Nl->node = (int*)malloc(degreeMax * sizeof(int));
	if (Nl->node == NULL) {
		exit(EXIT_FAILURE);
	}

	return Nl;
}

/*__attribute__((always_inline))*/
void add_NeighborList(NeighborList* Nl, int u) {
	// Les tailles sont allouées à l'avance : ne peut pas dépasser
	// ajout d'un noeud seulement pour depth == 0
	Nl->node[Nl->n[0]++] = u;
}

/*__attribute__((always_inline))*/
void remove_ind_NeighborList(NeighborList* Nl, int i, int depth) {
	// assert(i < Nl->n[depth]);
	swap(Nl->node, i, --Nl->n[depth]);
}

/*__attribute__((always_inline))*/
void clear_NeighborList(NeighborList* Nl) {
	// clear seulement pour depth == 0
	Nl->n[0] = 0;
}

/*__attribute__((always_inline))*/
bool is_in_NeighborList(int u, NeighborList* Nl, int depth) {
	// check if there is u between indexes i and j
	for (int i = 0; i < Nl->n[depth]; i++) {
		if (Nl->node[i] == u) {
			return true;
		}
	}
	return false;
}

/*__attribute__((always_inline))*/
void keep_in_NeighborList(int i, NeighborList* Nl, int depth) {
	// assert(i >= Nl->n[depth]);
	swap(Nl->node, i, Nl->n[depth]);
	Nl->n[depth]++;
}

// void swap_NeighborList_X(int i, NeighborList *Nl, int depth)
// {
//     swap_NeighborList_P(i, Nl, depth);
//     swap(Nl->node, Nl->endP[depth] - 1, Nl->beginP[depth]);
//     Nl->beginP[depth]++;
// }

/*__attribute__((always_inline))*/
void reorderS_P(XPR* xpr, NeighborList** S, int depth) {
	int i, j, u, v;

	for (i = xpr->beginP[depth]; i < xpr->endP[depth]; i++) {
		u = xpr->XPnode[i];
		// assert(xpr->XPnode[xpr->XPindex[u]] == u);

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
		// cerr << Nl->node[i] << " ";
		printf("%d ", Nl->node[i]);
	}
	// cerr << endl;
	printf("\n");
}

typedef struct {
	int* n;	   // number of neighbors of subgraph at depth
	int* node; // list of neighbors
	// int *beginX; // in [depth] from beginX to beginP-1 : neighbors INTER X
	// int *beginP; // in [depth] from beginP to endP-1 : neighbors INTER P
	// int *endP;
	int* end; // list of end time of neighbors
} NeighborListEnd;

NeighborListEnd* alloc_NeighborListEnd(int degreeMax, int depthMax) {
	int i;
	NeighborListEnd* Nle = (NeighborListEnd*)malloc(sizeof(NeighborListEnd));
	if (Nle == NULL) {
		exit(EXIT_FAILURE);
	}

	Nle->n = (int*)malloc(degreeMax * sizeof(int));
	if (Nle->n == NULL) {
		exit(EXIT_FAILURE);
	}

	Nle->n[0] = 0;
	Nle->node = (int*)malloc(degreeMax * sizeof(int));
	if (Nle->node == NULL) {
		exit(EXIT_FAILURE);
	}

	Nle->end = (int*)malloc(degreeMax * sizeof(int));
	if (Nle->end == NULL) {
		exit(EXIT_FAILURE);
	}

	return Nle;
}

/*__attribute__((always_inline))*/
void add_NeighborListEnd(NeighborListEnd* Nle, int u, int e) {
	// ajout seulement pour depth == 0

	// cerr << "-- 1" << endl;
	// cerr << "Nle->n[0] = " << Nle->n[0] << endl;

	Nle->node[Nle->n[0]] = u;

	// cerr << "-- 2" << endl;

	Nle->end[Nle->n[0]++] = e;
}

/*__attribute__((always_inline))*/
void remove_node_NeighborListEnd(NeighborListEnd* Nle, int u) {
	// suppression d'un noeud seulement pour depth == 0
	for (int i = 0; i < Nle->n[0]; i++) {
		if (Nle->node[i] == u) {
			Nle->node[i] = Nle->node[--Nle->n[0]];
			Nle->end[i] = Nle->end[Nle->n[0]];
			return;
		}
	}
}

/*__attribute__((always_inline))*/
void remove_ind_NeighborListEnd(NeighborListEnd* Nle, int i, int depth) {
	// assert(i < Nle->n[depth]);
	swap(Nle->node, i, --Nle->n[depth]);
	swap(Nle->end, i, Nle->n[depth]);
}

/*__attribute__((always_inline))*/
int ind_in_NeighborListEnd(int u, NeighborListEnd* Nle, int depth) {
	for (int i = 0; i < Nle->n[depth]; i++) {
		if (Nle->node[i] == u) {
			return i;
		}
	}
	return -1;
}

/*__attribute__((always_inline))*/
void keep_in_NeighborListEnd(int i, NeighborListEnd* Nle, int depth) {
	// cout << "------" << endl;
	// cout << "i = " << i << endl;
	// cout << "Nle->endP[depth] = " << Nle->endP[depth] << endl;
	// for (int j = 0; j < Nle->endP[depth]; j++)
	// {
	//     cout << "j = " << j << " ; Nle->node[j] = " << Nle->node[j] << endl;
	// }

	// assert(i >= Nle->n[depth]);
	swap(Nle->node, i, Nle->n[depth]);
	swap(Nle->end, i, Nle->n[depth]);
	Nle->n[depth]++;

	// cout << "***********************************" << endl;
}

// void swap_NeighborListEnd_X(int i, NeighborListEnd *Nle, int depth)
// {
//     swap_NeighborListEnd_P(i, Nle, depth);
//     swap(Nle->node, Nle->endP[depth] - 1, Nle->beginP[depth]);
//     swap(Nle->end, Nle->endP[depth] - 1, Nle->beginP[depth]);
//     Nle->beginP[depth]++;
// }

/*__attribute__((always_inline))*/
void reorderN_XUP(XPR* xpr, NeighborListEnd** N, int depth) {
	// cerr << "----- reorder_NeighborListEnd -----" << endl;
	// cout << "xpr->beginP[depth] = " << xpr->beginP[depth] << endl;
	// cout << "xpr->endP[depth] = " << xpr->endP[depth] << endl;

	// print_P(xpr, depth);

	int i, j, u, v;
	// for (i = xpr->beginP[depth]; i < xpr->endP[depth]; i++)
	for (i = xpr->beginX[depth]; i < xpr->endP[depth]; i++) {

		// cout << "i = " << i << endl;
		u = xpr->XPnode[i];
		// assert(xpr->XPnode[xpr->XPindex[u]] == u);

		// cout << "u = " << u << endl;
		// cout << "xpr->XPindex[u] = " << xpr->XPindex[u] << endl;

		// cerr << "cc 1" << endl;

		// cerr << "depth = " << depth << ", u = " << u << endl;

		N[u]->n[depth] = 0;

		//  cerr << "cc 2" << endl;

		// cout << "depth = " << depth << endl;
		// cout << "u = " << u << endl;
		// cout << "N[u]->n[depth-1] = " << N[u]->n[depth-1] << endl;

		for (j = 0; j < N[u]->n[depth - 1]; j++) {
			v = N[u]->node[j];

			// cout << "j = " << j << endl;
			// cout << "v = " << v << endl;

			if (is_in_PUX(xpr, v, depth)) {
				keep_in_NeighborListEnd(j, N[u], depth);
			}
		}
	}
	// cerr << "----- END reorder_NeighborListEnd -----" << endl;
}

void print_NeighborListEnd_nodes(NeighborListEnd* Nle, int depth) {
	// affiche les voisins entre les indices i et j
	for (int i = 0; i < Nle->n[depth]; i++) {
		// cerr << Nle->node[i] << " ";
		printf("%d ", Nle->node[i]);
	}
	// cerr << endl;
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
	MySet* s = (MySet*)malloc(sizeof(MySet));
	if (s == NULL) {
		exit(EXIT_FAILURE);
	}

	s->n = 0;
	// s->nlmax = init_size;
	s->list = (int*)malloc(init_size * sizeof(int));
	if (s->list == NULL) {
		exit(EXIT_FAILURE);
	}

	// s->ntmax = init_size;
	s->tab = (bool*)calloc(init_size, sizeof(bool));
	if (s->tab == NULL) {
		exit(EXIT_FAILURE);
	}

	return s;
}

/*__attribute__((always_inline))*/
bool isInMySet(MySet* s, int x) {
	//   if (x >= s->ntmax)
	//   {
	//     return 0;
	//   }
	return s->tab[x];
}

/*__attribute__((always_inline))*/
void addToMySet(MySet* s, int x) {
	//   if (x >= s->ntmax)
	//   {
	//     s->tab = realloc(s->tab, (x + S2MAX) * sizeof(Kvalue));
	//     bzero(s->tab + s->ntmax, (x + S2MAX - s->ntmax) * sizeof(Kvalue));
	//     s->ntmax = x + S2MAX;
	//   }
	if (s->tab[x] == false) {
		// s->nl++;
		// if (s->nl == s->nlmax)
		// {
		//   s->nlmax *= 2;
		//   s->list = realloc(s->list, s->nlmax * sizeof(Clique));
		// }
		s->list[s->n++] = x;
		s->tab[x] = true;
	}
}

/*__attribute__((always_inline))*/
void clearMySet(MySet* s) {
	int i;
	for (i = 0; i < s->n; i++) {
		s->tab[s->list[i]] = false;
	}
	s->n = 0;
}

typedef struct {
	int b;
	int e;
	int u;
	int v;
} MyLink;

typedef struct {
	int m;
	MyLink* link;
	MyList* timesteps;
} MyLinkStream;

int endlink_sorter(const MyLink* l1, const MyLink* l2) {
	return l1->e - l2->e;
}

MyLinkStream* allocLinkStream_end(char* lsFile, int m, int ntimestep) {
	int i, b, old_b, im, e, u, v;
	int ia, id, ba, ea, ua, va, ed, ud, vd;

	MyLinkStream* ls_end = MALLOC(sizeof(MyLinkStream));
	int* link = MALLOC(4 * sizeof(int));
	char* line = NULL;
	size_t len = 0;

	// Init link stream data structures
	ls_end->m = m;
	ls_end->link = MALLOC(2 * m * sizeof(MyLink)); // double to add end

	MyLink* add_link = MALLOC(m * sizeof(MyLink));
	MyLink* del_link = MALLOC(m * sizeof(MyLink));

	ls_end->timesteps = allocMyList(ntimestep);
	im = 0;

	// Fill link stream tmp structures
	FILE* fp = fopen(lsFile, "r");

	old_b = -1;
	while ((getline(&line, &len, fp)) != -1) {
		i = 0;
		link_from_line(link, line);
		b = link[0];
		e = link[1];
		u = link[2];
		v = link[3];
		if (b > old_b) {
			pushback(ls_end->timesteps, b);
		}
		old_b = b;

		add_link[im].b = b;
		add_link[im].e = e;
		add_link[im].u = u;
		add_link[im].v = v;

		del_link[im].b = -1;
		del_link[im].e = e;
		del_link[im].u = u;
		del_link[im].v = v;

		im++;
	}
	fclose(fp);

	assert(im == m);

	// Sort end link
	// sort(del_link, del_link + m, &endlink_sorter);
	qsort(del_link, m, sizeof(MyLink), (int (*)(const void*, const void*)) & endlink_sorter);
	// for (i = 0; i < 100; i++)
	// {
	//     cerr << "e = " << del_link[i].e << endl;
	// }

	ia = 0;
	id = 0;
	i = 0;
	while (ia < m) {
		ba = add_link[ia].b;
		ea = add_link[ia].e;
		ua = add_link[ia].u;
		va = add_link[ia].v;

		ed = del_link[id].e;
		ud = del_link[id].u;
		vd = del_link[id].v;

		if (ba <= ed) {
			ls_end->link[i++] = add_link[ia++];
		}
		else {
			ls_end->link[i++] = del_link[id++];
		}
	}
	ls_end->m = i;

	// for (i = 0; i < ls_end->m; i++)
	// {
	//     b = ls_end->link[i].b;
	//     e = ls_end->link[i].e;
	//     u = ls_end->link[i].u;
	//     v = ls_end->link[i].v;
	//     cerr << b << " " << e << " " << u << " " << v << endl;
	// }

	free(link);
	free(line);
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
	MyList* NewEdges;	 // Edges at a given time : have to start max cliques at this time
	XPR* xpr;			 // X P R bron kerbosh datastructure
} Datastructure;

Datastructure* allocDatastrucure(char* lsFile) {
	NeighborListEnd** N;
	NeighborList** S;
	MySet* Snodes;
	MyList* NewEdges;
	XPR* xpr;

	int* link = (int*)malloc(4 * sizeof(int));
	if (link == NULL) {
		exit(EXIT_FAILURE);
	}

	int b, e, u, v, i;
	int n = 0, m = 0, ntimestep = 0;
	int nmin = -1;
	int nreal = 0;
	int mt = 0, mtmax = 0; // nombre max d'arêtes par unité de temps
	int old_b;
	int *degreeT, *degreeMax;
	int dmax, dsum;
	bool first = true;

	// Read link stream file
	FILE* fp = fopen(lsFile, "r");
	if (fp == NULL) {
		exit(EXIT_FAILURE);
	}

	char* line = NULL;
	size_t len = 0;
	while ((getline(&line, &len, fp)) != -1) {
		int i = 0;
		link_from_line(link, line);
		m++;
		b = link[0];

		if (first) {
			old_b = b;
			ntimestep++;
			first = false;
		}

		assert(b >= old_b);

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

		u = link[2];
		v = link[3];
		assert(u < v);

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
	fclose(fp);

	assert(b == old_b);
	mt++;

	if (mt > mtmax) {
		mtmax = mt;
	}

	// Init data
	degreeT = (int*)calloc(n + 1, sizeof(int));
	degreeMax = (int*)calloc(n + 1, sizeof(int));
	N = (NeighborListEnd**)malloc((n + 1) * sizeof(NeighborListEnd*));
	S = (NeighborList**)malloc((n + 1) * sizeof(NeighborList*));
	Snodes = allocMySet(n + 1);
	NewEdges = allocMyList(3 * mtmax); // we need to store u,v,e for each starting edge

	MyLinkStream* ls_end = allocLinkStream_end(lsFile, m, ntimestep);

	assert(ls_end->m >= m);

	// cerr << ls_end->timesteps->n << endl;
	// cerr << ntimestep << endl;
	printf("%d\n", ls_end->timesteps->n);
	printf("%d\n", ntimestep);

	assert(ls_end->timesteps->n == ntimestep);

	// Fill degree
	for (i = 0; i < ls_end->m; i++) {
		b = ls_end->link[i].b;
		// e = ls_end->link[i].e;
		u = ls_end->link[i].u;
		v = ls_end->link[i].v;
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

	// Dans PUX, au plus dmax éléments
	// Dans R au plus dmax+1 éléments
	xpr = allocXPR(n + 1, dmax + 1);

	// cerr << "----------" << endl;
	// cerr << "n = " << n << endl;
	// cerr << "m = " << m << endl;
	// cerr << "ntimestep = " << ntimestep << endl;
	// cerr << "nreal = " << nreal << endl;
	// cerr << "nmin = " << nmin << endl;
	// cerr << "mtmax = " << mtmax << endl;
	// cerr << "mtmoy = " << (double)m / ntimestep << endl;
	// cerr << "dmax = " << dmax << endl;
	// cerr << "dmoy = " << (double)dsum / nreal << endl;
	// cerr << "----------" << endl;

	free(degreeT);
	free(degreeMax);
	free(link);
	free(line);

	Datastructure* d = (Datastructure*)malloc(sizeof(Datastructure));
	if (d == NULL) {
		exit(EXIT_FAILURE);
	}

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

		// print_P(xpr, depth);
		// print_X(xpr, depth);
		// cerr << "p = " << p << endl;
		// cerr << "depth = " << depth << endl;
		// cerr << "N[p]->n[depth] = " << N[p]->n[depth] << endl;

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
	// unsigned long long int sumSizeCliques;
	// unsigned long long int sumSizeMaxNodeCliques;
	// unsigned long long int sumDegreeEdge1;
	// unsigned long long int sumDegreeEdge2;
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
	// mc->sumDegreeEdge1 = 0;
	// mc->sumDegreeEdge2 = 0;
	// mc->sumSizeCliques = 0;
	// mc->sumSizeMaxNodeCliques = 0;
	return mc;
}

void BKtemporal(XPR* xpr, int b, int e, NeighborListEnd** N, NeighborList** S,
				// int *cpt,
				MyCounter* mc, int depth, bool PIVOT) {
	// if (debug)
	// {
	//     cerr << "----------" << endl;
	//     print_R(xpr);
	//     cerr << "b, e = " << b << ", " << e << endl;
	//     print_P(xpr, depth);
	//     print_X(xpr, depth);
	//     cerr << "xpr->beginX[depth] = " << xpr->beginX[depth] << endl;
	//     cerr << "xpr->beginP[depth] = " << xpr->beginP[depth] << endl;
	//     cerr << "xpr->enP[depth] = " << xpr->endP[depth] << endl;
	// }

	// Node of search tree
	mc->nTimeMaxCliques++;

	int i, j, k, u, v, e_new;
	bool R_is_max = true;
	bool BK_not_called = true;

	if (xpr->beginP[depth] < xpr->endP[depth]) {
		// Intern node of the search tree

		int p, epR, euR, eup;
		int np = xpr->endP[depth];

		if (PIVOT) {
			// pivot
			// p = xpr->XPnode[xpr->beginP[depth]];
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
						swap_P_node_position(xpr, u, --np);
					}
				}
			}
		}

		const int next_depth = depth + 1;
		clearMyList(xpr->candidates_to_iterate[depth]);

		// noeuds sur lesquels faire un appel récursif
		for (i = xpr->beginP[depth]; i < np; i++) {
			pushback(xpr->candidates_to_iterate[depth], xpr->XPnode[i]);
		}

		// parcours des noeuds sur lesquels faire un appel récursif
		for (i = 0; i < xpr->candidates_to_iterate[depth]->n; i++) {
			u = xpr->candidates_to_iterate[depth]->tab[i];

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
			pushback(xpr->R, u);

			// Appel Récursif
			// BKtemporal(xpr, b, e_new, N, S, cpt, next_depth, PIVOT);
			BKtemporal(xpr, b, e_new, N, S, mc, next_depth, PIVOT);
			BK_not_called = false;

			// Enlever de X les éléments de S[u]
			for (j = 0; j < S[u]->n[depth]; j++) {
				v = S[u]->node[j];
				// assert(is_in_X(xpr, v, next_depth));
				swap_from_X_to_P(xpr, v, next_depth);
			}

			// Enlever u de R
			popback(xpr->R);

			// Enlever u de P et le mettre dans X, à depth
			swap_from_P_to_X(xpr, u, depth);
		}

		// Réordonner X pour remettre à la bonne place le X du début
		for (i = 0; i < xpr->candidates_to_iterate[depth]->n; i++) {
			u = xpr->candidates_to_iterate[depth]->tab[i];
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
			if (xpr->R->n > mc->maxCliqueSize) {
				mc->maxCliqueSize = xpr->R->n;
			}
		}
		// (*cpt)++;
		mc->nMaxCliques++;
		// mc->sumSizeCliques += xpr->R->n;

		// Print Clique
		// cout << b << " " << e << " ";
		// for (i = 0; i < xpr->R->n; i++) {
		// 	cout << xpr->R->tab[i] << " ";
		// }
		// cout << endl;
		printf("%d %d ", b, e);
		for (i = 0; i < xpr->R->n; i++) {
			printf("%d ", xpr->R->tab[i]);
		}
		printf("\n");

		// ostringstream stringStream;
		// stringStream << b << " " << e << " ";
		// for (i = 0; i < xpr->R->n; i++)
		// {
		//     stringStream << xpr->R->tab[i] << " ";
		// }
		// stringStream << endl;
		// cout << stringStream.str();
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

void MaxCliquesFromEdges(MyList* NewEdges, NeighborList** S, MySet* Snodes, int b, NeighborListEnd** N, XPR* xpr,
						 // int *cpt,
						 MyCounter* mc, bool PIVOT) {
	// int a = 1583;
	// cout << "a = " << a << endl;
	// cout << "N[a]->n = " << N[a]->n << endl;
	// cout << "N[a] = ";
	// for (int ja = 0; ja < N[a]->n; ja++)
	// {
	//     cout << N[a]->node[ja] << " ";
	// }
	// cout << endl;

	int i, j, u, v, w, e, iw, w1, iu, iv;
	const int depth = 0;
	const int next_depth = depth + 1;

	for (i = 0; i < NewEdges->n; i += 3) {
		u = NewEdges->tab[i];
		v = NewEdges->tab[i + 1];
		e = NewEdges->tab[i + 2];

		// cout << "u = " << u << endl;
		// cout << xpr->XPindex[u] << endl;
		// cout << xpr->XPnode[xpr->XPindex[u]] << endl;

		// assert(xpr->XPnode[xpr->XPindex[u]] == u);
		// assert(xpr->XPnode[xpr->XPindex[v]] == v);

		// cerr << "u = " << u << endl;
		// cerr << "v = " << v << endl;
		// cerr << "N[u] = ";
		// print_Neighbor_nodes(N[u], 0, N[u]->n);
		// cerr << "N[v] = ";
		// print_Neighbor_nodes(N[v], 0, N[v]->n);

		// Calculer X,P,R et faire l'appel récursif
		xpr->beginX[next_depth] = 0;
		xpr->beginP[next_depth] = 0;
		xpr->endP[next_depth] = 0;

		// cout << "-------" << endl;
		// cout << "u = " << u << endl;
		// print_NeighborListEnd_nodes(N[u], 0, N[u]->n);

		for (iu = 0; iu < N[u]->n[depth]; iu++) {
			// cout << "1 ";
			// print_P(xpr, depth);

			w = N[u]->node[iu];

			// cout << "--" << endl;
			// print_NeighborListEnd_nodes(N[u], 0, N[u]->n);
			// cout << "u = " << u << endl;
			// cout << "w = " << w << endl;

			// assert(xpr->XPnode[xpr->XPindex[w]] == w);

			iv = ind_in_NeighborListEnd(w, N[v], depth);
			if (iv != -1) // w in N[u] INTER N[v]
			{
				if (is_in_NeighborList(w, S[u], depth)) {
					// cout << "Add to X 1" << endl;

					// Ajouter wu à X
					add_to_X0(xpr, w, next_depth);
				}
				else if (is_in_NeighborList(w, S[v], depth)) {
					// cout << "Add to X 2" << endl;

					// Ajouter wu à X
					add_to_X0(xpr, w, next_depth);
				}
				else {
					// cout << "Add to P" << endl;

					// cout << "2 ";
					// print_P(xpr, depth);

					// cout << "w = " << w << endl;

					add_to_P(xpr, w, next_depth);

					// cout << "3 ";
					// print_P(xpr, depth);
				}

				init_timeEndRNode(xpr, w, e, N[u]->end[iu], N[v]->end[iv], next_depth);
			}
		}

		// Ordonner les voisins des noeuds de P
		// print_P(xpr,depth);
		reorderN_XUP(xpr, N, next_depth);
		reorderS_P(xpr, S, next_depth);

		// Initialisation de R = {u,v}
		clearMyList(xpr->R);
		pushback(xpr->R, u);
		pushback(xpr->R, v);

		// Appel du calcul des cliques max
		// BKtemporal(xpr, b, e, N, S, cpt, next_depth, PIVOT);
		BKtemporal(xpr, b, e, N, S, mc, next_depth, PIVOT);

		// ********** COUNT **********
		// Max degré sommet arête
		// mc->sumDegreeEdge1 += max(N[u]->n[depth], N[v]->n[depth]);
		// Taille de l'intersection N[u] INTER N[v]
		// mc->sumDegreeEdge2 += (xpr->endP[next_depth] - xpr->beginX[next_depth]);

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

void cliques_sequential(MyLinkStream* ls_end, Datastructure* d, MyCounter* mc, bool PIVOT) {
	int i, b, e, u, v;
	int old_b = -1;
	NeighborListEnd** N = d->N;
	NeighborList** S = d->S;
	MySet* Snodes = d->Snodes;
	MyList* NewEdges = d->NewEdges;
	XPR* xpr = d->xpr;

	const int depth0 = 0;

	// TIME PER STEP
	// struct timeval t1
	// {
	// }, t2{};
	// gettimeofday(&t1, NULL);

	for (i = 0; i < ls_end->m; i++) {

		b = ls_end->link[i].b;
		e = ls_end->link[i].e;
		u = ls_end->link[i].u;
		v = ls_end->link[i].v;

		if (b == -1) // end link
		{

			if (e >= old_b && NewEdges->n > 0) {
				// MaxCliquesFromEdges(NewEdges, S, Snodes, old_b, N, xpr, cpt, PIVOT);
				MaxCliquesFromEdges(NewEdges, S, Snodes, old_b, N, xpr, mc, PIVOT);

				// TIME FOR THIS STEP
				// gettimeofday(&t2, NULL);
				// cout << old_b << " " << (t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec) <<
				// endl; gettimeofday(&t1, NULL);

				clearMyList(NewEdges);
				old_b = e;
			}

			// Normalement l'arête existe forcément
			remove_node_NeighborListEnd(N[u], v);
			remove_node_NeighborListEnd(N[v], u);
		}
		else // New link
		{
			// cout << "u = " << u << endl;
			// cout << "v = " << v << endl;
			// cout << xpr->XPindex[u] << endl;
			// cout << xpr->XPnode[xpr->XPindex[u]] << endl;

			// assert(xpr->XPnode[xpr->XPindex[u]] == u);
			// assert(xpr->XPnode[xpr->XPindex[v]] == v);

			if (old_b == -1) {
				old_b = b;
			}
			if (b > old_b) {
				// MaxCliquesFromEdges(NewEdges, S, Snodes, old_b, N, xpr, cpt, PIVOT);
				MaxCliquesFromEdges(NewEdges, S, Snodes, old_b, N, xpr, mc, PIVOT);

				// TIME FOR THIS STEP
				// gettimeofday(&t2, NULL);
				// cout << old_b << " " << (t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec) <<
				// endl; gettimeofday(&t1, NULL);

				clearMyList(NewEdges);
				old_b = b;
			}

			// normalement (u,v) n'est pas une arête de N
			// for (w = 0; w < N[u]->n[depth0]; w++)
			// {
			//     assert(N[u]->node[w] != v);
			// }
			add_NeighborListEnd(N[u], v, e);

			// // cout << "----------" << endl;
			// for (w = 0; w < N[v]->n[depth0]; w++)
			// {
			//     // cout << w << endl;
			//     // if (w == u)
			//     // {
			//     //     cout << b << " " << e << " " << u << " " << v << endl;
			//     // }
			//     assert(N[v]->node[w] != u);
			// }
			add_NeighborListEnd(N[v], u, e);

			pushback(NewEdges, u);
			pushback(NewEdges, v);
			pushback(NewEdges, e);
		}
	}
	if (NewEdges->n > 0) {
		MaxCliquesFromEdges(NewEdges, S, Snodes, old_b, N, xpr, mc, PIVOT);
		// TIME FOR THIS STEP
		// gettimeofday(&t2, NULL);
		// cout << old_b << " " << (t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec) << endl;
		// gettimeofday(&t1, NULL);
	}
}

bool MyLink_equals(MyLink l1, MyLink l2) {
	return l1.b == l2.b && l1.e == l2.e && l1.u == l2.u && l1.v == l2.v;
}

char* MyLink_to_string(MyLink* l) {
	char* str = (char*)malloc(100 * sizeof(char));
	sprintf(str, "b = %d, e = %d, u = %d, v = %d", l->b, l->e, l->u, l->v);
	return str;
}

DefVector(MyLink, NO_FREE(MyLink));

int cmp_by_b(const void* a, const void* b) {
	MyLink* l1 = (MyLink*)a;
	MyLink* l2 = (MyLink*)b;
	return l1->b - l2->b;
}

CliqueVector maximal_cliques(Stream* st) {

	bool PIVOT = true;

	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	MyLinkVector links = MyLinkVector_new();
	LinksIterator it = funcs.links_set(st);
	printf("st : %p\n", st);
	FullStreamGraph* fsg = st->stream;
	printf("fsg : %p\n", fsg);
	printf("underlying : %p\n", fsg->underlying_stream_graph);
	printf("funcs : %p\n", funcs.links_present_at_t);
	FOR_EACH_LINK(link_id, it) {
		Link link = funcs.nth_link(st, link_id);
		TimesIterator times_it = funcs.times_link_present(st, link_id);
		FOR_EACH_TIME(time, times_it) {
			MyLink l = (MyLink){.b = time.start, .e = time.end, .u = link.nodes[0], .v = link.nodes[1]};
			MyLinkVector_push(&links, l);
		}
	}

	char* lsFile = "links.txt";

	MyLinkVector_sort(&links, cmp_by_b);
	// write them all to a file
	FILE* f = fopen(lsFile, "w");
	for (size_t i = 0; i < links.size; i++) {
		MyLink l = links.array[i];
		fprintf(f, "%d %d %d %d\n", l.b, l.e, l.u, l.v);
	}
	fclose(f);

	Datastructure* d = allocDatastrucure(lsFile);

	/*cerr << "============================================" << endl;
	cerr << "\t n = " << d->n << endl;
	cerr << "\t m = " << d->m << endl;
	cerr << "\t nT = " << d->ntimestep << endl;
	cerr << "\t degmax = " << d->degmax << endl;
	cerr << "============================================" << endl;

	t2 = time(NULL);
	cerr << "- Time init datastructure = " << (t2 - t1) << "s" << endl;
	t1 = t2;*/

	// cpt = 0;
	MyCounter* mc = alloc_MyCounter();
	// cliques_from_stream(lsFile, d->N, d->S, d->Snodes, d->NewEdges, d->xpr, &cpt, PIVOT);
	cliques_sequential(d->ls_end, d, mc, PIVOT);

	// cerr << "Number of maximal cliques = " << cpt << endl;
	/*cerr << "============================================" << endl;
	cerr << "\t nMaxCliques = " << mc->nMaxCliques << endl;
	cerr << "\t maxCliqueSize = " << mc->maxCliqueSize << endl;
	cerr << "\t nTimeMaxCliques = " << mc->nTimeMaxCliques << endl;
	cerr << "\t nLeafGood = " << mc->nLeafGood << endl;
	cerr << "\t nLeafBad = " << mc->nLeafBad << endl;
	cerr << "\t ratio = ";
	cerr << setprecision(5) << (float)mc->nLeafGood / (float)(mc->nLeafGood + mc->nLeafBad) << endl;
	cerr << "============================================" << endl;

	t2 = time(NULL);
	cerr << "- Time computing cliques = " << (t2 - t1) << "s" << endl;
	t1 = t2;*/

	return CliqueVector_new();
}