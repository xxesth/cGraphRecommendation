#include <stdio.h>
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

    MatrixFactorization *model = initModel(graph);
    trainModel(model, graph, 100); // 100 epochs
    float pred = predictRating(model, 53, 53);
    printf("Predicted rating for User 53, Item 53: %.2f (New prediction)\n", pred);
    int recommendations[3];
    // 3 recommendation for user 53
    getTopNRecommendations(model, graph, 53, 3, recommendations);
    for (int i = 0; i < 3; i++) {
        printf("Recommended item %d: Item %d (Predicted rating: %.2f)\n",
               i + 1, recommendations[i],
               predictRating(model, 53, recommendations[i]));
    }
    freeModel(model);

    freeGraph(graph);

    return 0;
}
