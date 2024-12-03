#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include "recommendation.h"
#include "neuralnetwork.h"

void recommendRandomMovie(Graph *graph, int userId) {
    Node *user = findNode(graph->users, userId); 
    if (!user) {
        printf("User ID %d not found!\n", userId);
        return;
    }
    printf("\n\n ***** \n\n GIVEN USER ID: %d, FOUND: %d", userId, user->id);
    printEdges(user);

    Node *item = graph->items;
    int itemcount = 0;
    while (item){
        itemcount++;
        item = item->next;
    }
    
    int *unratedMovies = malloc(itemcount * sizeof(int));
    int unratedCount = 0;

    Node *movie = graph->items;
    while (movie) {
        // Check if the user has rated this movie
        int rated = 0;
        Edge *edge = user->edges;
        while (edge) {
            if (edge->itemId == movie->id) {
                rated = 1;
                break;
            }
            edge = edge->nextEdge;
        }

        if (!rated) {
            unratedMovies[unratedCount++] = movie->id;
            //bir listeye o filmin id'sini ekliyor
        }

        movie = movie->next;
    }

    if (unratedCount == 0) {
        printf("User %d has rated all movies. No recommendations available.\n", userId);
        free(unratedMovies);
        return;
    }

    // Randomly select a movie from the unrated list
    srand(time(NULL)); // Seed the random number generator
    int randomIndex = rand() % unratedCount;
    int recommendedMovie = unratedMovies[randomIndex];

    printf("Recommended movie for user %d: Movie ID %d\n", userId, recommendedMovie);

    free(unratedMovies);
}

void recommendTopRatedMovies(Graph *graph, int userId, int n) {
    Node *user = findNode(graph->users, userId); 
    if (!user) {
        printf("User ID %d not found!\n", userId);
        return;
    }

    // Array to store top N movies: [movieID, rating]
    int (*topMovies)[2] = malloc(n * sizeof(int[2]));
    for (int i = 0; i < n; i++) {
        topMovies[i][0] = -1; // Movie ID
        topMovies[i][1] = 0;  // Rating
    }

    Node *movie = graph->items;
    
    //TEST
    //printf("\n\n*****\nEDGES OF %d MOVIE\n\n",movie->next->id);
    //printEdges(findNode(graph->items, 133));
    
    while (movie) {
        // Check if the user has rated this movie
        int rated = 0;
        Edge *edge = user->edges;
        while (edge) {
            if (edge->itemId == movie->id) {
                rated = 1;
                break;
            }
            edge = edge->nextEdge;
        }

        if (!rated) {
            // Calculate the maximum rating for this movie
            Edge *movieEdge = movie->edges;
            int maxRating = 0;
            while (movieEdge) {
                if (movieEdge->rating > maxRating) {
                    maxRating = movieEdge->rating;
                }
                movieEdge = movieEdge->nextEdge;
            }

            // Check if this movie qualifies for the top N
            for (int i = 0; i < n; i++) {
                if (maxRating > topMovies[i][1]) {
                    // Shift lower-rated movies down
                    for (int j = n - 1; j > i; j--) {
                        topMovies[j][0] = topMovies[j - 1][0];
                        topMovies[j][1] = topMovies[j - 1][1];
                    }

                    // Insert this movie
                    topMovies[i][0] = movie->id;
                    topMovies[i][1] = maxRating;
                    break;
                }
            }
        }

        movie = movie->next;
    }

    // Print the top N movies
    printf("Top %d movie recommendations for user %d:\n", n, userId);
    for (int i = 0; i < n; i++) {
        if (topMovies[i][0] != -1) {
            printf("  Movie ID %d with rating %d\n", topMovies[i][0], topMovies[i][1]);
        }
    }

    free(topMovies);
}

void recommendBasedOnSimilarUser(Graph *graph, int userId, int n) {
    Node *user = findNode(graph->users, userId);
    if (!user) {
        printf("User with ID %d not found.\n", userId);
        return;
    }

    // Find the most similar user
    int maxCommonMovies = 0;
    int maxSimilarity = 0;
    Node *mostSimilarUser = NULL;

    Node *otherUser = graph->users;
    while (otherUser) {
        if (otherUser->id != userId) {
            // Count common movies
            int commonMovies = 0;
            int similarity = 0;
            Edge *userEdge = user->edges;
            while (userEdge) {
                Edge *otherUserEdge = otherUser->edges;
                while (otherUserEdge) {
                    if (userEdge->itemId == otherUserEdge->itemId) {
//TEST                        printf("count: %d itemid: %d userid: %d\n", commonMovies, otherUserEdge->itemId, otherUserEdge->userId);
                        commonMovies++;
                        if (userEdge->rating >= otherUserEdge->rating){
                            similarity += 6 - (userEdge->rating - otherUserEdge->rating);
                        }else if (userEdge->rating < otherUserEdge->rating){
                            similarity += 6 - (otherUserEdge->rating - userEdge->rating);
                        }

                       // otherUserEdge = otherUserEdge->nextEdge;
                        break;
                    }
                    otherUserEdge = otherUserEdge->nextEdge;
                }
                userEdge = userEdge->nextEdge;
            }

            // Update the most similar user
            if (similarity > maxSimilarity) {
                maxSimilarity = similarity;
                maxCommonMovies = commonMovies;
                mostSimilarUser = otherUser;
            }
        }
        otherUser = otherUser->next;
    }
    
    if (!mostSimilarUser) {
        printf("No similar users found for user %d.\n", userId);
        return;
    }

    //TEST
    //printEdges(user);
    //printEdges(mostSimilarUser);
    
    // Recommend movies based on the most similar user
    printf("Most similar user to user %d is user %d with %d common movies and %d similarity score.\n",
           userId, mostSimilarUser->id, maxCommonMovies, maxSimilarity);

    // Find top N recommendations
    int (*topMovies)[2] = malloc(n * sizeof(int[2]));
    for (int i = 0; i < n; i++) {
        topMovies[i][0] = -1; // Movie ID
        topMovies[i][1] = 0;  // Rating
    }

    Edge *similarUserEdge = mostSimilarUser->edges;
    while (similarUserEdge) {
        // Check if the target user has already rated this movie
        int rated = 0;
        Edge *userEdge = user->edges;
        while (userEdge) {
            if (userEdge->itemId == similarUserEdge->itemId) {
                rated = 1;
                break;
            }
            userEdge = userEdge->nextEdge;
        }
        
        if (!rated) {
            // Check if the movie qualifies for the top N
            for (int i = 0; i < n; i++) {
                if (similarUserEdge->rating > topMovies[i][1]) {
                    // Shift lower-rated movies down
                    for (int j = n - 1; j > i; j--) {
                        topMovies[j][0] = topMovies[j - 1][0];
                        topMovies[j][1] = topMovies[j - 1][1];
                    }

                    // Insert this movie
                    topMovies[i][0] = similarUserEdge->itemId;
                    topMovies[i][1] = similarUserEdge->rating;
                    
                    break;
                }
            }
        }

        similarUserEdge = similarUserEdge->nextEdge;
    }

    // Print the top N movies
    printf("Top %d movie recommendations for user %d:\n", n, userId);
    for (int i = 0; i < n; i++) {
        if (topMovies[i][0] != -1 && topMovies[i][1] != 0) {
            printf("  Movie ID %d with rating %d\n", topMovies[i][0], topMovies[i][1]);
        }
        else{
            break;
        }
    }
    
    free(topMovies);
}

// Helper function to find the node with the smallest distance
static int findMinDistanceNode(int *distances, int *visited, int totalNodes) {
    int minDistance = INT_MAX;
    int minIndex = -1;

    for (int i = 0; i < totalNodes; i++) {
        if (!visited[i] && distances[i] < minDistance) {
            minDistance = distances[i];
            minIndex = i;
        }
    }

    return minIndex;
}

// Dijkstra's algorithm
int *dijkstra(Graph *graph, Node *startNode) {
    int totalNodes = countNodes(graph->items); // Use countNodes instead of graph->items
    int *distances = malloc(totalNodes * sizeof(int));
    int *visited = calloc(totalNodes, sizeof(int));

    // Initialize distances to infinity
    for (int i = 0; i < totalNodes; i++) {
        distances[i] = INT_MAX;
    }
    distances[startNode->id] = 0; // Distance to start node is zero

    for (int i = 0; i < totalNodes - 1; i++) {
        int currentNode = findMinDistanceNode(distances, visited, totalNodes);
        if (currentNode == -1) break; // No reachable nodes left
        visited[currentNode] = 1;

        Node *current = findNode(graph->items, currentNode);
        Edge *edge = current->edges;

        while (edge) {
            int neighbor = edge->itemId;
            if (!visited[neighbor] && distances[currentNode] != INT_MAX) {
                int newDistance = distances[currentNode] + (5 - edge->rating); // Invert weight
                if (newDistance < distances[neighbor]) {
                    distances[neighbor] = newDistance;
                }
            }
            edge = edge->nextEdge;
        }
    }

    free(visited);
    return distances;
}

// Updated recommendClosestMovies using Dijkstra's algorithm
void recommendClosestMovies(Graph *graph, int userId, int n) {
    Node *userNode = findNode(graph->users, userId);
    if (!userNode) {
        printf("User %d not found.\n", userId);
        return;
    }
    int totalItems = countNodes(graph->items);
    int *distances = dijkstra(graph, userNode);
    
    // Find the n closest unrated movies
    int *recommendedMovies = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        recommendedMovies[i] = -1;
    }

    for (int i = 0; i < totalItems; i++) {
        Node *itemNode = findNode(graph->items, i);
        if (itemNode == NULL){
            continue;
        }
        if (!hasEdge(userNode, itemNode->id)) { // Check if the user has rated this movie
            for (int j = 0; j < n; j++) {
                if (recommendedMovies[j] == -1 || distances[i] < distances[recommendedMovies[j]]) {
                    for (int k = n - 1; k > j; k--) {
                        recommendedMovies[k] = recommendedMovies[k - 1];
                    }
                    recommendedMovies[j] = i;
                    break;
                }
            }
        }
    }

    printf("Recommended Closest Movies for User %d:\n", userId);
    for (int i = 0; i < n && recommendedMovies[i] != -1; i++) {
        printf("Movie ID: %d, Distance: %d\n", recommendedMovies[i], distances[recommendedMovies[i]]);
    }

    free(recommendedMovies);
    free(distances);
}

//void recommendMovies(NeuralNetwork *nn, int userId, int n, Graph *graph) {
//    double inputs[INPUT_NODES];
//    double output;
//
//    printf("Top %d recommendations for user %d:\n", n, userId);
//
//    Node *item = graph->items;
//    int recommended = 0;
//
//    while (item && recommended < n) {
//        // Skip if the user has already rated the item
//        if (hasEdge(graph->users, userId, item->id)) {
//            item = item->next;
//            continue;
//        }
//
//        // Prepare inputs
//        inputs[0] = (double)userId;
//        inputs[1] = (double)item->id;
//        inputs[2] = 0.0; // Default rating for unrated items
//
//        // Predict Rating
//        forward(nn, inputs, &output);
//
//        printf("Movie %d: Predicted Rating = %.2f\n", item->id, output);
//        recommended++;
//        item = item->next;
//    }
//}
