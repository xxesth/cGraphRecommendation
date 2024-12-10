#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include "recommendation.h"
#include "neuralnetwork.h"

#define MOSTLIKEDMOVIECOUNT 5
#define MODEL_PATH "./model/trained_model.bin"

void recommendRandomMovie(Graph *graph, int userId) {
    printf("\nSoru 1- Random movie (unrated)\n");
    Node *user = findNode(graph->users, userId); 
    if (!user) {
        printf("User ID %d not found!\n", userId);
        return;
    }
    //printf("\n\n ***** \n\n GIVEN USER ID: %d, FOUND: %d", userId, user->id);
    //printEdges(user);

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
    float error = calculate_error(userId, recommendedMovie);

    printf("Recommended movie for user %d: Movie ID %d. Error: %f\n", userId, recommendedMovie, error);

    printf("\n");
    free(unratedMovies);
}

void recommendTopRatedMovies(Graph *graph, int userId, int n) {
    printf("Soru 2-\n");
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
            float error = calculate_error(userId, topMovies[i][0]);
            printf("  Movie ID %d with rating %d. Error: %f\n", topMovies[i][0], topMovies[i][1], error);
        }
    }

    printf("\n");
    free(topMovies);
}

void recommendBasedOnSimilarUser(Graph *graph, int userId, int n){
    printf("Soru 3-\n");
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
            float error = calculate_error(userId, topMovies[i][0]);
            printf("Movie ID %d with rating %d. Error: %f\n", topMovies[i][0], topMovies[i][1], error);
        }
        else{
            break;
        }
    }
    
    printf("\n");
    free(topMovies);
}

// Helper function to find the node with the smallest distance in the unvisited set
int findMinDistance(int *distances, int *visited, int totalNodes) {
    int min = INT_MAX;
    int minIndex = -1;

    for (int i = 0; i < totalNodes; i++) {
        if (!visited[i] && distances[i] < min) {
            min = distances[i];
            minIndex = i;
        }
    }

    return minIndex;
}

// Dijkstra's algorithm to calculate shortest distances from the user to all items
void dijkstra(Graph *graph, int userId, int *distances) {
    int totalItems = countNodes(graph->items); // Number of item nodes
    int totalNodes = totalItems; // Items are 1-indexed in the graph
    int *visited = calloc(totalNodes, sizeof(int)); // Track visited nodes

    // Initialize distances to "infinity"
    for (int i = 0; i < totalNodes; i++) {
        distances[i] = INT_MAX;
    }

    // Start from the given user
    Node *user = findNode(graph->users, userId);
    if (!user) {
        fprintf(stderr, "User with ID %d not found.\n", userId);
        free(visited);
        return;
    }

    // Set distances for directly connected items
    distances[userId - 1] = 0; // Start node has a distance of 0
    Edge *edge = user->edges;
    while (edge) {
        distances[edge->itemId - 1] = edge->rating; // Distance is the rating (edge weight)
        edge = edge->nextEdge;
    }

    // Dijkstra's algorithm
    for (int i = 0; i < totalNodes - 1; i++) {
        // Find the unvisited node with the smallest distance
        int u = findMinDistance(distances, visited, totalNodes);
        if (u == -1) break; // No reachable unvisited nodes remain

        visited[u] = 1;

        // Update distances for all neighbors of `u`
        Node *item = findNode(graph->items, u + 1); // Convert 0-indexed to itemId
        if (!item) continue;

        edge = item->edges;
        while (edge) {
            if (!visited[edge->userId - 1] && distances[u] != INT_MAX) {
                int newDist = distances[u] + edge->rating;
                if (newDist < distances[edge->userId - 1]) {
                    distances[edge->userId - 1] = newDist;
                }
            }
            edge = edge->nextEdge;
        }
    }

    free(visited);
}

void recommendClosestMovies(Graph *graph, int userId, int n) {
    printf("Soru 4-\n");
    int totalItems = countNodes(graph->items);
    int *distances = malloc(totalItems * sizeof(int));
    dijkstra(graph, userId, distances);

    // Collect unrated movies with their distances
    int **movieDistances = malloc(totalItems * sizeof(int *));
    int count = 0;
    Node *user = findNode(graph->users, userId);

    // Check which movies are unrated by the user
    for (int i = 0; i < totalItems; i++) {
        if (!hasEdge(user, i + 1) && distances[i] != INT_MAX) {
            movieDistances[count] = malloc(2 * sizeof(int));
            movieDistances[count][0] = i + 1;        // Movie ID
            movieDistances[count][1] = distances[i]; // Distance
            count++;
        }
    }

    // Sort unrated movies by distance (ascending)
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (movieDistances[i][1] > movieDistances[j][1]) {
                int *temp = movieDistances[i];
                movieDistances[i] = movieDistances[j];
                movieDistances[j] = temp;
            }
        }
    }

    // Recommend top `n` movies
    printf("Closest movies for user %d:\n", userId);
    for (int i = 0; i < n && i < count; i++) {
        float error = calculate_error(userId, movieDistances[i][0]);
        printf("Movie ID: %d, Distance: %d, Error: %f\n", movieDistances[i][0], movieDistances[i][1], error);
        free(movieDistances[i]);
    }

    printf("\n");
    free(movieDistances);
    free(distances);
}

int *findUsersWithHighRating(Graph *graph, int itemId, int *userCount) {
    Node *itemNode = findNode(graph->items, itemId);
    if (!itemNode) return NULL;

    Edge *edge = itemNode->edges;
    int *users = malloc((countNodes(graph->users) + 1) * sizeof(int)); // 1-based indexing
    *userCount = 0;

    while (edge) {
        if (edge->rating == 5) {
            users[*userCount] = edge->userId; // Store user ID directly
            (*userCount)++;
        }
        edge = edge->nextEdge;
    }
    return users;
}

int *findMostLikedMovies(Graph *graph, int *users, int userCount, int excludeItem, int n) {
    int *movieLikes = calloc(countNodes(graph->items) + 1, sizeof(int)); // 1-based indexing
    int *topMovies = malloc(n * sizeof(int)); // Array to store the top `n` movies
    for (int i = 0; i < n; i++) {
        topMovies[i] = -1; // Initialize to invalid movie IDs
    }

    // Count likes for each movie
    for (int i = 0; i < userCount; i++) {
        Node *userNode = findNode(graph->users, users[i]);
        if (!userNode) continue;

        Edge *edge = userNode->edges;
        while (edge) {
            if (edge->rating == 5 && edge->itemId != excludeItem) {
                movieLikes[edge->itemId]++;
            }
            edge = edge->nextEdge;
        }
    }

    // Find top `n` movies with the most likes
    for (int i = 0; i < n; i++) {
        int maxLikes = 0, mostLikedMovie = -1;
        for (int j = 1; j <= countNodes(graph->items); j++) { // Start from 1
            if (movieLikes[j] > maxLikes) {
                maxLikes = movieLikes[j];
                mostLikedMovie = j;
            }
        }
        if (mostLikedMovie != -1) {
            //printf("most liked movie: %d\n", mostLikedMovie);
            topMovies[i] = mostLikedMovie;
            movieLikes[mostLikedMovie] = 0; // Mark as processed
        }
    }

    free(movieLikes);
    return topMovies;
}

void recommendMovieBasedOnItem(Graph *graph, int itemId) {
    printf("Soru 5-\n");
    int userCount = 0;
    int *users = findUsersWithHighRating(graph, itemId, &userCount);

    if (!users || userCount == 0) {
        printf("No users rated this movie with 5.\n");
        free(users);
        return;
    }

    int *recommendedMovies = findMostLikedMovies(graph, users, userCount, itemId, MOSTLIKEDMOVIECOUNT);
    int *recommendedMovieTracker = malloc((countNodes(graph->items) + 1) * sizeof(int)); // To track recommended movies
    for (int i = 0; i <= countNodes(graph->items); i++) {
        recommendedMovieTracker[i] = 0; // Initialize all to 0 (not recommended)
    }

    for (int i = 0; i < MOSTLIKEDMOVIECOUNT; i++) {
        if (recommendedMovies[i] == -1) continue;

        // Skip this movie if it's already recommended in this round
        if (recommendedMovieTracker[recommendedMovies[i]] == 1) {
            continue;
        }

        int hasUnratedUsers = 0;

        for (int j = 0; j < userCount; j++) {
            Node *userNode = findNode(graph->users, users[j]);
            if (!userNode) continue;

            Edge *edge = userNode->edges;
            int hasRated = 0;

            while (edge) {
                if (edge->itemId == recommendedMovies[i]) {
                    hasRated = 1;
                    break;
                }
                edge = edge->nextEdge;
            }

            if (!hasRated) {
                float error = calculate_error(users[j], recommendedMovies[i]);
                printf("Movie %d is recommended for User %d. Error: %f\n", recommendedMovies[i], users[j], error);
                hasUnratedUsers = 1;
            }
        }

        // Mark this movie as recommended
        if (hasUnratedUsers) {
            recommendedMovieTracker[recommendedMovies[i]] = 1;
        }
    }

    // If all movies were already rated, print no recommendation message
    int allRated = 1;
    for (int i = 0; i < MOSTLIKEDMOVIECOUNT; i++) {
        if (recommendedMovies[i] != -1 && recommendedMovieTracker[recommendedMovies[i]] == 0) {
            allRated = 0;
            printf("No possible recommendations.\n");
            break;
        }
    }

    if (allRated) {
        printf("No more recommendation is found.\n");
    }
    
    printf("\n");
    free(recommendedMovies);
    free(users);
    free(recommendedMovieTracker);
}

float get_recommendation(int userId, int itemId){
    static MatrixFactorization* model = NULL;

    // Load model if not already loaded
    if (model == NULL) {
        model = loadModel(MODEL_PATH);
        if (model == NULL) {
            return -1; // or some error value
        }
    }

    return predictRating(model, userId, itemId);
}

float calculate_error(int userId, int itemId){
    float predicted = get_recommendation(userId, itemId);
    float error = (5-predicted)/5;

    return error;
}

// Count the number of edges for a node
int countEdges(Node* node) {
    int count = 0;
    Edge* current = node->edges;
    while (current != NULL) {
        count++;
        current = current->nextEdge;
    }
    return count;
}

// Get a random edge from a node's edge list
Edge* getRandomEdge(Node* node) {
    int edgeCount = countEdges(node);
    if (edgeCount == 0) return NULL;

    // Generate random index
    int randomIndex = rand() % edgeCount;

    // Traverse to the randomly selected edge
    Edge* current = node->edges;
    for (int i = 0; i < randomIndex; i++) {
        current = current->nextEdge;
    }
    return current;
}

// Perform random walk starting from a given node
Node* randomWalk(Graph* graph, Node* startNode, int walkLength, float alpha) {
    if (startNode == NULL || graph == NULL) return NULL;
    
    Node* currentNode = startNode;
    Node* currentItem = NULL;
    
    // Always end on an item node after walkLength steps
    for (int step = 0; step < walkLength; step++) {
        // Random restart with probability alpha
        if ((float)rand() / RAND_MAX < alpha) {
            currentNode = startNode;
            continue;
        }
        
        Edge* randomEdge = getRandomEdge(currentNode);
        if (randomEdge == NULL) break;
        
        // If we're at a user node, move to an item
        if (currentNode == startNode || currentNode->id == randomEdge->userId) {
            currentItem = findNode(graph->items, randomEdge->itemId);
            currentNode = currentItem;
        }
        // If we're at an item node, move to a user
        else {
            Node* nextUser = findNode(graph->users, randomEdge->userId);
            currentNode = nextUser;
            // Don't update currentItem as we want to keep track of last item visited
        }
    }
    
    // If we end on a user or haven't moved, try one more step to get to an item
    if (currentNode == startNode || currentNode->id == startNode->id) {
        Edge* finalEdge = getRandomEdge(currentNode);
        if (finalEdge != NULL) {
            currentItem = findNode(graph->items, finalEdge->itemId);
        }
    }
    
    return currentItem;  // Return the last item visited
}

void recommendRandomWalk(Graph* graph, int userId){
    printf("Extra- Random Walk:\n");
    srand(time(NULL));
    int walkLength = 10; // length of random walk
    float alpha = 0.15;     // restart probabiltiy
    
    // Start random walk from a user node
    Node* startNode = findNode(graph->users, userId);
    Node* endNode = randomWalk(graph, startNode, walkLength, alpha);
    
    if (endNode != NULL) {
        printf("Random walk started at user %d, ended at item %d\n", userId, endNode->id);
    }
    printf("\n");
}
