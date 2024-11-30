#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include "recommendation.h"

// Checks if there is an edge between a user and an item
bool hasEdge(Node *user, int userId, int itemId) {
    Node *current = user;
    while (current) {
        if (current->id == userId) {
            Edge *edge = current->edges;
            while (edge) {
                if (edge->itemId == itemId) {
                    return true;
                }
                edge = edge->nextEdge;
            }
        }
        current = current->next;
    }
    return false;
}

void initializeNetwork(NeuralNetwork *nn) {
    srand(time(NULL));
    for (int i = 0; i < INPUT_NODES; i++) {
        for (int j = 0; j < HIDDEN_NODES; j++) {
            nn->weights_input_hidden[i][j] = (double)rand() / RAND_MAX - 0.5;
        }
    }
    for (int i = 0; i < HIDDEN_NODES; i++) {
        nn->biases_hidden[i] = (double)rand() / RAND_MAX - 0.5;
        for (int j = 0; j < OUTPUT_NODES; j++) {
            nn->weights_hidden_output[i][j] = (double)rand() / RAND_MAX - 0.5;
        }
    }
    for (int i = 0; i < OUTPUT_NODES; i++) {
        nn->biases_output[i] = (double)rand() / RAND_MAX - 0.5;
    }
}

double sigmoid(double x) {
    return 1 / (1 + exp(-x));
}

void forward(NeuralNetwork *nn, double inputs[INPUT_NODES], double *output) {
    double hidden_layer[HIDDEN_NODES];

    // Input to Hidden Layer
    for (int i = 0; i < HIDDEN_NODES; i++) {
        hidden_layer[i] = nn->biases_hidden[i];
        for (int j = 0; j < INPUT_NODES; j++) {
            hidden_layer[i] += inputs[j] * nn->weights_input_hidden[j][i];
        }
        hidden_layer[i] = sigmoid(hidden_layer[i]);
    }

    // Hidden to Output Layer
    *output = nn->biases_output[0];
    for (int i = 0; i < HIDDEN_NODES; i++) {
        *output += hidden_layer[i] * nn->weights_hidden_output[i][0];
    }
    *output = sigmoid(*output) * 5; // Scale to rating range
}

void train(NeuralNetwork *nn, double inputs[INPUT_NODES], double target, double learning_rate) {
    double hidden_layer[HIDDEN_NODES];
    double output;

    // Forward Pass
    forward(nn, inputs, &output);

    // Output Layer Error
    double output_error = target - output;

    // Hidden Layer Error
    double hidden_error[HIDDEN_NODES];
    for (int i = 0; i < HIDDEN_NODES; i++) {
        hidden_error[i] = output_error * nn->weights_hidden_output[i][0];
    }

    // Update Hidden to Output Weights
    for (int i = 0; i < HIDDEN_NODES; i++) {
        nn->weights_hidden_output[i][0] += learning_rate * output_error * sigmoid(hidden_layer[i]);
    }

    // Update Input to Hidden Weights
    for (int i = 0; i < INPUT_NODES; i++) {
        for (int j = 0; j < HIDDEN_NODES; j++) {
            nn->weights_input_hidden[i][j] += learning_rate * hidden_error[j] * inputs[i];
        }
    }

    // Update Biases
    nn->biases_output[0] += learning_rate * output_error;
    for (int i = 0; i < HIDDEN_NODES; i++) {
        nn->biases_hidden[i] += learning_rate * hidden_error[i];
    }
}

void trainNetwork(NeuralNetwork *nn, double dataset[][3], int dataset_size, double learning_rate, int epochs) {
    for (int e = 0; e < epochs; e++) {
        for (int i = 0; i < dataset_size; i++) {
            double inputs[INPUT_NODES] = {dataset[i][0], dataset[i][1]};
            double target = dataset[i][2];
            train(nn, inputs, target, learning_rate);
        }
    }
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

void b2recommendClosestMovies(Graph *graph, int userId, int n) {
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
        if (!rated) {  //!= idi == yaptim&& distances[item->id - 1] == INT_MAX
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

void c4recommendClosestMovies(Graph *graph, int userId, int n) {
    Node *user = findNode(graph->users, userId);
    if (!user) {
        printf("User with ID %d not found.\n", userId);
        return;
    }

    int itemCount = countNodes(graph->items);
    int *distances = malloc(itemCount * sizeof(int)); // Store distances for all items
    int *unratedMovies = malloc(itemCount * sizeof(int)); // Track unrated movies
    for (int i = 0; i < itemCount; i++) {
        distances[i] = INT_MAX; // Initialize distances as max value
        unratedMovies[i] = 1;  // Assume all movies are unrated initially
    }

    // Mark movies rated by the user as rated
    Edge *userEdge = user->edges;
    while (userEdge) {
        unratedMovies[userEdge->itemId - 1] = 0; // Set as rated
        userEdge = userEdge->nextEdge;
    }

    // Compute distances from the user to all items
    Node *item = graph->items;
    while (item) {
        if (unratedMovies[item->id - 1]) { // Only consider unrated movies
            int distance = 0;
            Edge *itemEdge = item->edges;

            // Sum distances based on connected users' ratings
            while (itemEdge) {
                distance += 6 - itemEdge->rating; // Invert the rating
                itemEdge = itemEdge->nextEdge;
            }
            distances[item->id - 1] = distance;
        }
        item = item->next;
    }

    // Select top N closest movies
    int (*topMovies)[2] = malloc(n * sizeof(int[2])); // Store movie ID and distance
    for (int i = 0; i < n; i++) {
        topMovies[i][0] = -1; // Movie ID
        topMovies[i][1] = INT_MAX; // Distance
    }

    for (int i = 0; i < itemCount; i++) {
        if (unratedMovies[i] && distances[i] != INT_MAX) {
            for (int j = 0; j < n; j++) {
                if (distances[i] < topMovies[j][1]) {
                    // Shift lower-priority movies down
                    for (int k = n - 1; k > j; k--) {
                        topMovies[k][0] = topMovies[k - 1][0];
                        topMovies[k][1] = topMovies[k - 1][1];
                    }

                    // Insert current movie
                    topMovies[j][0] = i + 1; // Movie ID
                    topMovies[j][1] = distances[i]; // Distance
                    break;
                }
            }
        }
    }

    // Print recommendations
    printf("Top %d closest movie recommendations for user %d:\n", n, userId);
    for (int i = 0; i < n; i++) {
        if (topMovies[i][0] != -1) {
            printf("  Movie ID %d with inverted distance %d\n",
                   topMovies[i][0], topMovies[i][1]);
        } else {
            printf("  No more recommendations available.\n");
        }
    }

    free(distances);
    free(unratedMovies);
    free(topMovies);
}
