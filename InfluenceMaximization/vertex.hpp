//
//  vertex.hpp
//  InfluenceMaximization
//
//  Created by Preeti on 4/18/18.
//  Copyright © 2018 Madhavan R.P. All rights reserved.
//

#ifndef vertex_hpp
#define vertex_hpp

#include <stdio.h>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <set>
#include <utility>

//using namespace std;

class vertex{

public:
    int id;
    /*struct EqualFn
    {
    public:
        bool operator()(pair<vertex*,unordered_set<int>> const& t1, pair<vertex*,unordered_set<int>> const& t2) const
        { return (t1.first->id == t2.first->id);
        }
    };*/
    bool operator==(const vertex &obj) const
    {
        return id==obj.id;
    }
    
    int indDegree;
    int outDegree;
    std::unordered_map<int,vertex*>  inBoundNeighbours;
    std::unordered_map<vertex*,std::unordered_set<int>>  outBoundNeighbours;
    std::unordered_map<int,vertex*>  labels;
    //unordered_map<int,unordered_set<int>>labels;
    
public:
    
    vertex(int id);
    int getId();
    void setId(int id);
    int getIndDegree();
    void setIndDegree(int indDegree);
    int getOutDegree();
    void setOutDegree(int outDegree);
    std::unordered_map<int,vertex*> getInBoundNeighbours();
    void setInBoundNeighbours(std::unordered_map<int,vertex*> inBoundNeighbours);
    std::unordered_map<vertex*,std::unordered_set<int>> getOutBoundNeighbours();
    void setOutBoundNeighbours(std::unordered_map<vertex*,std::unordered_set<int>> outBoundNeighbours);
    void removeOutBoundNeighbour(vertex* toVertex);
    void removeInBoundNeighbour(vertex* fromVertex);
    void addInBoundNeighbour(vertex* v);
    void addOutBoundNeighbour(vertex *v, int label);
    void deleteOutBoundNeighbour();
    
    //unordered_map<int,unordered_set<int>> getLabels();
    //void setLabels(unordered_map<int,unordered_set<int>> labels);
    //unordered_set<int> getVertexLabel(vertex neighbour);
};

namespace std
{
    template<>
    struct hash<vertex>
    {
        size_t operator()(const vertex &obj) const
        {
            return hash<int>()(obj.id);
        }
    };
    
}
/*
    template<>
    struct hash<pair<vertex*,std::unordered_set<int>>>
    {
        std::size_t operator()(const pair<vertex*,std::unordered_set<int>>& k) const
        {
            using std::size_t;
            using std::hash;
            
            return hash<int>()(k.first->id);
        }
    };
}*/





#endif /* vertex_hpp */
