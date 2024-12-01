#include <stdio.h>
#include "graph.h"
#include "recommendation.h"
#include "neuralnetwork.h"

#define DATA_ADDRESS "./ml-100k/u.data"
#define RECOMMEND_TO_USER 53
#define NUMBER_OF_MOVIES 5

int main() {
    Graph *graph = createGraph();
    parseDataFile(graph, DATA_ADDRESS); 

    printGraph(graph);
    
    //Recommendations
    recommendRandomMovie(graph, RECOMMEND_TO_USER);
    recommendTopRatedMovies(graph, RECOMMEND_TO_USER, NUMBER_OF_MOVIES);
    recommendBasedOnSimilarUser(graph, RECOMMEND_TO_USER, NUMBER_OF_MOVIES);
    recommendClosestMovies(graph, RECOMMEND_TO_USER, NUMBER_OF_MOVIES);

    //recommendMovies(nn, RECOMMEND_TO_USER, NUMBER_OF_MOVIES, graph);

    freeGraph(graph);

    return 0;
}
