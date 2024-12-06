#ifndef RECOMMENDATION_H
#define RECOMMENDATION_H

#include "graph.h"

void recommendRandomMovie(Graph *graph, int userId);
void recommendTopRatedMovies(Graph *graph, int userId, int n);
void recommendBasedOnSimilarUser(Graph *graph, int userId, int n);
void recommendClosestMovies(Graph *graph, int userId, int n);
void recommendMovieBasedOnItem(Graph *graph, int itemId);

void dijkstra(Graph *graph, int userId, int *distances);
int findMinDistance(int *distances, int *visited, int totalNodes);
int *findUsersWithHighRating(Graph *graph, int itemId, int *userCount);
int *findMostLikedMovies(Graph *graph, int *users, int userCount, int excludeItem, int n);

#endif
