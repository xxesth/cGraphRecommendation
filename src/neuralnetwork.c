#include "graph.h"
#include "neuralnetwork.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define LEARNING_RATE 0.01
#define REGULARIZATION 0.02
#define EMBEDDING_SIZE 10  // Number of latent features

// Initialize embedding vector
void initEmbedding(EmbeddingVector *vec) {
    vec->embedding = malloc(EMBEDDING_SIZE * sizeof(float));
    for (int i = 0; i < EMBEDDING_SIZE; i++) {
        vec->embedding[i] = ((float)rand() / RAND_MAX - 0.5f) * 0.1f;
    }
}

// Find maximum ID in a linked list of nodes
int findMaxId(Node *head) {
    int maxId = 0;
    Node *current = head;
    while (current != NULL) {
        if (current->id > maxId) {
            maxId = current->id;
        }
        current = current->next;
    }
    return maxId;
}

// Create ID to index mapping
void createIdMapping(Node *head, int *idMap, int maxId) {
    // Initialize all mappings to -1 (invalid)
    for (int i = 0; i <= maxId; i++) {
        idMap[i] = -1;
    }

    // Create mapping for existing IDs
    int index = 0;
    Node *current = head;
    while (current != NULL) {
        idMap[current->id] = index;
        index++;
        current = current->next;
    }
}

// Initialize the model
MatrixFactorization* initModel(Graph *graph) {
    MatrixFactorization *model = malloc(sizeof(MatrixFactorization));

    // Find maximum IDs
    model->maxUserId = findMaxId(graph->users);
    model->maxItemId = findMaxId(graph->items);

    // Allocate and initialize ID mapping arrays
    model->userIdMap = malloc((model->maxUserId + 1) * sizeof(int));
    model->itemIdMap = malloc((model->maxItemId + 1) * sizeof(int));

    createIdMapping(graph->users, model->userIdMap, model->maxUserId);
    createIdMapping(graph->items, model->itemIdMap, model->maxItemId);

    model->numUsers = countNodes(graph->users);
    model->numItems = countNodes(graph->items);

    // Allocate arrays for embeddings
    model->userEmbeddings = malloc(model->numUsers * sizeof(EmbeddingVector));
    model->itemEmbeddings = malloc(model->numItems * sizeof(EmbeddingVector));

    // Initialize all embeddings
    for (int i = 0; i < model->numUsers; i++) {
        initEmbedding(&model->userEmbeddings[i]);
    }
    for (int i = 0; i < model->numItems; i++) {
        initEmbedding(&model->itemEmbeddings[i]);
    }

    return model;
}

float dotProduct(float *vec1, float *vec2) {
    float sum = 0;
    for (int i = 0; i < EMBEDDING_SIZE; i++) {
        sum += vec1[i] * vec2[i];
    }
    return sum;
}

// Get array index for a user ID
int getUserIndex(MatrixFactorization *model, int userId) {
    if (userId > model->maxUserId || model->userIdMap[userId] == -1) {
        printf("Error: Invalid user ID %d\n", userId);
        exit(1);
    }
    return model->userIdMap[userId];
}

// Get array index for an item ID
int getItemIndex(MatrixFactorization *model, int itemId) {
    if (itemId > model->maxItemId || model->itemIdMap[itemId] == -1) {
        printf("Error: Invalid item ID %d\n", itemId);
        exit(1);
    }
    return model->itemIdMap[itemId];
}

float predictRating(MatrixFactorization *model, int userId, int itemId) {
    int userIndex = getUserIndex(model, userId);
    int itemIndex = getItemIndex(model, itemId);
    
    float pred = dotProduct(
        model->userEmbeddings[userIndex].embedding,
        model->itemEmbeddings[itemIndex].embedding
    );
    
    if (pred < 1.0f) pred = 1.0f;
    if (pred > 5.0f) pred = 5.0f;
    
    return pred;
}

void trainOnExample(MatrixFactorization *model, int userId, int itemId, 
                   float actualRating) {
    int userIndex = getUserIndex(model, userId);
    int itemIndex = getItemIndex(model, itemId);
    
    float *userEmb = model->userEmbeddings[userIndex].embedding;
    float *itemEmb = model->itemEmbeddings[itemIndex].embedding;
    
    float pred = predictRating(model, userId, itemId);
    float error = actualRating - pred;
    
    for (int f = 0; f < EMBEDDING_SIZE; f++) {
        float userGrad = error * itemEmb[f] - REGULARIZATION * userEmb[f];
        float itemGrad = error * userEmb[f] - REGULARIZATION * itemEmb[f];
        
        userEmb[f] += LEARNING_RATE * userGrad;
        itemEmb[f] += LEARNING_RATE * itemGrad;
    }
}

// Train model on entire graph
void trainModel(MatrixFactorization *model, Graph *graph, int numEpochs) {
    for (int epoch = 0; epoch < numEpochs; epoch++) {
        Node *user = graph->users;
        while (user != NULL) {
            Edge *edge = user->edges;
            while (edge != NULL) {
                trainOnExample(model, user->id, edge->itemId, edge->rating);
                edge = edge->nextEdge;
            }
            user = user->next;
        }
    }
}

// Get top N recommendations for a user
void getTopNRecommendations(MatrixFactorization *model, Graph *graph,
                          int userId, int N, int *recommendedItems) {
    // Create array of all predictions
    typedef struct {
        int itemId;
        float prediction;
    } ItemPrediction;
    
    ItemPrediction *predictions = malloc(model->numItems * sizeof(ItemPrediction));
    int numPredictions = 0;
    
    // Get predictions for all items user hasn't rated
    Node *item = graph->items;
    while (item != NULL) {
        Node *user = findNode(graph->users, userId);
        if (!hasEdge(user, item->id)) {
            predictions[numPredictions].itemId = item->id;
            predictions[numPredictions].prediction = 
                predictRating(model, userId, item->id);
            numPredictions++;
        }
        item = item->next;
    }
    
    // Sort predictions (simple bubble sort for now)
    for (int i = 0; i < numPredictions - 1; i++) {
        for (int j = 0; j < numPredictions - i - 1; j++) {
            if (predictions[j].prediction < predictions[j + 1].prediction) {
                ItemPrediction temp = predictions[j];
                predictions[j] = predictions[j + 1];
                predictions[j + 1] = temp;
            }
        }
    }
    
    // Copy top N items to output array
    for (int i = 0; i < N && i < numPredictions; i++) {
        recommendedItems[i] = predictions[i].itemId;
    }
    
    free(predictions);
}

void freeModel(MatrixFactorization *model) {
    for (int i = 0; i < model->numUsers; i++) {
        free(model->userEmbeddings[i].embedding);
    }
    for (int i = 0; i < model->numItems; i++) {
        free(model->itemEmbeddings[i].embedding);
    }
    free(model->userEmbeddings);
    free(model->itemEmbeddings);
    free(model->userIdMap);
    free(model->itemIdMap);
    free(model);
}
