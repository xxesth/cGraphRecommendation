#include <stdio.h>
#include "graph.h"

#define DATA_ADDRESS "./ml-100k/u.data"
#define RECOMMEND_TO_USER 53
#define NUMBER_OF_MOVIES 5

int main() {
    Graph *graph = createGraph();
    parseDataFile(graph, DATA_ADDRESS); // Replace with your actual data file path

    printGraph(graph);
    
    //TEST
    //Node *user = findNode(graph->users, RECOMMEND_TO_USER);
    //printEdges(user);
    
    //Recommendations
    recommendRandomMovie(graph, RECOMMEND_TO_USER);
    recommendTopRatedMovies(graph, RECOMMEND_TO_USER, NUMBER_OF_MOVIES);
    recommendBasedOnSimilarUser(graph, RECOMMEND_TO_USER, NUMBER_OF_MOVIES);
    recommendClosestMovies(graph, RECOMMEND_TO_USER, NUMBER_OF_MOVIES);

    freeGraph(graph);

    return 0;
}
