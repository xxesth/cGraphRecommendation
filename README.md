
# Graph Recommendation
A project focused on constructing a graph from raw data and developing a recommendation system leveraging the graph structure. The system combines traditional algorithms with neural network-based methods to provide recommendations.

---
  
### to run:  
1. ```git clone https://github.com/xxesth/cGraphRecommendation.git```
2. ```cd cGraphRecommendation```
3. ```make```
4. ```./build/run```
  
### to debug:  
1. ``gcc -g -o debug src/main.c src/graph.c src/recommendation.c -lm``
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
  
4. soru algoritma guncellemesi  
5. soru noral ag gelistir
