#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "neuralnetwork.h"

#define INPUT_SIZE 3
#define HIDDEN_SIZE 5
#define OUTPUT_SIZE 1
#define LEARNING_RATE 0.1
#define EPOCHS 1000

// Helper function: Random initialization
double randomWeight() {
    return ((double)rand() / RAND_MAX) * 2 - 1; // Random value in [-1, 1]
}

// Helper function: Sigmoid activation
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

// Helper function: Derivative of sigmoid
double sigmoidDerivative(double x) {
    return x * (1.0 - x);
}

// Create and initialize a neural network
NeuralNetwork *createNeuralNetwork(){
    NeuralNetwork *nn = malloc(sizeof(NeuralNetwork));
    nn->inputSize = INPUT_SIZE;
    nn->hiddenSize = HIDDEN_SIZE;
    nn->outputSize = OUTPUT_SIZE;
    nn->learningRate = LEARNING_RATE;

    // Allocate weights and biases
    nn->weights1 = malloc(nn->inputSize * sizeof(double *));
    for (int i = 0; i < nn->inputSize; i++) {
        nn->weights1[i] = malloc(nn->hiddenSize * sizeof(double));
        for (int j = 0; j < nn->hiddenSize; j++) {
            nn->weights1[i][j] = randomWeight();
        }
    }
    nn->bias1 = malloc(nn->hiddenSize * sizeof(double));
    for (int i = 0; i < nn->hiddenSize; i++) {
        nn->bias1[i] = randomWeight();
    }

    nn->weights2 = malloc(nn->hiddenSize * sizeof(double *));
    for (int i = 0; i < nn->hiddenSize; i++) {
        nn->weights2[i] = malloc(nn->outputSize * sizeof(double));
        for (int j = 0; j < nn->outputSize; j++) {
            nn->weights2[i][j] = randomWeight();
        }
    }
    nn->bias2 = randomWeight();

    return nn;
}

// Train the neural network
void trainNeuralNetwork(NeuralNetwork *nn, double **inputs, double *targets, int dataSize){
    const int epochs = EPOCHS;
    double *hiddenLayer = malloc(nn->hiddenSize * sizeof(double));
    double output;

    for (int epoch = 0; epoch < epochs; epoch++) {
        for (int i = 0; i < dataSize; i++) {
            // Forward pass
            for (int j = 0; j < nn->hiddenSize; j++) {
                hiddenLayer[j] = nn->bias1[j];
                for (int k = 0; k < nn->inputSize; k++) {
                    hiddenLayer[j] += inputs[i][k] * nn->weights1[k][j];
                }
                hiddenLayer[j] = sigmoid(hiddenLayer[j]);
            }

            output = nn->bias2;
            for (int j = 0; j < nn->hiddenSize; j++) {
                hiddenLayer[j] = nn->bias1[j];
                for (int k = 0; k < nn->inputSize; k++) {
                    // Debug statements
                    if (inputs[i] == NULL) {
                        printf("Inputs row %d is NULL\n", i);
                        return; // Exit on NULL
                    }
                    hiddenLayer[j] += inputs[i][k] * nn->weights1[k][j];
                }
                hiddenLayer[j] = sigmoid(hiddenLayer[j]);
                output = hiddenLayer[j];
            }

            // Calculate error
            double error = targets[i] - output;

            // Backpropagation
            double outputGradient = error * sigmoidDerivative(output);
            for (int j = 0; j < nn->hiddenSize; j++) {
                double hiddenGradient = outputGradient * nn->weights2[j][0] * sigmoidDerivative(hiddenLayer[j]);
                nn->weights2[j][0] += nn->learningRate * outputGradient * hiddenLayer[j];
                nn->bias1[j] += nn->learningRate * hiddenGradient;

                for (int k = 0; k < nn->inputSize; k++) {
                    nn->weights1[k][j] += nn->learningRate * hiddenGradient * inputs[i][k];
                }
            }
            nn->bias2 += nn->learningRate * outputGradient;
        }
    }

    free(hiddenLayer);
}

// Predict using the neural network
double predict(NeuralNetwork *nn, double *input) {
    double *hiddenLayer = malloc(nn->hiddenSize * sizeof(double));
    double output;

    // Forward pass
    for (int j = 0; j < nn->hiddenSize; j++) {
        hiddenLayer[j] = nn->bias1[j];
        for (int k = 0; k < nn->inputSize; k++) {
            hiddenLayer[j] += input[k] * nn->weights1[k][j];
        }
        hiddenLayer[j] = sigmoid(hiddenLayer[j]);
    }

    output = nn->bias2;
    for (int j = 0; j < nn->hiddenSize; j++) {
        output += hiddenLayer[j] * nn->weights2[j][0];
    }
    output = sigmoid(output);

    free(hiddenLayer);
    return output;
}

// Free memory allocated for the neural network
void freeNeuralNetwork(NeuralNetwork *nn) {
    for (int i = 0; i < nn->inputSize; i++) {
        free(nn->weights1[i]);
    }
    free(nn->weights1);
    free(nn->bias1);

    for (int i = 0; i < nn->hiddenSize; i++) {
        free(nn->weights2[i]);
    }
    free(nn->weights2);

    free(nn);
}
