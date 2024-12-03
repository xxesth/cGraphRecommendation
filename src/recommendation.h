#ifndef RECOMMENDATION_H
#define RECOMMENDATION_H

#include "graph.h"

void recommendRandomMovie(Graph *graph, int userId);
void recommendTopRatedMovies(Graph *graph, int userId, int n);
void recommendBasedOnSimilarUser(Graph *graph, int userId, int n);
void recommendClosestMovies(Graph *graph, int userId, int n);
void oldrecommendClosestMovies(Graph *graph, int userId, int n);

int *dijkstra(Graph *graph, Node *startNode);

#endif

