#ifndef RECOMMENDATION_H
#define RECOMMENDATION_H

#include "graph.h"

void recommendRandomMovie(Graph *graph, int userId);
void recommendTopRatedMovies(Graph *graph, int userId, int n);
void recommendBasedOnSimilarUser(Graph *graph, int userId, int n);
void recommendClosestMovies(Graph *graph, int userId, int n);
void recommendMovieBasedOnItem(Graph *graph, int itemId);
void recommendRandomWalk(Graph* graph, int userId);

void dijkstra(Graph *graph, int userId, int *distances);
int findMinDistance(int *distances, int *visited, int totalNodes);
int *findUsersWithHighRating(Graph *graph, int itemId, int *userCount);
int *findMostLikedMovies(Graph *graph, int *users, int userCount, int excludeItem, int n);
float get_recommendation(int userId, int itemId);
float calculate_error(int userId, int itemId);
int countEdges(Node* node);
Edge* getRandomEdge(Node* node);
Node* randomWalk(Graph* graph, Node* startNode, int walkLength, float alpha);


#endif
