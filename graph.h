#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>

typedef struct Edge {
    int userId;
    int itemId;            // ID of the item (movie) in this edge
    int rating;            // Rating given by the user for the item
    struct Edge *nextEdge; // Pointer to the next edge
} Edge;

typedef struct Node {
    int id;               // ID of the user or item
    struct Edge *edges;   // List of edges (connections to items or users)
    struct Node *next;    // Pointer to the next user/item in the list
} Node;

typedef struct Graph {
    Node *users;          // Linked list of user nodes
    Node *items;          // Linked list of item nodes
} Graph;

// Function prototypes
Graph *createGraph();
Node *createNode(int id);
Node *findNode(Node *head, int id);
int countNodes(Node *head);
void addUser(Graph *graph, int userId);
void addItem(Graph *graph, int itemId);
void addEdge(Graph *graph, int userId, int itemId, int rating);
int degreeOfVertex(Graph *graph, int userId);
void deleteUser(Graph *graph, int userId);
void deleteItem(Graph *graph, int itemId);
void printGraph(Graph *graph);
void printEdges(Node *node);
void parseDataFile(Graph *graph, const char *filename);
void removeEdgesForItem(Graph *graph, int itemId);
void freeEdges(Edge *edge);
void freeNodes(Node *node);
void freeGraph(Graph *graph);

void recommendRandomMovie(Graph *graph, int userId);
void recommendTopRatedMovies(Graph *graph, int userId, int n);
void recommendBasedOnSimilarUser(Graph *graph, int userId, int n);
void recommendClosestMovies(Graph *graph, int userId, int n);

#endif // GRAPH_H
