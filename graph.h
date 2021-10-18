#ifndef GRAPH_H_
#define GRAPH_H_
#include "Util.h"

typedef struct TNode{
	int v; // Numarul nodului Destination node index
	char* sursa;
	char* name;
	struct TNode *next; 
}TNode, *ATNode;

typedef struct{
	int nn; // Numarul de noduri din graf
	ATNode *adl; // Lista de noduri
}TGraphL;

TGraphL* createGraphAdjList(int numberOfNodes);
void addEdgeList(TGraphL* graph, int v1, char* name1, int v2, char* name2);
void dfs(TGraphL* G, int* visited, int s,int color);
void destroyGraphAdjList(TGraphL* graph);

#endif
