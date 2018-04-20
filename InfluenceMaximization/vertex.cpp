//
//  vertex.cpp
//  InfluenceMaximization
//
//  Created by Preeti on 4/18/18.
//  Copyright © 2018 Madhavan R.P. All rights reserved.
//

#include "vertex.hpp"
vertex:: vertex(int id) {
    this->id = id;
    this->indDegree = 0;
    this->outDegree = 0;
}

int vertex::getId() {
    return id;
}

void vertex::setId(int id) {
    this->id = id;
}

int vertex::getIndDegree() {
    return indDegree;
}

void vertex::setIndDegree(int indDegree) {
    this->indDegree = indDegree;
}

int vertex:: getOutDegree() {
    return outDegree;
}

void vertex:: setOutDegree(int outDegree) {
    this->outDegree = outDegree;
}

vector<Edge*> vertex:: getinComingEdges() {
    return inComingEdges;
}

void vertex::  setInBoundNeighbours(vector<Edge*>inBoundNeighbours) {
    this->inComingEdges = inBoundNeighbours;
}

vector<Edge*> vertex::getoutGoingEdges() {
    return outGoingEdges;
}

void vertex::setOutBoundNeighbours(vector<Edge*> outBoundNeighbours) {
    this->outGoingEdges = outBoundNeighbours;
}


void vertex::addInComingEdges(Edge *e) {
        this->inComingEdges.push_back(e);
        this->indDegree++;
    
}

void vertex::addOutGoingEdges(Edge *e) {
        this->outGoingEdges.push_back(e);
        this->outDegree++;

}

void vertex::removeOutgoingEdge(Edge* e, int rrSetID) {
    for(Edge* z:this->outGoingEdges){
        if(z ==e){
            z->removeRRid(rrSetID);
            outDegree--;
            return;
        }
    }
}

void vertex::removeIncomingEdge(Edge* e,int rrSetID) {
    for(Edge* z:this->inComingEdges){
        if(z ==e){
            z->removeRRid(rrSetID);
            indDegree--;
            return;
        }
    }
}

void vertex:: deleteOutBoundNeighbour(){
    this->outGoingEdges.clear();
    this->setOutDegree(0);
    this->setIndDegree(0);
}

/*
 public void vertex::setLabel(String label) {
 if (this.getProperties() != null) {
 this.properties.put("label", label);
 } else {
 this.properties = new HashMap<>();
 this.properties.put("label", label);
 }
 }
 
 String vertex:: getLabel() {
 return this->properties.get("label");
 }
 unordered_map<int,unordered_set<int>> vertex:: getLabels() {
 return labels;
 }
 
 void vertex::setLabels(unordered_map<int,unordered_set<int>> labels) {
 this->labels = labels;
 }
 
 unordered_set<int> vertex::getVertexLabel(vertex neighbour) {
 return this->labels.find(neighbour.id);
 }
 
 
 boolean vertex::hasLabel(Set<String> targetLabels) {
 for (String targetLabel : targetLabels) {
 if (this->getProperties().get("label").equals(targetLabel)) {
 return true;
 }
 }
 return false;
 }
 */
