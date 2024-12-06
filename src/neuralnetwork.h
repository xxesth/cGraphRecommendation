#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

typedef struct {
    float *embedding;  // Array of latent features
} EmbeddingVector;

typedef struct {
    EmbeddingVector *userEmbeddings;  // Array of user embedding vectors
    EmbeddingVector *itemEmbeddings;  // Array of item embedding vectors
    int *userIdMap;     // Maps user IDs to array indices
    int *itemIdMap;     // Maps item IDs to array indices
    int numUsers;
    int numItems;
    int maxUserId;      // Highest user ID in the graph
    int maxItemId;      // Highest item ID in the graph
} MatrixFactorization;

void initEmbedding(EmbeddingVector *vec);
int findMaxId(Node *head);
void createIdMapping(Node *head, int *idMap, int maxId);
MatrixFactorization* initModel(Graph *graph);
float dotProduct(float *vec1, float *vec2);
int getUserIndex(MatrixFactorization *model, int userId);
int getItemIndex(MatrixFactorization *model, int itemId);
float predictRating(MatrixFactorization *model, int userId, int itemId);
void trainOnExample(MatrixFactorization *model, int userId, int itemId, float actualRating);
void trainModel(MatrixFactorization *model, Graph *graph, int numEpochs);
void getTopNRecommendations(MatrixFactorization *model, Graph *graph, int userId, int N, int *recommendedItems);
void freeModel(MatrixFactorization *model);

#endif
