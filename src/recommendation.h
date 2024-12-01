#ifndef RECOMMENDATION_H
#define RECOMMENDATION_H

#include "graph.h"
#include "neuralnetwork.h"

void recommendRandomMovie(Graph *graph, int userId);
void recommendTopRatedMovies(Graph *graph, int userId, int n);
void recommendBasedOnSimilarUser(Graph *graph, int userId, int n);
void recommendClosestMovies(Graph *graph, int userId, int n);
void recommendMovies(NeuralNetwork *nn, int userId, int n, Graph *graph);

#endif

