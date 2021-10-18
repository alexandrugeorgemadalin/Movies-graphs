#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"
#include "Util.h"

#define MOD 350003
#define MAX 100
#define INF 99999

// Structura folosita pentru a crea hashtable-ul
typedef struct Element {
  char* name;
  int nod;
  struct Element *next;
} Element;

// Structura folosita pentru a retine muchiile critice
typedef struct{
	char* name1;
	char* name2;
}Pereche;

long hashfunction(char* str){
	int i,h = 0;
	for(i = 0; i < strlen(str); i++)
		h = (h*26 + abs(str[i]))%MOD;
	return h;
}

Element** initHtable(long size){
	Element** hashtable = (Element**)malloc(sizeof(Element*)*size);
	if(hashtable == NULL){
		free(hashtable);
		printf("Eroare de alocare\n");
	}
	for(int i = 0; i < MOD; i++)
		hashtable[i] = NULL;
	return hashtable;
}

void destroyHtable(Element** hashtable){
	Element *prev, *curr;
	int i;
	for(i = 0; i < MOD; i++){
		if(hashtable[i] != NULL){
			curr = hashtable[i];
			while(curr != NULL){
				prev = curr;
				curr = curr->next;
				free(prev->name);
				free(prev);
			}
		}
		else
			free(hashtable[i]);
	}
	free(hashtable);
}

Element** createHashtable(char* intrare, int* size){
	FILE* in = fopen(intrare,"r");
	int n,i,j;
	fscanf(in,"%d", &n);
	Element** hashtable = initHtable(MOD);
	int idx = 0,nr_actori;
	char *film = malloc(sizeof(char)*MAX);
	char *actor = malloc(sizeof(char)*MAX);
	for(i = 0; i < n; i++){
		fscanf(in,"\n");
		fscanf(in,"%[^\n]",film);
		fscanf(in,"%d",&nr_actori);
		for(j = 0; j < nr_actori; j++){
			fscanf(in,"\n");
			fscanf(in,"%[^\n]",actor);
			long h = hashfunction(actor);
			// Pentru fiecare element ce trebuie introdus se verifica daca lista
			// este goala si se adauga in locul potrivit
			if(hashtable[h] == NULL){
				Element* new = (Element*)malloc(sizeof(Element));
				new->name = malloc(sizeof(char)*(strlen(actor)+1));
				strcpy(new->name,actor);
				new->nod = idx;
				new->next = NULL;
				hashtable[h] = new;
				idx++;
			}
			else{
				Element* e = hashtable[h];
				Element* prev;
				while(e != NULL && strcmp(e->name,actor) != 0){
					prev = e;
					e = e->next;
				}
				if(e == NULL){
					Element* new = (Element*)malloc(sizeof(Element));
					new->name = malloc(sizeof(char)*(strlen(actor)+1));
					strcpy(new->name,actor);
					new->nod = idx;
					new->next = NULL;
					prev->next = new;
					idx++;
				}
			}
		}	
	}
	*size = idx;
	free(actor);
	free(film);
	fclose(in);
	return hashtable;
}

TGraphL* createGraph(char* intrare,Element** hashtable, int size){
	FILE* in = fopen(intrare,"r");
	int n,i,j,k;
	int nr_actori;
	char *film = malloc(sizeof(char)*MAX);
	char *actor = malloc(sizeof(char)*MAX);
	long h1,h2;
	TGraphL* graph;
	graph = createGraphAdjList(size);
	fscanf(in,"%d", &n);
	for(i = 0; i< n; i++){
		fscanf(in,"\n");
		fscanf(in,"%[^\n]",film);
		fscanf(in,"%d",&nr_actori);
		char** actori = (char**)malloc(sizeof(char*)*nr_actori);
		for(j = 0; j < nr_actori; j++){
			actori[j] = malloc(sizeof(char)*MAX);
			fscanf(in,"\n");
			fscanf(in,"%[^\n]",actori[j]);
		}
		for(j = 0; j < nr_actori - 1; j++){
			h1 = hashfunction(actori[j]);
			Element* curr = hashtable[h1];
			// Se verifica daca numele nodului corespunde cu numele actorului
			if(strcmp(hashtable[h1]->name,actori[j]) != 0){
					while(curr != NULL && strcmp(curr->name,actori[j]) != 0){
						curr = curr->next;
					}
				}
			for(k = j+1; k < nr_actori; k++){
				h2 = hashfunction(actori[k]);
				Element* x = hashtable[h2];
				// Se face aceeasi verificare ca mai inainte
				if(strcmp(hashtable[h2]->name,actori[k]) != 0){
					while(x != NULL && strcmp(x->name,actori[k]) != 0){
						x = x->next;
					}
				}
				// Adaugare muchie noua in graf
				addEdgeList(graph,curr->nod,curr->name,
						x->nod,x->name);
			}
		}
		for(j = 0; j < nr_actori; j++)
			free(actori[j]);
		free(actori);
	}
	fclose(in);
	free(actor);
	free(film);
	return graph;
}
// Functie folosita pentru a sorta alfabetic elementele componentei conexe
int compare(const void *a, const void *b){
	return strcmp(*(const char**)a,*(const char**)b);
}


void conex(TGraphL* graph, char* iesire){
	int* visited = calloc(graph->nn,sizeof(int));
	int i,j,idx = 0;
	//Fiecare componenta conexa se "coloreaza" cu numarul componentei
	for(i = 0; i < graph->nn; i++){
		if(visited[i] == 0){
			idx++;
			dfs(graph,visited,i,idx);
		}
	}
	// In comp se calculeaza numarul de elemente pentru fiecare componenta conexa
	int* comp = calloc(idx+1,sizeof(int)); 
	for(i = 0; i < graph->nn; i++)
		comp[visited[i]] += 1;
	int max = comp[0];
	int nr = 1;
	// Aflarea componentei cu numarul maxim de elemente
	for(i = 1; i <= idx ; i++)
		if(max < comp[i]){
			max = comp[i];
			nr = i;
		}
	free(comp);
	char **actori = (char**) malloc(sizeof(char*)*max);
	FILE* out = fopen(iesire,"w");
	fprintf(out,"%d\n", max);
	j = 0;
	// Se creaza un vector cu elementele din componenta conexa aflata mai sus
	for(i = 0 ; i < graph->nn; i++){
		if(visited[i] == nr){
			actori[j] = malloc(sizeof(char)*(strlen(graph->adl[i]->sursa)+1));
			strcpy(actori[j],graph->adl[i]->sursa);
			j++;
		}
	}
	// Se sorteaza alfabetic elementele componentei conexe si se scriu in fisier
	qsort(actori,max,sizeof(char*),compare);
	for(i = 0; i < max; i++){
		fprintf(out,"%s\n",actori[i]);
		free(actori[i]);
	}
	fclose(out);
	free(actori);
	free(visited);
}
// Parcurge in latime graful pornind din nodul sursa pana ajunge in nodul dorit
// Se actualizeaza din nod in nod distanta de la sursa pana la nodul respectiv
void bfs(TGraphL* graph, int s, int d, char* iesire){
	int* visited = calloc(graph->nn, sizeof(int));
	int* dist = calloc(graph->nn, sizeof(int));
	Queue* coada = createQueue();
	int x;
	TNode* p;
	enqueue(coada,s);
	while(!isQueueEmpty(coada)){
		x = front(coada);
		dequeue(coada);
		visited[x] = 1;
		p = graph->adl[x];
		while(p != NULL){
			if(visited[p->v] == 0){
				visited[p->v] = 1;
				dist[p->v] = dist[x] + 1;
				enqueue(coada,p->v);
			}
			if(p->v == d){
				break;
			}
			p = p->next;
		}
	}
	// La final, gradul de inrudire este -1 daca nu se poate ajunge din nodul 
	// sursa in nodul final respectiv este egal cu distanta pana la acel nod
	FILE* out = fopen(iesire,"w");
	if(dist[d] == 0)
		dist[d] = -1;
	fprintf(out,"%d\n",dist[d]);
	fclose(out);
	free(dist);
	destroyQueue(coada);
	free(visited);
}

void inrudire(TGraphL* graph, Element** hashtable, char* intrare, char* iesire){
	FILE* in = fopen(intrare,"r");
	char* name1 = malloc(sizeof(char)*MAX);
	char* name2 = malloc(sizeof(char)*MAX);
	char* buffer = malloc(sizeof(char)*MAX);
	int n, nr_actori,i,j;
	// Se citesc in buffer toate informatiile de care nu avem nevoie la aceasta
	// cerinta
	fscanf(in,"%d", &n);
	for(i = 0; i < n; i++){
		fscanf(in,"\n");
		fscanf(in,"%[^\n]",buffer);
		fscanf(in,"%d",&nr_actori);
		for(j = 0; j < nr_actori; j++){
			fscanf(in,"\n");
			fscanf(in,"%[^\n]",buffer);
		}
	}
	fscanf(in,"\n");
	fscanf(in,"%[^\n]",name1);
	fscanf(in,"\n");
	fscanf(in,"%[^\n]",name2);
	// Pentru cei doi actori, se cauta in hashtable indexul nodului corespunzator
	// numelui fiecarui actor 
	long h1, h2;
	h1 = hashfunction(name1);
	h2 = hashfunction(name2);
	Element* curr1 = hashtable[h1];
	if(strcmp(hashtable[h1]->name,name1) != 0){
		while(curr1 != NULL && strcmp(curr1->name,name1) != 0){
			curr1 = curr1->next;
			}
	}
	Element* curr2 = hashtable[h2];
	if(strcmp(hashtable[h2]->name,name2) != 0){
		while(curr2 != NULL && strcmp(curr2->name,name2) != 0){
			curr2 = curr2->next;
			}
	}
	// Se apeleaza functia definita mai sus pentru a afla gradul de inrudire
	bfs(graph,curr1->nod,curr2->nod,iesire);		

	free(buffer);
	free(name2);
	free(name1);
	fclose(in);
}


void dfsB(TGraphL* graph, int i, int timp, int* idx, int* low, int* pi, Pereche** pereche, int* nr){
	idx[i] = timp;
	low[i] = timp;
	timp++;
	TNode* u = graph->adl[i];
	while(u != NULL){
		if(u->v != pi[i]){
			if(idx[u->v] == -1){
				pi[u->v] = i;
				dfsB(graph,u->v,timp,idx,low,pi,pereche,nr);
				if(low[i] > low[u->v])
					low[i] = low[u->v];
				if(low[u->v] > idx[i]){
					pereche[*nr] = (Pereche*)malloc(sizeof(Pereche));
					pereche[*nr]->name1 = malloc(sizeof(char)*(strlen(graph->adl[i]->sursa)+1));
					strcpy(pereche[*nr]->name1,graph->adl[i]->sursa);
					pereche[*nr]->name2 = malloc(sizeof(char)*(strlen(u->name)+1));
					strcpy(pereche[*nr]->name2,u->name);
					(*nr)++;
				}
			}
			else{
			if(low[i] > low[u->v])
				low[i] = low[u->v];
			}
		}
		u = u->next;
	}
}
int comp(const void *a, const void *b){
	if(strcmp((*(Pereche**)a)->name1,(*(Pereche**)b)->name1) == 0)
		return strcmp((*(Pereche**)a)->name2,(*(Pereche**)b)->name2);
	return strcmp((*(Pereche**)a)->name1,(*(Pereche**)b)->name1);
}


void punti(TGraphL* graph, char* iesire){
	int* idx = malloc(sizeof(int)*graph->nn);
	int* low = malloc(sizeof(int)*graph->nn);
	int* pi = calloc(graph->nn, sizeof(int));

	int nr = 0;
	// Vector in care se retine numele actorilor din fiecare muchie critica
	Pereche** pereche = (Pereche**)malloc(sizeof(Pereche*)*MAX);
	int i, timp = 0;
	for(i = 0; i < graph->nn ; i++){
		idx[i] = -1;
		low[i] = INF;
	}
	for(i = 0; i < graph->nn; i++){
		if(idx[i] == -1)
			dfsB(graph,i,timp,idx,low,pi,pereche,&nr);
	}
	FILE* out = fopen(iesire,"w");
	fprintf(out,"%d\n", nr);
	char* aux;
	// Se sorteaza alfabetic numele actorilor din fiecarei muchie critica
	for(i = 0; i < nr; i++){
		if(strcmp(pereche[i]->name1,pereche[i]->name2) > 0){
			aux = pereche[i]->name2;
			pereche[i]->name2 = pereche[i]->name1;
			pereche[i]->name1 = aux;
		}
	}
	// Se sorteaza alfabetic toate muchiile critice
	qsort(pereche,nr,sizeof(char*),comp);
	for(i = 0; i < nr; i++)
		fprintf(out,"%s %s\n",pereche[i]->name1, pereche[i]->name2);
	fclose(out);
	for(i = 0; i < nr; i++){
		free(pereche[i]->name1);
		free(pereche[i]->name2);
		free(pereche[i]);
	}
	free(pereche);
	free(pi);
	free(low);
	free(idx);

}


int main(int argc, char *argv[]){
	int size;
	Element** hashtable = createHashtable(argv[2],&size);
	TGraphL* graph = createGraph(argv[2],hashtable,size);
	if(strcmp(argv[1],"-c1") == 0){
		conex(graph,argv[3]);
	}
	if(strcmp(argv[1],"-c2") == 0){
		inrudire(graph,hashtable,argv[2],argv[3]);
	}
	if(strcmp(argv[1],"-c3") == 0){
		punti(graph,argv[3]);
	}
	destroyGraphAdjList(graph);
	destroyHtable(hashtable);
	return 0;
}