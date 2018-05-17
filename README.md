# Disrupting Diffusion: Critical Nodes in Network

We formulate and study the problem of identifying nodes whose absence can maximally disrupt network-diffusion under independent cascade model. We refer to such nodes as critical nodes. We present the notion of impact and characterize critical nodes based on this notion. Informally, impact of a set of nodes quantifies the necessity of the nodes in the diffusion process. We prove that the impact is monotonic. Interestingly, unlike similar formulation of critical edges in the context of Linear Threshold diffusion model, impact is neither submodular nor supermodular. Hence, we develop heuristics that rely on greedy strategy and modular or submodular approximations of impact function. We empirically evaluate our heuristics by comparing the level of disruption achieved by identifying and removing critical nodes as opposed to that acheived by removing the most influential nodes.

## How to Compile
We recommend using GCC 4.9 and greater.

Set the compiler path for CXX in Makefile
Make the influence target:
make influence
This will make an executable named "influence".


Store the graph file/labels file in the graphs folder. The graph file has the following format:
```
	First line: <number of node> <number of edges>
	From second line: <from node> <to node>
```
Set the following Parameters:

algorithm - "timtim"
fullgraph - true (false if passed only the influenced graph)
seedset - 0 (selecting best seed to get the influenced graph)
Diffusion - 0 (Algorithm of one simulation to get the influenced graph)
percentage - The percentage of targets. 100 i.e. all nodes are Targets
budget - Set the seed set size
nodesRemove - nodes to be removed (the critical nodes)
newSeedset -  new Seed for influence check is calculated using the best seed nodes for all three approaches after removal of critical nodes.

Example command:
```
./influence --algorithm timtim --fullgraph true --Diffusion 0 --newSeedset 5 --modularity modular2 --graph ca-GrQc-processed.txt --seedset 0 --budget 10 --percentage 100 --threshold 10 --nodesRemove 5 
```
## Download Graph Data
The graphs that we have used, along with labels file can be found in "graphs" folder of the code. If you want to use another graph file then place the file in graphs folder before running the program.

## Results
Results file get created for every execution and can be found in results folder (if budget, graph name, nodeRemove are same then the file in results folder will get appended instead of generating new file). Log file will also get created and can be found in ResultsData folder
