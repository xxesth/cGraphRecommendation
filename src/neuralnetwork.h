#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

typedef struct NeuralNetwork {
    int inputSize;       // Number of input features
    int hiddenSize;      // Number of neurons in the hidden layer
    int outputSize;      // Output size (single value for predicted rating)
    double **weights1;   // Weights from input to hidden layer
    double *bias1;       // Bias for the hidden layer
    double **weights2;   // Weights from hidden to output layer
    double bias2;        // Bias for the output layer
    double learningRate; // Learning rate for training
} NeuralNetwork;

// Function declarations
NeuralNetwork *createNeuralNetwork();
void trainNeuralNetwork(NeuralNetwork *nn, double **inputs, double *targets, int dataSize);
double predict(NeuralNetwork *nn, double *input);
void freeNeuralNetwork(NeuralNetwork *nn);

#endif // NEURALNETWORK_H
