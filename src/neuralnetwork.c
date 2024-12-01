#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "graph.h"
#include "neuralnetwork.h"

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
