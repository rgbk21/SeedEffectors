//
//  RRassociatedGraph.cpp
//  InfluenceMaximization
//
//  Created by Preeti on 4/18/18.
//  Copyright © 2018 Madhavan R.P. All rights reserved.
//

#include "RRassociatedGraph.hpp"
RRassociatedGraph::RRassociatedGraph() {
    //vertices = unordered_set<vertex>();
}
/*
unordered_set<vertex> RRassociatedGraph::getVertices() {
    return vertices;
}

int RRassociatedGraph::getNumberOfVertices() {
    return (int)this->vertices.size();
}
*/
void RRassociatedGraph::addVertex(vertex v) {
    //this->vertices.insert(v);
    vertexMap.insert(pair<int,vertex*>(v.getId(), &v));
}

vertex* RRassociatedGraph::find(int id) {
    unordered_map<int,vertex*>::const_iterator got=vertexMap.find(id);
    if(got != vertexMap.end() )
        return got->second;
    return nullptr;
}

void RRassociatedGraph::addEdge(int from, int to, int label) {
    vertex* fromVertex = find(from);
    vertex* toVertex = find(to);
    if (fromVertex == nullptr) {
        fromVertex = new vertex(from);
        vertexMap.insert(pair<int,vertex*>(fromVertex->getId(), fromVertex));
    }
    if (toVertex == nullptr) {
        toVertex = new vertex(to);
        vertexMap.insert(pair<int,vertex*>(toVertex->getId(), toVertex));
    }
    fromVertex->addOutBoundNeighbour(toVertex, label);
    toVertex->addInBoundNeighbour(fromVertex);
    // noOfEdges++;
}



void RRassociatedGraph::removeEdge(int from, int to) {
    vertex* fromVertex = find(from);
    vertex* toVertex = find(to);
    fromVertex->removeOutBoundNeighbour(toVertex);
    //to->removeInBoundNeighbour(from);
    
}
/*
 void RRassociatedGraph::print() {
 System.out.print("Vertices of the graph : ");
 StringBuilder vertexString = new StringBuilder("");
 StringBuilder edgeString = new StringBuilder("");
 for (Vertex vertex : vertices) {
 vertexString.append(vertex.getId() + ",");
 edgeString.append("Edges for the vertex " + vertex.getId() + " are : ");
 for (Vertex neighbour : vertex.getOutBoundNeighbours()) {
 edgeString.append(neighbour.getId() + ",");
 }
 edgeString.deleteCharAt(edgeString.length() - 1);
 edgeString.append("\n");
 }
 
 System.out.println(vertexString.deleteCharAt(vertexString.length() - 1));
 System.out.println(edgeString);
 }*/

