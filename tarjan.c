#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define min(a,b) (a>b)?b:a
#define checkStack(v) v->on_stack

typedef struct _vertex {
    int ref, n_adj, d, low, i_scc, on_stack;
    struct _vertex **adj;
} Vertex;

typedef struct {
    int size, max_size;
    Vertex **stack;
} Vertex_stack;

void initializeStack(Vertex_stack* stack);
void freeStack(Vertex_stack* stack);
void push(Vertex_stack* stack, Vertex* v);
Vertex* pop(Vertex_stack* stack);

int cmpVertexRef(const void* v, const void* u) {
    return ((Vertex*)v)->ref - ((Vertex*)u)->ref;
}

void Tarjan_visit(Vertex* v, int* visited, Vertex_stack* stack, Vertex**** scc, int* n_scc, int** sizes_scc) {
    int i;
    v->low = *visited;
    v->d = (*visited)++;
    push(stack, v);
    for (i=0; i<v->n_adj; i++)
        if (v->adj[i]->d == -1 || checkStack(v->adj[i])) {
            if (v->adj[i]->d == -1)
                Tarjan_visit(v->adj[i], visited, stack, scc, n_scc, sizes_scc);
            v->low = min(v->low, v->adj[i]->low);
        }

    if (v->low == v->d) {
        *scc = (Vertex***)realloc(*scc, ++(*n_scc)*sizeof(Vertex**));
        (*scc)[*n_scc-1] = NULL;
        int size_scc = 0;
        do {
            (*scc)[*n_scc-1] = (Vertex**)realloc((*scc)[*n_scc-1], ++size_scc*sizeof(Vertex*));
            (*scc)[*n_scc-1][size_scc-1] = pop(stack);
        } while ((*scc)[*n_scc-1][size_scc-1] != v);
        *sizes_scc = (int*)realloc(*sizes_scc, *n_scc*sizeof(int));
        (*sizes_scc)[*n_scc-1] = size_scc;
    }
}

int main() {
    int size_graph, n_edge, i, j, k, start, end;
    scanf("%d%d", &size_graph, &n_edge);
    Vertex *graph = (Vertex*)malloc(size_graph*sizeof(Vertex));
    for (i=0; i<size_graph; i++) {
        graph[i].adj = NULL;
        graph[i].ref = i+1;
        graph[i].n_adj = 0;
        graph[i].d = -1;
        graph[i].low = -1;
    }
    for (i=0; i<n_edge; i++) {
        scanf("%d%d", &start, &end);
        graph[start-1].adj = (Vertex**)realloc(graph[start-1].adj, ++(graph[start-1].n_adj)*sizeof(Vertex*));
        graph[start-1].adj[graph[start-1].n_adj-1] = &graph[end-1];
    }

    Vertex ***scc = NULL;
    int n_scc = 0, visited = 0, *sizes_scc = NULL;
    Vertex_stack stack;
    initializeStack(&stack);
    for (i=0; i<size_graph; i++)
        if (graph[i].d == -1)
            Tarjan_visit(&graph[i], &visited, &stack, &scc, &n_scc, &sizes_scc);
    freeStack(&stack);
    
    Vertex **graph_output = (Vertex**)malloc(n_scc*sizeof(Vertex*));
    for (i=0; i<n_scc; i++) {
        scc[i][0]->i_scc = i;
        graph_output[i] = scc[i][0];
        for (j=1; j<sizes_scc[i]; j++) {
            scc[i][j]->i_scc = i;
            if (scc[i][j]->ref < graph_output[i]->ref)
                graph_output[i] = scc[i][j];
        }
    }
    int new_n_adj, *exists = (int*)malloc(n_scc*sizeof(int));
    for (i=0; i<n_scc; i++) {
        memset(exists, 0, n_scc*sizeof(int));
        exists[i] = 1, new_n_adj = 0;
        Vertex **new_adj = NULL;
        for (j=0; j<sizes_scc[i]; j++)
            for (k=0; k<scc[i][j]->n_adj; k++)
                if (!exists[scc[i][j]->adj[k]->i_scc]++) {
                    new_adj = (Vertex**)realloc(new_adj, ++new_n_adj*sizeof(Vertex*));
                    new_adj[new_n_adj-1] = graph_output[scc[i][j]->adj[k]->i_scc];
                }
        qsort(new_adj, new_n_adj, sizeof(Vertex*), cmpVertexRef);
        free(graph_output[i]->adj);
        graph_output[i]->adj = new_adj;
        graph_output[i]->n_adj = new_n_adj;
    }
    free(exists);
    qsort(graph_output, n_scc, sizeof(Vertex*), cmpVertexRef);

    printf("%d\n", n_scc);
    int n_edge_output = 0;
    for (i=0; i<n_scc; i++)
        n_edge_output += graph_output[i]->n_adj;
    printf("%d\n", n_edge_output);
    for (i=0; i<n_scc; i++)
        for (j=0; j<graph_output[i]->n_adj; j++)
            printf("%d %d\n", graph_output[i]->ref, graph_output[i]->adj[j]->ref);

    free(graph_output);
    for (i=0; i<size_graph; i++)
        free(graph[i].adj);
    free(graph);
    for (i=0; i<n_scc; i++)
        free(scc[i]);
    free(scc);
    free(sizes_scc);

    return 0;
}

void initializeStack(Vertex_stack* stack) {
    stack->size = 0;
    stack->max_size = 2;
    stack->stack = (Vertex**)malloc(2*sizeof(Vertex*));
}

void freeStack(Vertex_stack* stack) {
    free(stack->stack);
}

void push(Vertex_stack* stack, Vertex* v) {
    if (stack->size >= stack->max_size) {
        stack->max_size = stack->max_size*2;
        stack->stack = (Vertex**)realloc(stack->stack, stack->max_size*sizeof(Vertex*));
    }
    v->on_stack = 1;
    stack->stack[stack->size++] = v;
}

Vertex* pop(Vertex_stack* stack) {
    if (stack->size == 0)
        return NULL;
    stack->stack[--stack->size]->on_stack = 0;
    return stack->stack[stack->size];
}