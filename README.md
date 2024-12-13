
# Graph Recommendation
A project focused on constructing a graph from raw data and developing a recommendation system leveraging the graph structure. The system combines traditional algorithms with neural network-based methods to provide recommendations.

Dataset: https://grouplens.org/datasets/movielens/

---
  
### to run:  
1. ```git clone https://github.com/xxesth/cGraphRecommendation.git```
2. ```cd cGraphRecommendation```
3. download [dataset](https://files.grouplens.org/datasets/movielens/ml-100k.zip), extract and move it to the project directory
4. ```make```
5. ```./build/run```
  
### to debug:  
1. ``gcc -g -o debug src/main.c src/graph.c src/recommendation.c src/neuralnetwork.c -lm``
2. ``gdb ./debug``
3. ``(gdb) run``
4. ``(gdb) backtrace`` or ``(gdb) print X``

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
|-- model
|   |-- trained_model.bin
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
1. Matrix factorization neural network is implemented for this project. GNN and NCF are other possible solutions.
2. Content based recommendation can be included as a further step of this project.
