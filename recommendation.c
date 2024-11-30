#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "recommendation.h"


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

    // Count the number of items
    int numItems = countNodes(graph->items);

    // Array to store distances to each movie (initialize with max value)
    int *distances = malloc(numItems * sizeof(int));
    for (int i = 0; i < numItems; i++) {
        distances[i] = INT_MAX; // Assume movies are 1-indexed
    }

    // Perform a BFS to calculate weighted distances from the user
    Edge *userEdges = user->edges;
    while (userEdges) {
        // Set initial distance for directly rated movies
        distances[userEdges->itemId - 1] = 1.0 / userEdges->rating;
        userEdges = userEdges->nextEdge;
    }

    // Initialize an array to store the top N movie recommendations
    int (*topMovies)[2] = malloc(n * sizeof(int[2]));
    for (int i = 0; i < n; i++) {
        topMovies[i][0] = -1; // Movie ID
        topMovies[i][1] = INT_MAX; // Distance
    }

    // Traverse items and find unrated movies
    Node *item = graph->items;
    while (item) {
        // Check if the user has already rated this movie
        int rated = 0;
        Edge *userEdge = user->edges;
        while (userEdge) {
            if (userEdge->itemId == item->id) {
                rated = 1;
                break;
            }
            userEdge = userEdge->nextEdge;
        }

        // If unrated, consider for recommendation
        if (!rated && distances[item->id - 1] != INT_MAX) {
            for (int i = 0; i < n; i++) {
                if (distances[item->id - 1] < topMovies[i][1]) {
                    // Shift lower-distance movies down
                    for (int j = n - 1; j > i; j--) {
                        topMovies[j][0] = topMovies[j - 1][0];
                        topMovies[j][1] = topMovies[j - 1][1];
                    }

                    // Insert this movie into the array
                    topMovies[i][0] = item->id;
                    topMovies[i][1] = distances[item->id - 1];
                    break;
                }
            }
        }
        item = item->next;
    }

    // Print the top N movie recommendations
    printf("Top %d closest movie recommendations for user %d:\n", n, userId);
    for (int i = 0; i < n; i++) {
        if (topMovies[i][0] != -1) {
            printf("  Movie ID %d with distance %.2f\n", topMovies[i][0], (float)topMovies[i][1]);
        } else {
            printf("  No more recommendations available.\n");
        }
    }

    free(topMovies);
    free(distances);
}
