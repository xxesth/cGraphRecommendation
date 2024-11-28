#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "graph.h"

// Initialize a new empty graph
Graph *createGraph() {
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    graph->users = NULL;
    graph->items = NULL;
    return graph;
}

// Create a new user or item node
Node *createNode(int id) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->id = id;
    newNode->edges = NULL;
    newNode->next = NULL;
    return newNode;
}

Node *findNode(Node *head, int id) {
    Node *current = head;
    while (current) {
        if (current->id == id) {
            return current;
        }
        current = current->next;
    }
    return NULL; // Return NULL if the node with the given ID is not found
}

int countNodes(Node *head) {
    int count = 0;
    while (head) {
        count++;
        head = head->next;
    }
    return count;
}

// Add a new user to the graph
void addUser(Graph *graph, int userId) {
    Node *newUser = createNode(userId);
    newUser->next = graph->users;
    graph->users = newUser;
    printf("Added user with ID: %d\n", userId);
}

// Add a new item to the graph
void addItem(Graph *graph, int itemId) {
    Node *newItem = createNode(itemId);
    newItem->next = graph->items;
    graph->items = newItem;
    printf("Added item with ID: %d\n", itemId);
}

// Add an edge from a user to an item with a given rating
void addEdge(Graph *graph, int userId, int itemId, int rating) {
    Node *user = graph->users;
    Node *item = graph->items;
    while (user && user->id != userId) {
        user = user->next;
    }
    if (!user) {
        printf("User ID %d not found!\n", userId);
        return;
    }
    while (item && item->id != itemId){
        item = item->next;
    }
    if(!item){
        printf("Item ID %d not found!\n", itemId);
        return;
    }

    Edge *newEdge = (Edge *)malloc(sizeof(Edge));
    Edge *newEdge2 = (Edge *)malloc(sizeof(Edge));
    newEdge->userId = userId;
    newEdge->itemId = itemId;
    newEdge->rating = rating;
    newEdge2->userId = userId;
    newEdge2->itemId = itemId;
    newEdge2->rating = rating;
    newEdge->nextEdge = user->edges;
    newEdge2->nextEdge = item->edges;
    user->edges = newEdge;
    item->edges = newEdge2; 

    printf("Added edge from user %d to item %d with rating %d\n", userId, itemId, rating);
}

// Calculate the degree of a user (number of edges)
int degreeOfVertex(Graph *graph, int userId) {
    Node *user = graph->users;
    while (user && user->id != userId) {
        user = user->next;
    }
    if (!user) {
        printf("User ID %d not found!\n", userId);
        return -1; // User not found
    }

    int degree = 0;
    Edge *edge = user->edges;
    while (edge) {
        degree++;
        edge = edge->nextEdge;
    }
    return degree;
}

// Remove edges for an item during item deletion
void removeEdgesForItem(Graph *graph, int itemId) {
    Node *user = graph->users;
    while (user) {
        Edge **currentEdge = &(user->edges);
        while (*currentEdge) {
            if ((*currentEdge)->itemId == itemId) {
                Edge *toDelete = *currentEdge;
                *currentEdge = (*currentEdge)->nextEdge; // Remove the edge
                free(toDelete);
            } else {
                currentEdge = &((*currentEdge)->nextEdge);
            }
        }
        user = user->next;
    }
}

// Delete an item from the graph
void deleteItem(Graph *graph, int itemId) {
    Node **current = &(graph->items);
    while (*current) {
        if ((*current)->id == itemId) {
            Node *toDelete = *current;
            *current = (*current)->next; // Remove from the list
            removeEdgesForItem(graph, itemId); // Remove edges associated with this item
            free(toDelete);
            printf("Deleted item with ID: %d\n", itemId);
            return;
        }
        current = &((*current)->next);
    }
    printf("Item ID %d not found!\n", itemId);
}

// Print graph for debugging
void printGraph(Graph *graph) {
    Node *user = graph->users;
    printf("\n\nUsers:\n\n");
    while (user) {
        printf("User ID: %d\n", user->id);
        Edge *edge = user->edges;
        while (edge) {
            printf("  -> Item ID: %d, Rating: %d\n", edge->itemId, edge->rating);
            edge = edge->nextEdge;
        }
        user = user->next;
    }

    Node *item = graph->items;
    printf("\n\nItems:\n\n");
    while(item){
        printf("Item ID: %d\n", item->id);
        Edge *edge2 = item->edges;
        while(edge2){
            printf("  -> User ID: %d, Rating: %d\n", edge2->userId, edge2->rating);
            edge2 = edge2->nextEdge;
        }
        item = item->next;
    }
  
    Node *best = graph->users;
    Node *test = graph->items;
    printf("\n\nTEST USERS\n\n");
    while(best){
       printf("%d ", best->id);
       best = best->next;
    }
    printf("\n\nTEST ITEMS\n\n");
    while(test){
        printf("%d ", test->id);
        test = test->next;
    }

}

void printEdges(Node *node){
    Edge *edge = node->edges;
    while(edge){
        printf("user/item/rating %d %d %d\n", edge->userId, edge->itemId, edge->rating);
        edge = edge->nextEdge;
    }
    printf("\n\n");
}

// Parse the u.data file to populate the graph
void parseDataFile(Graph *graph, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open file");
        return;
    }

    int userId, itemId, rating, timestamp;
    while (fscanf(file, "%d\t%d\t%d\t%d", &userId, &itemId, &rating, &timestamp) == 4) {
        // Add user if not already present
        Node *user = graph->users;
        bool userExists = false;
        while (user) {
            if (user->id == userId) {
                userExists = true;
                break;
            }
            user = user->next;
        }
        if (!userExists) {
            addUser(graph, userId);
        }

        // Add item if not already present
        Node *item = graph->items;
        bool itemExists = false;
        while (item) {
            if (item->id == itemId) {
                itemExists = true;
                break;
            }
            item = item->next;
        }
        if (!itemExists) {
            addItem(graph, itemId);
        }

        // Add edge for user and item
        addEdge(graph, userId, itemId, rating);
    }

    fclose(file);
}

// Function to free edges
void freeEdges(Edge *edge) {
    while (edge) {
        Edge *temp = edge;
        edge = edge->nextEdge;
        free(temp);
    }
}

// Function to free nodes (users and items)
void freeNodes(Node *node) {
    while (node) {
        Node *temp = node;
        node = node->next;
        free(temp);
    }
}

// Function to free the entire graph
void freeGraph(Graph *graph) {
    freeNodes(graph->users);
    freeNodes(graph->items);
//    freeEdges(graph->users->edges);
    free(graph); 
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
