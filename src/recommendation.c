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
    Node *mostSimilarUser = NULL;

    Node *otherUser = graph->users;
    while (otherUser) {
        if (otherUser->id != userId) {
            // Count common movies
            int commonMovies = 0;
            Edge *userEdge = user->edges;
            while (userEdge) {
                Edge *otherUserEdge = otherUser->edges;
                while (otherUserEdge) {
                    if (userEdge->itemId == otherUserEdge->itemId) {
//TEST                        printf("count: %d itemid: %d userid: %d\n", commonMovies, otherUserEdge->itemId, otherUserEdge->userId);
                        commonMovies++;
                       // otherUserEdge = otherUserEdge->nextEdge;
                        break;
                    }
                    otherUserEdge = otherUserEdge->nextEdge;
                }
                userEdge = userEdge->nextEdge;
            }

            // Update the most similar user
            if (commonMovies > maxCommonMovies) {
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
    printf("Most similar user to user %d is user %d with %d common movies.\n",
           userId, mostSimilarUser->id, maxCommonMovies);

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
        if (topMovies[i][0] != -1) {
            printf("  Movie ID %d with rating %d\n", topMovies[i][0], topMovies[i][1]);
        }
    }

    free(topMovies);
}

void recommendClosestMovies(Graph *graph, int userId, int n) {
    Node *user = findNode(graph->users, userId);
    if (!user) {
        printf("User with ID %d not found.\n", userId);
        return;
    }

    int totalMovies = countNodes(graph->items); // Get the total number of items
    double *distances = malloc(totalMovies * sizeof(double));
    int *visited = malloc(totalMovies * sizeof(int));

    // Initialize distances and visited array
    for (int i = 0; i < totalMovies; i++) {
        distances[i] = DBL_MAX;
        visited[i] = 0;
    }

    // Traverse the graph from the user's rated movies
    Edge *userEdge = user->edges;
    while (userEdge) {
        int movieId = userEdge->itemId;

        Node *movieNode = findNode(graph->items, movieId);
        if (movieNode) {
            Edge *otherUserEdge = movieNode->edges;

            // Visit other users who rated this movie
            while (otherUserEdge) {
                Node *otherUser = findNode(graph->users, otherUserEdge->itemId);
                if (otherUser && otherUser->id != userId) {
                    Edge *otherUserMovieEdge = otherUser->edges;

                    // Explore movies rated by the other user
                    while (otherUserMovieEdge) {
                        int otherMovieId = otherUserMovieEdge->itemId;

                        // Calculate the inverted rating as the distance weight
                        double rating = otherUserMovieEdge->rating;
                        double distance = (rating > 0) ? 1.0 / rating : DBL_MAX;

                        // Update distance if it is smaller
                        if (!visited[otherMovieId - 1] && distance < distances[otherMovieId - 1]) {
                            distances[otherMovieId - 1] = distance;
                        }

                        otherUserMovieEdge = otherUserMovieEdge->nextEdge;
                    }
                }
                otherUserEdge = otherUserEdge->nextEdge;
            }
        }

        userEdge = userEdge->nextEdge;
    }

    // Find the top N recommendations
    int *topMovies = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        topMovies[i] = -1;
    }

    for (int i = 0; i < totalMovies; i++) {
        if (distances[i] != DBL_MAX && !visited[i]) {
            for (int j = 0; j < n; j++) {
                if (topMovies[j] == -1 || distances[i] < distances[topMovies[j] - 1]) {
                    // Shift lower-ranked movies
                    for (int k = n - 1; k > j; k--) {
                        topMovies[k] = topMovies[k - 1];
                    }
                    topMovies[j] = i + 1; // Store movie ID
                    break;
                }
            }
        }
    }

    // Print recommendations
    printf("Top %d closest movie recommendations for user %d:\n", n, userId);
    for (int i = 0; i < n; i++) {
        if (topMovies[i] != -1) {
            printf("  Movie ID: %d (Proximity: %.2f)\n", topMovies[i], distances[topMovies[i] - 1]);
        } else {
            printf("  No more recommendations available.\n");
        }
    }

    // Free allocated memory
    free(distances);
    free(visited);
    free(topMovies);
}

void recommendMovies(NeuralNetwork *nn, int userId, int n, Graph *graph) {
    double inputs[INPUT_NODES];
    double output;

    printf("Top %d recommendations for user %d:\n", n, userId);

    Node *item = graph->items;
    int recommended = 0;

    while (item && recommended < n) {
        // Skip if the user has already rated the item
        if (hasEdge(graph->users, userId, item->id)) {
            item = item->next;
            continue;
        }

        // Prepare inputs
        inputs[0] = (double)userId;
        inputs[1] = (double)item->id;
        inputs[2] = 0.0; // Default rating for unrated items

        // Predict Rating
        forward(nn, inputs, &output);

        printf("Movie %d: Predicted Rating = %.2f\n", item->id, output);
        recommended++;
        item = item->next;
    }
}
