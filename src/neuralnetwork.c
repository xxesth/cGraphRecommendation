#include "graph.h"
#include "neuralnetwork.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define LEARNING_RATE 0.01
#define REGULARIZATION 0.02
#define EMBEDDING_SIZE 10  // Number of latent features
#define MODEL_PATH "./model/trained_model.bin"

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

void recommendNeuralNetwork(Graph *graph, int epochs, int userId, int itemId, int n){
    printf("Bonus: Recommendation with neural network:\n");
    Node *user = findNode(graph->users, userId);
    if (user == NULL){
        printf("User %d not found in the graph.\n", userId);
        return;
    }

    MatrixFactorization *model = initModel(graph);
    trainModel(model, graph, epochs);
    if (hasEdge(user, itemId)){
        printf("User %d has already rated movie %d.\n", userId, itemId);
    }else{
        float pred = predictRating(model, userId, itemId);
        printf("Predicted rating for User %d - Item %d: %.2f (New prediction)\n", userId, itemId, pred);
    }
    printf("Recommendations for user %d:\n", userId);
    int recommendations[n];
    getTopNRecommendations(model, graph, userId, n, recommendations);
    for (int i = 0; i < n; i++) {
        printf("Recommended item %d: Item %d (Predicted rating: %.2f)\n",
               i + 1, recommendations[i],
               predictRating(model, userId, recommendations[i]));
    }
    printf("\n");
    saveModel(model, MODEL_PATH);
    freeModel(model);
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

void saveModel(MatrixFactorization *model, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Error opening file for writing\n");
        return;
    }

    // Save model parameters
    fwrite(&model->numUsers, sizeof(int), 1, file);
    fwrite(&model->numItems, sizeof(int), 1, file);
    fwrite(&model->maxUserId, sizeof(int), 1, file);
    fwrite(&model->maxItemId, sizeof(int), 1, file);
    int embeddingSize = EMBEDDING_SIZE;
    fwrite(&embeddingSize, sizeof(int), 1, file);

    // Save ID mappings
    fwrite(model->userIdMap, sizeof(int), model->maxUserId + 1, file);
    fwrite(model->itemIdMap, sizeof(int), model->maxItemId + 1, file);

    // Save user embeddings
    for (int i = 0; i < model->numUsers; i++) {
        fwrite(model->userEmbeddings[i].embedding, sizeof(float), EMBEDDING_SIZE, file);
    }

    // Save item embeddings
    for (int i = 0; i < model->numItems; i++) {
        fwrite(model->itemEmbeddings[i].embedding, sizeof(float), EMBEDDING_SIZE, file);
    }

    fclose(file);
}

MatrixFactorization* loadModel(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening file for reading\n");
        return NULL;
    }

    MatrixFactorization *model = malloc(sizeof(MatrixFactorization));
    int savedEmbeddingSize;

    // Load model parameters
    fread(&model->numUsers, sizeof(int), 1, file);
    fread(&model->numItems, sizeof(int), 1, file);
    fread(&model->maxUserId, sizeof(int), 1, file);
    fread(&model->maxItemId, sizeof(int), 1, file);
    fread(&savedEmbeddingSize, sizeof(int), 1, file);

    if (savedEmbeddingSize != EMBEDDING_SIZE) {
        printf("Error: Saved model has different embedding size\n");
        free(model);
        fclose(file);
        return NULL;
    }

    // Allocate and load ID mappings
    model->userIdMap = malloc((model->maxUserId + 1) * sizeof(int));
    model->itemIdMap = malloc((model->maxItemId + 1) * sizeof(int));
    fread(model->userIdMap, sizeof(int), model->maxUserId + 1, file);
    fread(model->itemIdMap, sizeof(int), model->maxItemId + 1, file);

    // Allocate and load user embeddings
    model->userEmbeddings = malloc(model->numUsers * sizeof(EmbeddingVector));
    for (int i = 0; i < model->numUsers; i++) {
        model->userEmbeddings[i].embedding = malloc(EMBEDDING_SIZE * sizeof(float));
        fread(model->userEmbeddings[i].embedding, sizeof(float), EMBEDDING_SIZE, file);
    }

    // Allocate and load item embeddings
    model->itemEmbeddings = malloc(model->numItems * sizeof(EmbeddingVector));
    for (int i = 0; i < model->numItems; i++) {
        model->itemEmbeddings[i].embedding = malloc(EMBEDDING_SIZE * sizeof(float));
        fread(model->itemEmbeddings[i].embedding, sizeof(float), EMBEDDING_SIZE, file);
    }

    fclose(file);
    return model;
}
