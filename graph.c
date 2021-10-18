#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "Util.h"
#include "graph.h"

TGraphL* createGraphAdjList(int numberOfNodes){
	TGraphL* graph = (TGraphL*)malloc(sizeof(TGraphL));
	graph->nn = numberOfNodes;
	graph->adl = (TNode**)malloc(sizeof(TNode*)*graph->nn);
	for(int i= 0 ; i< graph->nn; i++)
		graph->adl[i] = NULL;
	return graph;
}


void addEdgeList(TGraphL* graph, int v1, char* name1, int v2, char* name2){
	TNode* x = graph->adl[v1];
	while(x != NULL){
		if(x->v == v2)
			return;
		x = x->next;
	}
	TNode* n1 = (TNode*) malloc(sizeof(TNode));
	n1->v = v1;
	n1->name = malloc(sizeof(char)*(strlen(name1)+1));
	strcpy(n1->name,name1);
	n1->sursa = malloc(sizeof(char)*(strlen(name2)+1));
	strcpy(n1->sursa,name2);
	n1->next = NULL;
	TNode* n2 = (TNode*) malloc(sizeof(TNode));
	n2->v = v2;
	n2->name = malloc(sizeof(char)*(strlen(name2)+1));
	strcpy(n2->name,name2);
	n2->sursa = malloc(sizeof(char)*(strlen(name1)+1));
	strcpy(n2->sursa,name1);
	n2->next = NULL;
	if(graph->adl[v1] == NULL)
		graph->adl[v1] = n2;
	else{
		n2->next = graph->adl[v1];
		graph->adl[v1] = n2;
	}
	if(graph->adl[v2] == NULL)
		graph->adl[v2] = n1;
	else{
		n1->next = graph->adl[v2];
		graph->adl[v2] = n1;
	}
}

void destroyGraphAdjList(TGraphL* graph){
	TNode* aux;
	for(int i = 0 ; i< graph->nn; i++)
		while(graph->adl[i] != NULL){
			aux = graph->adl[i];
			graph->adl[i] = graph->adl[i]->next;
			free(aux->sursa);
			free(aux->name);
			free(aux);
		}
	free(graph->adl);
	free(graph);
}

void dfs(TGraphL* G, int* visited,int s,int color){
	visited[s] = color;
	TNode* p = G->adl[s];
	while(p != NULL) {
		if(visited[p->v] == 0)
			dfs(G,visited,p->v,color);
		p = p->next;	
	}
}


