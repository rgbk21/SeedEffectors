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
    noOfEdges=0;
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

Edge* RRassociatedGraph::findedge(string id){
    unordered_map<string,Edge*>::const_iterator got=EdgeMap.find(id);
    if(got != EdgeMap.end() )
        return got->second;
    return nullptr;
}

void RRassociatedGraph::addEdge(int from, int to, int label) {
    
    vertex* fromVertex = find(from);
    if (fromVertex == nullptr) {
        fromVertex = new vertex(from);
        vertexMap.insert(pair<int,vertex*>(fromVertex->getId(), fromVertex));
    }
    
    vertex* toVertex = find(to);
    if (toVertex == nullptr) {
        toVertex = new vertex(to);
        vertexMap.insert(pair<int,vertex*>(toVertex->getId(), toVertex));
    }
    
    if(fromVertex==toVertex){
       // fromVertex->outDegree++;
        //cout<<fromVertex->getId()<<" "<<toVertex->getId()<<"\n";
        return;
    }
    string eid=std::to_string(from);
    eid+=std::to_string(to);
    Edge* edge=findedge(eid);
    if(edge==nullptr){
        edge=new Edge(eid,from,to);
        edge->addRRid(label);
        fromVertex->addOutGoingEdges(edge);
        toVertex->addInComingEdges(edge);
        EdgeMap.insert(pair<string,Edge*>(edge->getId(), edge));
        noOfEdges++;
    }
    else{
        edge->addRRid(label);
        fromVertex->outDegree++;
        toVertex->indDegree++;
    }
    
}



void RRassociatedGraph::removeEdge(int from, int to,int rrSetID) {
    vertex* fromVertex = find(from);
    vertex* toVertex = find(to);
    string eid=std::to_string(from);
    eid+=std::to_string(to);
    if(EdgeMap.count(eid)==1){
        fromVertex->removeOutgoingEdge(EdgeMap.find(eid)->second,rrSetID);
        toVertex->removeIncomingEdge(EdgeMap.find(eid)->second,rrSetID);
    }
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

