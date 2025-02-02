#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int hasEdge(Node *node, int itemId) {
    // Traverse the edge list of the node (which is a user or item)
    Edge *currentEdge = node->edges;
    
    while (currentEdge != NULL) {
        // Check if the current edge's itemId matches the given itemId
        if (currentEdge->itemId == itemId) {
            return 1;  // Edge exists, meaning the user has rated the item
        }
        currentEdge = currentEdge->nextEdge;  // Move to the next edge
    }

    return 0;  // No edge found, meaning the user has not rated the item
}

// Add a new user to the graph
void addUser(Graph *graph, int userId) {
    Node *newUser = createNode(userId);
    newUser->next = graph->users;
    graph->users = newUser;
    //printf("Added user with ID: %d\n", userId);
}

// Add a new item to the graph
void addItem(Graph *graph, int itemId) {
    Node *newItem = createNode(itemId);
    newItem->next = graph->items;
    graph->items = newItem;
    //printf("Added item with ID: %d\n", itemId);
}

// Add an edge from a user to an item with a given rating
void addEdge(Graph *graph, int userId, int itemId, int rating) {
    Node *user = graph->users;
    Node *item = graph->items;
    while (user && user->id != userId) {
        user = user->next;
    }
    if (!user) {
        //printf("User ID %d not found!\n", userId);
        return;
    }
    while (item && item->id != itemId){
        item = item->next;
    }
    if(!item){
        //printf("Item ID %d not found!\n", itemId);
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

    //printf("Added edge from user %d to item %d with rating %d\n", userId, itemId, rating);
}

// Calculate the degree of a user (number of edges)
int degreeOfVertex(Graph *graph, int userId) {
    Node *user = graph->users;
    while (user && user->id != userId) {
        user = user->next;
    }
    if (!user){
        //printf("User ID %d not found!\n", userId);
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
  
    //Node *best = graph->users;
    //Node *test = graph->items;
    //printf("\n\nTEST USERS\n\n");
    //while(best){
    //   printf("%d ", best->id);
    //   best = best->next;
    //}
    //printf("\n\nTEST ITEMS\n\n");
    //while(test){
    //    printf("%d ", test->id);
    //    test = test->next;
    //}

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
