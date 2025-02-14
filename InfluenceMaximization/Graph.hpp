//
//  Graph.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/4/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#ifndef Graph_hpp
#define Graph_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <queue>
#include <ctime>
#include <deque>
#include <string.h>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <string>
#include "RRassociatedGraph.hpp"

using namespace std;
class CompareOutdegree {
public:
    bool operator()(const pair<int,int> &a,const pair<int,int> &b)
    {
        return (a.second > b.second);
    }
};


class Graph {
private:
    float propogationProbability;
    int propogationProbabilityNumber;
    bool standardProbability;
    string graphName;
    float percentageTargets;
    vector<int> nonTargets;
    int numberOfTargets;
    int numberOfNonTargets;
    
    

public:
    Graph();
    RRassociatedGraph *RRas;
    int n, m;
    double modImpactTime;
    vector<bool> alreadyVisited;
    //vector<unordered_map<AS,vector<AS>>> AStree;
    //vector<AS> *match;
    
    vector<vector<int> > graph;
    vector<vector<int> > graphTranspose;
    vector<vector<int>> rrSets;
    vector<bool> labels;
    deque<int> q;
    vector<int> inDegree;
    vector<bool> visited;
    vector<int> visitMark;
    vector<int> NodeinRRsetsWithCounts;
    vector<vector<int>> RRgraph;
    
    vector<int> outdegree;
    priority_queue<pair<int,int>,vector<pair<int,int>>,CompareOutdegree> workQueue;
    //set<pair<int,int>,CompareOutdegree> workMap;
    vector<vector<set<int>>> associatedSet;
    vector<set<int>>nodeAS;
    vector<unordered_map<int,unordered_set<int>>> pairAssociatedSet;
    vector<int> coverage;
    
    void readGraph(string fileName,std::ofstream& resultLogFile);
    void readGraph(string fileName, float percentage,std::ofstream& resultLogFile);
    void readReverseGraph(string fileName, float percentage);
    void readInfluencedGraph(string fileName, float percentage,vector<int> activatedSet);
    vector<int> writeInfluencedGraph(string fileName, float percentage, string convertedFile, vector<int> *seedNodes,vector<int> *seedOrder);
    void readHalfGraph(string fileName, float percentage,int graphCutValue,std::ofstream& resultLogFile);
    void readInfluencedHalfGraph(string fileName, float percentage, string influenceFile,int graphCutValue,std::ofstream& resultLogFile,bool fullgraph);
    void readLabels(string fileName);
    void writeLabels(std::ofstream& resultLogFile);
    void setLabels(vector<bool> labels, float percentageTargets);
    void removeVertexFromRRassociatedGraph(int vertex);
    //Numbers
    int getNumberOfVertices();
    int getNumberOfEdges();
    int getNumberOfTargets();
    int getNumberOfNonTargets();
    
    //Data Structure
    vector<int> *getNonTargets();
    
    vector<vector<int>> constructTranspose(vector<vector<int> > aGraph);
    void generateRandomRRSets(int R, bool label,std::ofstream& resultLogFile);
    void generateRandomRRSetsFromTargets(int R, vector<int> activatedSet, string modular,std::ofstream& resultLogFile);
    
    vector<int> generateRandomRRSet(int randomVertex, int rrSetID);
    void generateRandomRRSetwithCount(int randomVertex, int rrSetID);
    void generateRandomRRSetwithCountMod(int randomVertex, int rrSetID);
    void BFSonRRgraphs(int Rid,int rrSetID);
    void UpdateAssociatedSetMatrix(int rrSetID);
    void generateRandomRRSetwithRRgraphs(int randomVertex, int rrSetID);
    void clearRandomRRSets();
    vector<vector<int>>* getRandomRRSets();
    
    vector<int> oldRRSetGeneration(int randomVertex, int rrSetID);
    void assertTransposeIsCorrect();
    
    //Functions for propogation probability
    void setPropogationProbability(float p);
    bool flipCoinOnEdge(int u, int v);
    int generateRandomNumber(int u, int v);
    int getPropogationProbabilityNumber();
    void removeOutgoingEdges(int v);
    void removeNodeFromRRset(int v);
    void removeSetFromASmatrix(int row, int vertex, int rrSetID);
    void addSetintoASmatrix(int row, int vertex, int rrSetID);
};

#endif /* Graph_hpp */



