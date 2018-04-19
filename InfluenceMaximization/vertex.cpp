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

std::unordered_map<int,vertex*> vertex:: getInBoundNeighbours() {
    return inBoundNeighbours;
}

void vertex::  setInBoundNeighbours(std::unordered_map<int,vertex*>inBoundNeighbours) {
    this->inBoundNeighbours = inBoundNeighbours;
}

std::unordered_map<vertex*,std::unordered_set<int>> vertex::getOutBoundNeighbours() {
    return outBoundNeighbours;
}

void vertex::setOutBoundNeighbours(std::unordered_map<vertex*,std::unordered_set<int>> outBoundNeighbours) {
    this->outBoundNeighbours = outBoundNeighbours;
}


void vertex::addInBoundNeighbour(vertex *v) {
    this->inBoundNeighbours.insert(std::pair<int,vertex*>(v->getId(),v));
    indDegree++;
}

void vertex::addOutBoundNeighbour(vertex *v, int label) {
    
    std::unordered_map<vertex*,std::unordered_set<int>>::iterator it=this->outBoundNeighbours.find(v);
    if(it!=this->outBoundNeighbours.end()){
        it->second.insert(label);
    }
    else{
        std::pair<vertex*,std::unordered_set<int>> temp= std::pair<vertex*,std::unordered_set<int>>();
        temp.first=v;
        temp.second.insert(label);
        this->outBoundNeighbours.insert(temp);
    }
    //this->labels.insert(std::pair<vertex*,unordered_set<int>>(vertex*,new label);
    this->outDegree++;
}

void vertex::removeOutBoundNeighbour(vertex* toVertex) {
    if(this->outBoundNeighbours.count(toVertex)==1){
        this->outBoundNeighbours.erase(toVertex);
        outDegree--;
    }
}

void vertex::removeInBoundNeighbour(vertex* fromVertex) {
    this->inBoundNeighbours.clear();
    indDegree--;
}

void vertex:: deleteOutBoundNeighbour(){
    this->outBoundNeighbours.clear();
    this->outDegree=0;
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
