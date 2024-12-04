
# Graph Recommendation
A project focused on constructing a graph from raw data and developing a recommendation system leveraging the graph structure. The system combines traditional algorithms with neural network-based methods to provide recommendations.

---
  
### to run:  
1. ```git clone https://github.com/xxesth/cGraphRecommendation.git```
2. ```cd cGraphRecommendation```
3. ```make```
4. ```./build/run```
  
### to debug:  
1. ``gcc -g -o debug src/main.c src/graph.c src/recommendation.c src/neuralnetwork.c -lm``
2. ``gdb ./debug``
3. ``(gdb) run``
4. ``(gdb) backtrace``

---
```bash
.
|-- LICENSE
|-- Makefile 
|-- README.md  
|-- .gitignore
|-- projectdescription.pdf # project requirements 
|-- ml-100k # data
|   |-- u.data  # main dataset
|-- src  
|   |-- graph.c  
|   |-- graph.h  
|   |-- main.c  
|   |-- neuralnetwork.c  
|   |-- neuralnetwork.h  
|   |-- recommendation.c  
|   |-- recommendation.h
```
---
**notes:**  

neural network part is just a demo for several reasons:
1. ids are in a hiearchieal order, which requires vectorizing so computing capacity.
2. for a better performance, content based filtering is also required, which is not included in u.data
3. neural networks are not actually a direct subject of this project.
