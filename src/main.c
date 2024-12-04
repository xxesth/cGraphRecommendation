#include "graph.h"
#include "recommendation.h"
#include "neuralnetwork.h"

#define DATA_ADDRESS "./ml-100k/u.data"
#define RECOMMEND_TO_USER 53
#define RECOMMEND_FOR_ITEM 53
#define NUMBER_OF_MOVIES 5

int main() {
    Graph *graph = createGraph();
    parseDataFile(graph, DATA_ADDRESS); 

    //printGraph(graph);
    
    //Recommendations
    recommendRandomMovie(graph, RECOMMEND_TO_USER);
    recommendTopRatedMovies(graph, RECOMMEND_TO_USER, NUMBER_OF_MOVIES);
    recommendBasedOnSimilarUser(graph, RECOMMEND_TO_USER, NUMBER_OF_MOVIES);
    recommendClosestMovies(graph, RECOMMEND_TO_USER, NUMBER_OF_MOVIES);
    recommendMovieBasedOnItem(graph, RECOMMEND_FOR_ITEM);
    recommendNeuralNetwork(graph, RECOMMEND_TO_USER, NUMBER_OF_MOVIES);

    freeGraph(graph);

    return 0;
}
