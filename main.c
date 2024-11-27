#include <stdio.h>
#include "graph.h"

int main() {
    Graph *graph = createGraph();
    parseDataFile(graph, "./ml-100k/u.data"); // Replace with your actual data file path

    printGraph(graph);
    

    // Free allocated memory and clean up (not shown)
//    freeGraph(graph);

    return 0;
}

