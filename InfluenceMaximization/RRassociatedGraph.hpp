//
//  RRassociatedGraph.hpp
//  InfluenceMaximization
//
//  Created by Preeti on 4/18/18.
//  Copyright © 2018 Madhavan R.P. All rights reserved.
//

#ifndef RRassociatedGraph_hpp
#define RRassociatedGraph_hpp

#include <stdio.h>
#include <unordered_map>
#include <unordered_set>
#include "vertex.hpp"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <queue>
#include <ctime>
#include <deque>
#include <string.h>
#include <algorithm>

using namespace std;

class RRassociatedGraph{
    struct KeyHasher {
        std::size_t operator()(const vertex & k) const {
            return std::hash<int>()(k.id);
        }
    };
public:
    //unordered_set<vertex> vertices;
    unordered_map<int,vertex*,KeyHasher> vertexMap;
    
public:
    RRassociatedGraph();
    unordered_set<vertex> getVertices();
    int getNumberOfVertices();
    void addVertex(vertex v);
    vertex* find(int id);
    void addEdge(int from, int to, int label);
    void removeEdge(int from, int to);
};
#endif /* RRassociatedGraph_hpp */

