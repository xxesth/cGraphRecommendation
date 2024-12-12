#include <stdio.h>
#include "graph.h"
#include "recommendation.h"
#include "neuralnetwork.h"

#define DATA_ADDRESS "./ml-100k/u.data"
//#define RECOMMEND_TO_USER 53
//#define RECOMMEND_FOR_ITEM 53
//#define NUMBER_OF_MOVIES 5
//define EPOCHS 100
#define MOSTLIKEDMOVIECOUNT 5
#define WALKLENGTH 10
#define ALPHA 0.15

int main() {
    Graph *graph = createGraph();
    parseDataFile(graph, DATA_ADDRESS); 
    //printGraph(graph);
    
    int RECOMMEND_TO_USER, RECOMMEND_FOR_ITEM, NUMBER_OF_MOVIES, EPOCHS; //MOSTLIKEDMOVIECOUNT;
    printf("Enter the ID of the user you want to generate recommendations for (1-943): ");
    scanf("%d", &RECOMMEND_TO_USER);
    printf("Enter the ID of the item which will be used for recommendation (1-1682): ");
    scanf("%d", &RECOMMEND_FOR_ITEM);
    printf("Enter the number of movies to be recommended (~5): ");
    scanf("%d", &NUMBER_OF_MOVIES);
    printf("Enter the number of training epochs (~100): ");
    scanf("%d", &EPOCHS);
    //printf("Enter the number of most liked movie count for item based recommendation (~5): ");
    //scanf("%d", &MOSTLIKEDMOVIECOUNT);
    
    recommendNeuralNetwork(graph, EPOCHS, RECOMMEND_TO_USER, RECOMMEND_FOR_ITEM, NUMBER_OF_MOVIES);
    recommendRandomMovie(graph, RECOMMEND_TO_USER);
    recommendTopRatedMovies(graph, RECOMMEND_TO_USER, NUMBER_OF_MOVIES);
    recommendBasedOnSimilarUser(graph, RECOMMEND_TO_USER, NUMBER_OF_MOVIES);
    recommendClosestMovies(graph, RECOMMEND_TO_USER, NUMBER_OF_MOVIES);
    recommendMovieBasedOnItem(graph, RECOMMEND_FOR_ITEM, MOSTLIKEDMOVIECOUNT);
    recommendRandomWalk(graph, RECOMMEND_TO_USER, WALKLENGTH, ALPHA);
    
    freeGraph(graph);
    return 0;
}
