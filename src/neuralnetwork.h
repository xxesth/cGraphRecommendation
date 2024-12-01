#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#define INPUT_NODES 3   // userId, itemId, rating
#define HIDDEN_NODES 128
#define OUTPUT_NODES 1  // Predicted rating

typedef struct {
    double weights_input_hidden[INPUT_NODES][HIDDEN_NODES];
    double weights_hidden_output[HIDDEN_NODES][OUTPUT_NODES];
    double biases_hidden[HIDDEN_NODES];
    double biases_output[OUTPUT_NODES];
} NeuralNetwork;

bool hasEdge(Node *user, int userId, int itemId);
void initializeNetwork(NeuralNetwork *nn);
double sigmoid(double x);
void forward(NeuralNetwork *nn, double inputs[INPUT_NODES], double *output);
void train(NeuralNetwork *nn, double inputs[INPUT_NODES], double target, double learning_rate);
void trainNetwork(NeuralNetwork *nn, double dataset[][3], int dataset_size, double learning_rate, int epochs);

#endif // NEURALNETWORK_H