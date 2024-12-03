#include <stdlib.h>
#include <stdio.h>
#include "graph.h"
#include "recommendation.h"
#include "neuralnetwork.h"

#define DATA_ADDRESS "./ml-100k/u.data"
#define RECOMMEND_TO_USER 53
#define NUMBER_OF_MOVIES 5

#define EPOCHS 1000
#define LEARNING_RATE 0.1

int main() {
    Graph *graph = createGraph();
    parseDataFile(graph, DATA_ADDRESS); 

    printGraph(graph);
    
    //Recommendations
    recommendRandomMovie(graph, RECOMMEND_TO_USER);
    recommendTopRatedMovies(graph, RECOMMEND_TO_USER, NUMBER_OF_MOVIES);
    recommendBasedOnSimilarUser(graph, RECOMMEND_TO_USER, NUMBER_OF_MOVIES);
    recommendClosestMovies(graph, RECOMMEND_TO_USER, NUMBER_OF_MOVIES);

    // Training the neural network
    NeuralNetwork *nn = createNeuralNetwork(3, 5, 1, LEARNING_RATE); // 3 inputs, 5 hidden nodes, 1 output
    // Prepare training data (dummy example for now)
    double targets[2] = {0.8, 0.9}; // Normalized ratings
    double **inputs = malloc(2 * sizeof(double *));
    for (int i = 0; i < 2; i++) {
        inputs[i] = malloc(3 * sizeof(double)); // Allocate space for each feature
        if (inputs[i] == NULL) {
            fprintf(stderr, "Failed to allocate memory for inputs[%d]\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Populate inputs
    inputs[0][0] = 53; inputs[0][1] = 10; inputs[0][2] = 3; // Example: User ID, Item ID, Rating
    inputs[1][0] = 53; inputs[1][1] = 15; inputs[1][2] = 4;
    
    trainNeuralNetwork(nn, (double **)inputs, targets, 2, EPOCHS);

    // Predict a rating
    double testInput[3] = {53, 12, 4}; // User ID, Item ID, and Rating
    double prediction = predict(nn, testInput);
    printf("Predicted Rating: %.2f\n", prediction * 5);

    // Clean up
    freeNeuralNetwork(nn);
    freeGraph(graph);

    return 0;
}
