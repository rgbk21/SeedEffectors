//
//  main.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/4/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include <iostream>
#include "cxxopts.hpp"
#include "InfluenceMaximization/Graph.hpp"
#include "InfluenceMaximization/IMTree.hpp"
#include "InfluenceMaximization/EstimateNonTargets.hpp"
#include "InfluenceMaximization/TIMUtility.hpp"
#include "InfluenceMaximization/Phase2.hpp"
#include "InfluenceMaximization/SeedSet.hpp"
#include "InfluenceMaximization/Diffusion.hpp"
#include "InfluenceMaximization/IMResults/IMResults.h"
#include "InfluenceMaximization/memoryusage.h"
#include <string>
#include <chrono>
#include "InfluenceMaximization/log.h"
#include "InfluenceMaximization/DifferenceApproximator.hpp"
#include "InfluenceMaximization/GenerateGraphLabels.hpp"
#include "InfluenceMaximization/BaselineGreedy.hpp"
#include "InfluenceMaximization/BaselineGreedyTIM.hpp"
#include <stdio.h>

#include <iomanip>
#include <ctime>
#include <sstream>
#include <fstream>
#include <cstdio>

using json = nlohmann::json;
using namespace std;

#define PHASE1TIM_PHASE2TIM 1;
#define PHASE1SIM_PHASE2SIM 2;

int budget;
int nonTargetThreshold;
string graphFileName;
int percentageTargets;
bool fromFile = false;
string nonTargetsFileName;
int method = PHASE1TIM_PHASE2TIM;
bool useIndegree = true;
float probability = 0;
int removeNodes=0;
string modular;
int topBestThreshold=100;
int initialSeed;
int newSeed;
int diffusion;
ofstream myfile;

ofstream resultLogFile;

void setupLogger() {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    
    strftime(buffer,sizeof(buffer),"%d-%m-%Y-%I:%M:%S",timeinfo);
    std::string str(buffer);
    FILELog::ReportingLevel() = logDEBUG3;
    string logFileName = "logs/influence-" + str + ".log";
    FILE* log_fd = fopen( logFileName.c_str(), "w" );
    Output2FILE::Stream() = log_fd;
}

bool sortbysecdesc(const pair<int,int> &a,const pair<int,int> &b)
{
    return (a.second > b.second);
}

bool sortbydegree(const int &a,const int &b)
{
    return (a > b);
}

void testApprox(Graph *graph, int budget, ApproximationSetting setting, bool extendPermutation) {
    DifferenceApproximator differenceApproximator(graph);
    differenceApproximator.setN(graph->n);
    set<int> seedSet;
    vector<int> permutation = differenceApproximator.generatePermutation();
    ModularApproximation modularApprox(permutation, setting);
    modularApprox.createTIMEvaluator(graph);
    modularApprox.findAllApproximations();
    if(setting==setting3) {
        if(!extendPermutation) {
            seedSet = differenceApproximator.executeAlgorithmApproximatingOneFunction(setting, budget);
        } else {
            seedSet = differenceApproximator.executeAlgorithmApproximatingOneFunctionExtendPermutation(setting, budget);
        }
    } else {
        if(!extendPermutation) {
            seedSet = differenceApproximator.executeGreedyAlgorithm(graph, &modularApprox, budget);
        } else {
            seedSet = differenceApproximator.executeGreedyAlgorithmAdjustingPermutation(setting, budget);
        }
    }
    pair<int, int> influence = findInfluenceUsingDiffusion(graph, seedSet, NULL);
    cout <<"\n Results after Diffusion: ";
    cout << "\nInfluence Targets: " << influence.first;
    cout << "\nInfluence NT: " << influence.second;
    IMResults::getInstance().setApproximationInfluence(influence);
    
}

void loadResultsFileFrom(cxxopts::ParseResult result) {
    // Necessary paramters
    int budget = result["budget"].as<int>();
    string graphFileName = result["graph"].as<std::string>();
    int percentageTargets = result["percentage"].as<int>();
    float percentageTargetsFloat = (float)percentageTargets/(float)100;
    string algorithm = result["algorithm"].as<string>();
    IMResults::getInstance().setBudget(budget);
    IMResults::getInstance().setGraphName(graphFileName);
    IMResults::getInstance().setPercentageTargets(percentageTargets);
    IMResults::getInstance().setAlgorithm(algorithm);
    
    // Optional parameters
    if(result["threshold"].count()>0) {
        int nonTargetThreshold = result["threshold"].as<int>();
        IMResults::getInstance().setNonTargetThreshold(nonTargetThreshold);
    }
    IMResults::getInstance().setPropagationProbability("inDegree");
    if(result.count("p")>0) {
        double probability = result["p"].as<double>();
        IMResults::getInstance().setPropagationProbability(probability);
    }
    
    if(result.count("ntfile")>0) {
        string nonTargetsFileName = result["ntfile"].as<std::string>();
        IMResults::getInstance().setFromFile(true);
        IMResults::getInstance().setNonTargetFileName(nonTargetsFileName);
    }
}

void loadGraphSizeToResults(Graph *graph) {
    IMResults::getInstance().setNumberOfVertices(graph->getNumberOfVertices());
    IMResults::getInstance().setNumberOfEdges(graph->getNumberOfEdges());
}


void checkMod(string graphFileName, float percentageTargetsFloat, Graph* graph,set<int> seedSet,int budget,bool useIndegree, float probability){
    //find seed set through modular approach
    set<int> modseedSet;
    Graph *modGraph = new Graph;
    modGraph->readGraph(graphFileName, percentageTargetsFloat,resultLogFile);
    if(!useIndegree) {
        modGraph->setPropogationProbability(probability);
    }
    int n = (int)modGraph->getNumberOfVertices();
    double epsilon = (double)EPSILON;
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
    modGraph->generateRandomRRSetsFromTargets(R, vector<int>(),"modular",resultLogFile);
    vector<pair<int,int>> SortedNodeidCounts=vector<pair<int,int>>();
    for(int i=0;i<modGraph->NodeinRRsetsWithCounts.size();i++){
        pair<int,int> node= pair<int,int>();
        node.first=i;
        node.second=modGraph->NodeinRRsetsWithCounts[i];
        SortedNodeidCounts.push_back(node);
    }
    
    vector<int>().swap(modGraph->NodeinRRsetsWithCounts);
    
    std :: sort(SortedNodeidCounts.begin(),SortedNodeidCounts.end(), sortbysecdesc);
    int j=0;
    cout<<"\n Order of mod SeedSet: " << flush;
    resultLogFile<<"\n Order of mod SeedSet: " << flush;
    for(int i=0;i<budget;i++){
        modseedSet.insert(SortedNodeidCounts[i].first);
        cout<< " " << SortedNodeidCounts[i].first;
        resultLogFile<< " " << SortedNodeidCounts[i].first;
        if(seedSet.count(SortedNodeidCounts[i].first)==1)
            j++;
    }
    cout<<"\n Selected k mod SeedSet: " << flush;
    resultLogFile<<"\n Selected k mod SeedSet: " << flush;
    for(auto item:modseedSet){
        cout<< item << " ";
        resultLogFile<< item << " ";
    }
    cout<<"\n intersection of submodular and modular "<< j;
    resultLogFile<<"\n intersection of submodular and modular "<< j;
    //intersection of the nodes activated through mod seed set and submod seed set after diffusion
    /*vector<int> activatedSet=performDiffusion(graph,seedSet,NULL);
     cout << "\n submod activated size" <<activatedSet.size();
     
     vector<int> modActivatedSet=performDiffusion(modGraph,modseedSet,NULL);
     cout << "\n mod activated size" <<modActivatedSet.size();
     
     std::sort(modActivatedSet.begin(), modActivatedSet.end());
     std::sort(activatedSet.begin(), activatedSet.end());
     std::vector<int> v_intersection;
     std::set_intersection(modActivatedSet.begin(), modActivatedSet.end(),activatedSet.begin(), activatedSet.end(),std::back_inserter(v_intersection));
     cout << "\n influence intersection of mod and submod" <<v_intersection.size();*/
}

set<int> getSeed(Graph *graph,vector<int> activatedSet,set<int>modNodes,set<int>subModNodes,vector<int> *seedOrder){
    vector<vector<int>> lookupTable;
    TIMCoverage timCoverage(&lookupTable);
    double epsilon = 2;
    int n = graph->n;
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
    graph->generateRandomRRSets(R, true,resultLogFile);
    vector<vector<int>> *randomRRSets = graph->getRandomRRSets();
    timCoverage.initializeLookupTable(randomRRSets, graph->n);
    timCoverage.initializeDataStructures((int)randomRRSets->size(), graph->n);
    unordered_set<int> activatedNodes;
    activatedNodes.insert(activatedSet.begin(),activatedSet.end());
    set<int> seedSet = timCoverage.findTopKNodes(budget, randomRRSets,activatedNodes,modNodes,subModNodes,seedOrder);
    return seedSet;
}


set<int> removeVertices(Graph *influencedGraph, int removeNodes, set<int> seedSet, vector<int> activatedSet, string modular){
    //Random RR sets
    set<int> alreadyinSeed= set<int>();
    int n = (int)activatedSet.size();
    double epsilon = (double)EPSILON;
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
    vector<int>().swap(influencedGraph->NodeinRRsetsWithCounts);
    
    influencedGraph->generateRandomRRSetsFromTargets(R, activatedSet, modular,resultLogFile);
    cout << "\n RRsets done " << flush;
    resultLogFile << "\n RRsets done " << flush;
    int modStrength=0;
    for(int i=0;i<influencedGraph->NodeinRRsetsWithCounts.size();i++){
        modStrength+=influencedGraph->NodeinRRsetsWithCounts[i];
    }
    cout<<"\n \n Initial Strength is "<<modStrength;
    resultLogFile<<"\n \n Initial Strength is "<<modStrength;
    //clearing the memory
    vector<vector<int>>().swap(influencedGraph->rrSets);
    
    //Find nodes to be removed
    vector<pair<int,int>> SortedNodeidCounts=vector<pair<int,int>>();
    for(int i=0;i<influencedGraph->NodeinRRsetsWithCounts.size();i++){
        pair<int,int> node= pair<int,int>();
        node.first=i;
        node.second=influencedGraph->NodeinRRsetsWithCounts[i];
        SortedNodeidCounts.push_back(node);
    }
    
    
    std :: sort(SortedNodeidCounts.begin(),SortedNodeidCounts.end(), sortbysecdesc);
    assert(SortedNodeidCounts.at(0).second>=SortedNodeidCounts.at(1).second);
    vector<int>().swap(influencedGraph->NodeinRRsetsWithCounts);
    
    set<int> nodesToRemove;
    int i=0;
    int j=0;
    while(j<removeNodes && j< SortedNodeidCounts.size()){
        int nodeid=SortedNodeidCounts.at(i).first;
        if(nodesToRemove.count(nodeid)==0 /*&& seedSet.count(nodeid)==0*/){
            nodesToRemove.insert(nodeid);
            j++;
            if(seedSet.count(nodeid)==1){
                alreadyinSeed.insert(nodeid);
            }
        }
        i++;
    }
    vector<pair<int,int>>().swap(SortedNodeidCounts);
    cout << "\n Number of nodes Already present in seed set = " << alreadyinSeed.size()<<flush;
    resultLogFile << "\n Number of nodes Already present in seed set = " << alreadyinSeed.size()<<flush;
    for(int i:nodesToRemove){
        influencedGraph->removeOutgoingEdges(i);
        assert(influencedGraph->graph[i].size()==0);
        assert(influencedGraph->graphTranspose[i].size()==0);
    }
    influencedGraph->generateRandomRRSetsFromTargets(R, activatedSet, "modular",resultLogFile);
    modStrength=0;
    for(int i=0;i<influencedGraph->NodeinRRsetsWithCounts.size();i++){
        modStrength+=influencedGraph->NodeinRRsetsWithCounts[i];
    }
    cout<<"\n \n After removing Modular Strength is "<<modStrength;
    resultLogFile<<"\n \n After removing Modular Strength is "<<modStrength;
    myfile <<modStrength<<" ";
    vector<vector<int>>().swap(influencedGraph->rrSets);
    vector<int>().swap(influencedGraph->NodeinRRsetsWithCounts);
    return  nodesToRemove;
}



void newDiffusion(Graph *newGraph,Graph *subNewGraph,Graph *modImpactGraph, set<int>modNodes,set<int>subModNodes,set<int> *removalModImpact,vector<int> activatedSet,int newSeed,float percentageTargetsFloat,string convertedFile){
    
    vector<int> modResults;
    vector<int> SubmodResults;
    vector<int> SubImapctResults;
    
    cout<< "nodes To remove in mod graph ";
    resultLogFile<< "nodes To remove in mod graph ";
    for(int i:modNodes){
        cout<< i << " ";
        resultLogFile<< i << " ";
    }
    cout<< "\n nodes To remove in submod graph "<<flush;
    resultLogFile<< "\n nodes To remove in submod graph ";
    
    int A=0;
    for(int i:subModNodes){
        cout<< i << " ";
        resultLogFile<< i << " ";
        if(modNodes.count(i)==1)
            A++;
        subNewGraph->removeOutgoingEdges(i);
        assert(subNewGraph->graph[i].size()==0);
        assert(subNewGraph->graphTranspose[i].size()==0);
    }
    
    cout<< "\n nodes To remove in mod Impact graph "<<flush;
    resultLogFile<< "\n nodes To remove in mod Impact graph ";
    int B=0;
    int C=0;
    for(int i: *removalModImpact){
        cout<< i << " ";
        resultLogFile<< i << " ";
        if(subModNodes.count(i)==1)
            B++;
        if(modNodes.count(i)==1)
            C++;
        modImpactGraph->removeOutgoingEdges(i);
        assert(modImpactGraph->graph[i].size()==0);
        assert(modImpactGraph->graphTranspose[i].size()==0);
    }
    cout<< "\n intersection of mod and submod nodes to remove "<<A;
    cout<< "\n intersection of submod and submodImpact nodes to remove "<<B;
    cout<< "\n intersection of mod and submodImpact nodes to remove "<<C<<"\n";
    resultLogFile<< "\n intersection of mod and submod nodes to remove "<<A;
    resultLogFile<< "\n intersection of submod and submodImpact nodes to remove "<<B;
    resultLogFile<< "\n intersection of mod and submodImpact nodes to remove "<<C<<"\n";
    //Random RR sets
    int n = (int)activatedSet.size();
    double epsilon = (double)EPSILON;
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
    cout<< "RR sets are: "<<R;
    resultLogFile<< "RR sets are: "<<R;
    modImpactGraph->generateRandomRRSetsFromTargets(R, activatedSet, "modular",resultLogFile);
    int modImapactStrength=0;
    for(int i=0;i<modImpactGraph->NodeinRRsetsWithCounts.size();i++){
        modImapactStrength+=modImpactGraph->NodeinRRsetsWithCounts[i];
    }
    cout<<"\n \n After removing mod Impact Modular Strength is "<<modImapactStrength<<"\n";
    resultLogFile<<"\n \n After removing mod Impact Modular Strength is "<<modImapactStrength<<"\n";
    
    myfile <<modImapactStrength<<" ";
    vector<vector<int>>().swap(modImpactGraph->rrSets);
    vector<int>().swap(modImpactGraph->NodeinRRsetsWithCounts);
    
    SeedSet *SeedClass = new SeedSet(newGraph , budget);
    set<int> seedSet=set<int>();
    Graph *graph;
    int k=0;
    while(k<10){
        switch(1){
            case 0: //bestTim
                seedSet=getSeed(newGraph,activatedSet,modNodes,subModNodes,NULL);
                break;
            case 1:// best first Half Graph
                graph = new Graph;
                graph->readInfluencedHalfGraph(graphFileName, percentageTargetsFloat,convertedFile,8*k,resultLogFile);
                //graph->readHalfGraph(graphFileName, percentageTargetsFloat,8*k);
                if(!useIndegree) {
                    graph->setPropogationProbability(probability);
                }
                seedSet=getSeed(graph,activatedSet,modNodes,subModNodes,NULL);
                break;
            case 2: //random
                seedSet=SeedClass->getCompletelyRandom(modNodes,subModNodes);
                break;
            case 3: //random from OutDegree threshold nodes
                seedSet=SeedClass->outdegreeRandom(topBestThreshold,modNodes,subModNodes);
                break;
            case 4: //farthest from OutDegree threshold nodes
                seedSet=SeedClass->outdegreeFarthest(topBestThreshold);
                break;
        }
        
        cout<<"\n \n Selected new SeedSet in original graph: " << flush;
        resultLogFile<<"\n \n Selected new SeedSet in original graph: " << flush;
        for(auto item:seedSet){
            cout<< item << " ";
            resultLogFile<< item << " ";
        }
        
        for(int i:modNodes){
            newGraph->removeOutgoingEdges(i);
            assert(newGraph->graph[i].size()==0);
            assert(newGraph->graphTranspose[i].size()==0);
        }
        
        cout<<"\n Mod Results: " << flush;
        resultLogFile<<"\n Mod Results: " << flush;
        modResults.push_back(oldNewIntersection(newGraph, seedSet,activatedSet,resultLogFile));
        cout<<"\n Sub Mod Results: " << flush;
        resultLogFile<<"\n Sub Mod Results: " << flush;
        SubmodResults.push_back(oldNewIntersection(subNewGraph, seedSet,activatedSet,resultLogFile));
        cout<<"\n Mod Impact Results: " << flush;
        resultLogFile<<"\n Mod Impact Results: " << flush;
        SubImapctResults.push_back(oldNewIntersection(modImpactGraph, seedSet,activatedSet,resultLogFile));
        k++;
    }
    double subModGain=0;
    double ImapctGain=0;
    for(int i=0;i<k;i++){
        subModGain+= float(modResults[i]-SubmodResults[i])/float(modResults[i]);
        ImapctGain+=float(modResults[i]-SubImapctResults[i])/float(modResults[i]);
    }
    subModGain=(float)subModGain/k;
    ImapctGain=(float)ImapctGain/k;
    myfile <<subModGain<<" "<<ImapctGain<<"\n";
    
    delete SeedClass;
}

set<int> subModularNodesRemove(Graph *influencedGraph, vector<int> activatedSet, int removeNodes, set<int> seedSet,set<int> *removalModImpact){
    set<int> alreadyinSeed= set<int>();
    
    clock_t subModReverseStartTime = clock();
    set<int> subModNodesToremove;
    //Random RR sets
    int n = (int)activatedSet.size();
    double epsilon = (double)EPSILON;
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
    cout<< "RR sets are: "<<R;
    resultLogFile<< "RR sets are: "<<R;
    influencedGraph->generateRandomRRSetsFromTargets(R, activatedSet,modular,resultLogFile);
    
    int removalNum=removeNodes;
    bool SubImpact=false;
    while(removeNodes!=0){
        vector<pair<int,int>> SortedNodeidCounts=vector<pair<int,int>>();
        for(int i=0;i<influencedGraph->coverage.size();i++){
            pair<int,int> node= pair<int,int>();
            node.first=i;
            node.second=influencedGraph->coverage[i];
            SortedNodeidCounts.push_back(node);
        }
        std :: sort(SortedNodeidCounts.begin(),SortedNodeidCounts.end(), sortbysecdesc);
        assert(SortedNodeidCounts.at(0).second>=SortedNodeidCounts.at(1).second);
        
        if(!SubImpact){
            cout << "\n \n ******* Running Mod Impact approach ********" <<flush;
            resultLogFile << "\n \n ******* Running Mod Impact approach ********";
            int k=0;
            for(int i=0;i<removalNum;i++){
                if(seedSet.count(SortedNodeidCounts.at(i).first)==1){
                    k++;
                }
                removalModImpact->insert(SortedNodeidCounts.at(i).first);
                SubImpact=true;
            }
            cout << "\n Number of nodes for (mod impact) already present in seed set = " <<k;
            resultLogFile << "\n Number of nodes for (mod impact) already present in seed set = " <<k;
            clock_t subModImapctEndTime = clock();
            double totalModImapctTime = double(subModImapctEndTime-subModReverseStartTime-influencedGraph->modImpactTime) / (CLOCKS_PER_SEC*60);
            cout << "\n Reverse submod impact algorithm time in minutes " << totalModImapctTime<<"\n";
            resultLogFile << "\n Reverse submod impact algorithm time in minutes " << totalModImapctTime<<"\n";
            myfile <<totalModImapctTime<<" ";
        }
        int h=0;
        /*while(seedSet.count(SortedNodeidCounts.at(h).first)==1){
         h++;
         }*/
        int node = SortedNodeidCounts.at(h).first;
        subModNodesToremove.insert(node);
        if(seedSet.count(node)==1){
            alreadyinSeed.insert(node);
        }
        //remove node from RRset
        influencedGraph->removeNodeFromRRset(node);
        removeNodes--;
    }
    clock_t subModReverseEndTime = clock();
    
    vector<vector<int>>().swap(influencedGraph->rrSets);
    for(int i:subModNodesToremove){
        influencedGraph->removeOutgoingEdges(i);
        assert(influencedGraph->graph[i].size()==0);
        assert(influencedGraph->graphTranspose[i].size()==0);
    }
    influencedGraph->generateRandomRRSetsFromTargets(R, activatedSet,"modular",resultLogFile);
    int subModStrength=0;
    for(int i=0;i<influencedGraph->NodeinRRsetsWithCounts.size();i++){
        subModStrength+=influencedGraph->NodeinRRsetsWithCounts[i];
    }

    cout << "\n Number of nodes Already present in seed set = " << alreadyinSeed.size();
    cout << "\n Submodular strength = "<<subModStrength;
    resultLogFile << "\n Number of nodes Already present in seed set = " << alreadyinSeed.size();
    resultLogFile << "\n Submodular strength = "<<subModStrength;
    myfile <<subModStrength<<" ";
    double totalAlgorithmTime = double(subModReverseEndTime-subModReverseStartTime) / (CLOCKS_PER_SEC*60);
    cout << "\n Reverse submodular algorithm time in minutes " << totalAlgorithmTime;
    resultLogFile << "\n Reverse submodular algorithm time in minutes " << totalAlgorithmTime;
    myfile <<totalAlgorithmTime<<" ";
    return subModNodesToremove;
}

void convertInfluenceFile(string convertedFile, string influenceFile, set<int> seedSet,int n,int newVertices,vector<int> *seedOrder){
    vector<int> vertexIdMap=vector<int>(n,-1);
    ifstream myFile(influenceFile);
    ofstream newfile;
    newfile.open (convertedFile);
    newfile <<newVertices<<"\n";
    int u;
    int v;
    int numofEdges=0;
    int i=-1;
    if(myFile.is_open()){
        while (myFile >> u >> v) {
            numofEdges++;
            if(vertexIdMap[u]==-1){
                vertexIdMap[u]=++i;
                newfile <<i;
            }
            else{
                newfile <<vertexIdMap[u];
            }
            if(vertexIdMap[v]==-1){
                vertexIdMap[v]=++i;
                newfile <<" "<<i<<"\n";
            }
            else{
                newfile <<" "<<vertexIdMap[v]<<"\n";
            }
        }
    }
    newfile <<-1<<" "<<-1<<"\n";
    for(int i:seedSet){
        newfile<<i<<" ";
    }
    newfile <<"\n"<<-1<<" "<<-1<<"\n";
    for(int i: *seedOrder){
        newfile<<i<<" ";
    }
    newfile.close();
    myFile.close();
    
    if( std::remove(influenceFile.c_str( )) != 0 )
     perror( "Error deleting file" );
     else
     puts( "File successfully deleted" );
}

int getNumberofVertices(string influenceFile){
    ifstream myFile(influenceFile);
    unordered_set<int> vertices= unordered_set<int>();
    int u;
    int v;
    if(myFile.is_open()){
        while (myFile >> u >> v) {
            vertices.insert(u);
            vertices.insert(v);
        }
    }
    return (int)vertices.size();
}


void executeTIMTIM(cxxopts::ParseResult result) {
    clock_t executionTimeBegin = clock();

    IMResults::getInstance().setFromFile(fromFile);
    // insert code here...
    float percentageTargetsFloat = (float)percentageTargets/(float)100;
    
    //Generate graph
    Graph *graph = new Graph;
    int half_seed=initialSeed;
    if(initialSeed==2){
        graph->readHalfGraph(graphFileName, percentageTargetsFloat,50,resultLogFile);
        initialSeed=1;
    }
    else{
        graph->readGraph(graphFileName, percentageTargetsFloat,resultLogFile);
    }
    if(!useIndegree) {
        graph->setPropogationProbability(probability);
    }
    
    set<int> seedSet;
    vector<int> *seedOrder=new vector<int>(budget);
    SeedSet *SeedClass = nullptr;
    switch(initialSeed){
        case 1://bestTim for  case1, besthalfGRaph for case 2
            seedSet=getSeed(graph,vector<int>(),set<int>(),set<int>(),seedOrder);
            //checkMod(graphFileName,percentageTargetsFloat,graph,seedSet,budget,useIndegree,probability);
            if(half_seed==2){
                graph->readGraph(graphFileName, percentageTargetsFloat,resultLogFile);
                if(!useIndegree) {
                    graph->setPropogationProbability(probability);
                }
            }
            break;
        case 3: //random
            SeedClass=new SeedSet(graph , budget);
            seedSet=SeedClass->getCompletelyRandom(set<int>(),set<int>());
            break;
        case 4: //random from OutDegree threshold nodes
            SeedClass=new SeedSet(graph , budget);
            seedSet=SeedClass->outdegreeRandom(topBestThreshold,set<int>(),set<int>());
            break;
        case 5: //farthest from OutDegree threshold nodes
            SeedClass=new SeedSet(graph , budget);
            seedSet=SeedClass->outdegreeFarthest(topBestThreshold);
            break;
        default: break;
    }
    delete SeedClass;
    
    //Start Diffusion
    cout<< "\n Diffusion on graph started"<< flush;
    resultLogFile <<"\n Diffusion on graph started";
    
    vector<int> activatedSet;
    string influenceFile;
    string convertedFile="graphs/"+graphFileName+"_converted"+"_"+to_string(budget)+"_"+to_string(probability);
    int newVertices=0;
    
    switch(diffusion){
        case 1:activatedSet=performDiffusion(graph,seedSet,NULL);
            break;
        case 2:influenceFile=singleDiffusionInfluence(graph, seedSet, graphFileName,budget, probability);
            vector<vector<int>>().swap(graph->rrSets);
            newVertices=getNumberofVertices(influenceFile);
            convertInfluenceFile(convertedFile,influenceFile,seedSet,graph->n,newVertices,seedOrder);
            break;
            /*case 3:set<int> active=dagDiffusion(graph,seedSet);
             break;*/
        default: break;
    }
    delete graph;
    
    cout<< "\n Creating Influenced Graph "<< flush;
    resultLogFile <<"\n Creating Influenced Graph ";
    
    Graph *influencedGraph = new Graph;
    //influencedGraph->readInfluencedGraph(graphFileName, percentageTargetsFloat,activatedSet);
    vector<int> *seedNodes= new vector<int>(budget);
    activatedSet=influencedGraph->writeInfluencedGraph(graphFileName, percentageTargetsFloat,convertedFile,seedNodes,seedOrder);
    if(!useIndegree) {
        influencedGraph->setPropogationProbability(probability);
    }
    
    cout<<"\n Selected original SeedSet: " << flush;
    resultLogFile <<"\n Selected original SeedSet: ";
    
    for(int i: *seedNodes){
        seedSet.insert(i);
        cout<< i << " ";
        resultLogFile <<i<<" ";
    }
    cout<<"\n Selected Order of SeedSet: " << flush;
    resultLogFile <<"\n Selected Order of SeedSet: ";
    
    for(int j: *seedOrder){
        cout<< j << " ";
        resultLogFile<< j << " ";
    }
    myfile <<activatedSet.size()<<" ";
    cout << "\n Targets activated = " << activatedSet.size();
    cout << "\n Non targets are = " << influencedGraph->getNumberOfNonTargets()<< flush;
    
    resultLogFile << "\n Targets activated = " << activatedSet.size();
    resultLogFile << "\n Non targets are = " << influencedGraph->getNumberOfNonTargets();
    
    //get node to be removed
    set<int> modNodesToremove;
    cout << "\n ******* Running modular approach ******** \n" <<flush;
    resultLogFile << "\n ******* Running modular approach ******** \n";
    
    clock_t ModReverseStartTime = clock();
    modNodesToremove= removeVertices(influencedGraph, removeNodes, seedSet, activatedSet,"modular");
    clock_t ModReverseEndTime = clock();
    double totalAlgorithmTime = double(ModReverseEndTime-ModReverseStartTime) / (CLOCKS_PER_SEC*60);
    cout << "\n Reverse algorithm time in minutes \n" << totalAlgorithmTime<<flush;
    resultLogFile << "\n Reverse algorithm time in minutes \n" << totalAlgorithmTime;
    
    myfile <<totalAlgorithmTime<<" ";
    delete influencedGraph;

    //else{
    cout << "\n \n ******* Running Sub Modular approach ******** \n" <<flush;
    resultLogFile << "\n \n ******* Running Sub Modular approach ******** \n" <<flush;
    Graph *subInfluencedGraph = new Graph;
    vector<int> SubactivatedSet=subInfluencedGraph->writeInfluencedGraph(graphFileName, percentageTargetsFloat,convertedFile,NULL,NULL);
    if(!useIndegree) {
        subInfluencedGraph->setPropogationProbability(probability);
    }
    set<int> *removalModImpact=new set<int>();
    set<int> subModNodesToremove=subModularNodesRemove(subInfluencedGraph,SubactivatedSet,removeNodes, seedSet,removalModImpact);
    delete subInfluencedGraph;
    
    cout << "\n \n******* Node removed in all three approaches ******** \n" <<flush;
    resultLogFile << "\n \n******* Node removed in all three approaches ******** \n" <<flush;
    Graph *modNewGraph = new Graph;
    modNewGraph->writeInfluencedGraph(graphFileName, percentageTargetsFloat,convertedFile,NULL,NULL);
    if(!useIndegree) {
        modNewGraph->setPropogationProbability(probability);
    }
    
    Graph *modImpactGraph = new Graph;
    modImpactGraph->writeInfluencedGraph(graphFileName, percentageTargetsFloat,convertedFile,NULL,NULL);
    if(!useIndegree) {
        modImpactGraph->setPropogationProbability(probability);
    }
    
    Graph *subNewGraph = new Graph;
    subNewGraph->writeInfluencedGraph(graphFileName, percentageTargetsFloat,convertedFile,NULL,NULL);
    if(!useIndegree) {
        subNewGraph->setPropogationProbability(probability);
    }newDiffusion(modNewGraph,subNewGraph,modImpactGraph,modNodesToremove,subModNodesToremove,removalModImpact,activatedSet,newSeed,percentageTargetsFloat,convertedFile);
    
    clock_t executionTimeEnd = clock();
    double totalExecutionTime = double(executionTimeEnd - executionTimeBegin) / (CLOCKS_PER_SEC*60);
    cout << "\n Elapsed time in minutes " << totalExecutionTime;
    resultLogFile << "\n Elapsed time in minutes " << totalExecutionTime;
}

void executeTIMTIMfullGraph(cxxopts::ParseResult result) {
    clock_t executionTimeBegin = clock();
    
    IMResults::getInstance().setFromFile(fromFile);
    // insert code here...
    float percentageTargetsFloat = (float)percentageTargets/(float)100;
    
    
    Graph *influencedGraph = new Graph;
    influencedGraph->readGraph(graphFileName, percentageTargetsFloat,resultLogFile);
    if(!useIndegree) {
        influencedGraph->setPropogationProbability(probability);
    }
    vector<int> activatedSet=vector<int>(influencedGraph->n);
    for(int i=0;i<influencedGraph->n;i++){
        activatedSet[i]=i;
    }
    set<int> seedSet;

    cout << "\n Targets activated = " << activatedSet.size();
    cout << "\n Non targets are = " << influencedGraph->getNumberOfNonTargets()<< flush;
    
    //get node to be removed
    set<int> modNodesToremove;
    cout << "\n ******* Running modular approach ******** \n" <<flush;
    resultLogFile << "\n ******* Running modular approach ******** \n";
    
    clock_t ModReverseStartTime = clock();
    modNodesToremove= removeVertices(influencedGraph, removeNodes, seedSet, activatedSet,"modular");
    clock_t ModReverseEndTime = clock();
    double totalAlgorithmTime = double(ModReverseEndTime-ModReverseStartTime) / (CLOCKS_PER_SEC*60);
    cout << "\n Reverse algorithm time in minutes \n" << totalAlgorithmTime<<flush;
    resultLogFile << "\n Reverse algorithm time in minutes \n" << totalAlgorithmTime;
    
    myfile <<totalAlgorithmTime<<" ";
    delete influencedGraph;
    
    cout << "\n \n ******* Running Sub Modular approach ******** \n" <<flush;
    resultLogFile << "\n \n ******* Running Sub Modular approach ******** \n" <<flush;
    Graph *subInfluencedGraph = new Graph;
    subInfluencedGraph->readGraph(graphFileName, percentageTargetsFloat,resultLogFile);
    if(!useIndegree) {
        subInfluencedGraph->setPropogationProbability(probability);
    }
    set<int> *removalModImpact=new set<int>();
    set<int> subModNodesToremove=subModularNodesRemove(subInfluencedGraph,activatedSet,removeNodes, seedSet,removalModImpact);
    delete subInfluencedGraph;
    
    cout << "\n \n******* Node removed in all three approaches ******** \n" <<flush;
    resultLogFile << "\n \n******* Node removed in all three approaches ******** \n" <<flush;
    Graph *modNewGraph = new Graph;
    modNewGraph->readGraph(graphFileName, percentageTargetsFloat,resultLogFile);
    if(!useIndegree) {
        modNewGraph->setPropogationProbability(probability);
    }
    
    Graph *modImpactGraph = new Graph;
    modImpactGraph->readGraph(graphFileName, percentageTargetsFloat,resultLogFile);
    if(!useIndegree) {
        modImpactGraph->setPropogationProbability(probability);
    }
    string convertedFile="graphs/"+graphFileName;
    Graph *subNewGraph = new Graph;
    subNewGraph->readGraph(graphFileName, percentageTargetsFloat,resultLogFile);
    if(!useIndegree) {
        subNewGraph->setPropogationProbability(probability);
    }newDiffusion(modNewGraph,subNewGraph,modImpactGraph,modNodesToremove,subModNodesToremove,removalModImpact,activatedSet,newSeed,percentageTargetsFloat,convertedFile);
    
    clock_t executionTimeEnd = clock();
    double totalExecutionTime = double(executionTimeEnd - executionTimeBegin) / (CLOCKS_PER_SEC*60);
    cout << "\n Elapsed time in minutes " << totalExecutionTime;
    resultLogFile << "\n Elapsed time in minutes " << totalExecutionTime;
}


int main(int argc, char **argv) {
    cout << "Starting program\n";
    
    string resultDataFile;
    srand(time(0));
    setupLogger();
    cout << "Setup logger \n";
    cxxopts::Options options("Targeted Influence Maximization", "Experiments and research.");
    options.add_options()
    ("algorithm", "Choose which algorithm to run", cxxopts::value<std::string>())
    ("graph", "Graph file name", cxxopts::value<std::string>())
    ("b,budget", "Budget size", cxxopts::value<int>())
    ("t,threshold", "NT threshold", cxxopts::value<int>())
    ("m,method", "TIM-TIM or SIM-SIM", cxxopts::value<int>())
    ("percentage", "Percentage of Targets", cxxopts::value<int>())
    ("n,ntfile", "Non Targets File name", cxxopts::value<std::string>())
    ("p,probability", "Propogation probability", cxxopts::value<double>())
    ("approximation", " Approximation Settings", cxxopts::value<int>())
    ("r,nodesRemove", " Remove nodes number", cxxopts::value<int>())
    ("w,modularity", " Modular selection", cxxopts::value<std::string>())
    ("s,seedset", " Seed set selection", cxxopts::value<int>())
    ("d,Diffusion", " Diffusion selection", cxxopts::value<int>())
    ("x,newSeedset", " New Seed set selection", cxxopts::value<int>())
    ("e,extend", "Extend the permutation");
    auto result = options.parse(argc, argv);
    string algorithm = result["algorithm"].as<string>();
    
    budget = result["budget"].as<int>();
    nonTargetThreshold = result["threshold"].as<int>();
    graphFileName = result["graph"].as<std::string>();
    percentageTargets = result["percentage"].as<int>();
    removeNodes=result["nodesRemove"].as<int>();
    initialSeed=result["seedset"].as<int>();
    modular=result["modularity"].as<std::string>();
    newSeed=result["newSeedset"].as<int>();
    diffusion=result["Diffusion"].as<int>();
    
    cout << "\n begin execution tim tim ";
    resultDataFile=graphFileName;
    resultDataFile+="_Budget_"+ to_string(budget);
    resultDataFile+="_Removal_"+ to_string(removeNodes);
    resultDataFile+="__RRapproach_Log.txt";
    resultDataFile = "ResultData/" + resultDataFile;
    resultLogFile.open(resultDataFile);
    
    loadResultsFileFrom(result);
    
    if(result.count("method")>0) {
        method = result["method"].as<int>();
    }
    if(result.count("p")>0) {
        probability = result["p"].as<double>();
        useIndegree = false;
    }
    if(result.count("ntfile")>0) {
        nonTargetsFileName = result["ntfile"].as<std::string>();
        fromFile = true;
    }
    
    // Log information
    cout << "\n Conducting experiments for:\n";
    cout <<" Graph: " << graphFileName;
    cout << "\t Budget: " << budget;
    cout << "\t Non Target Threshod: " << nonTargetThreshold;
    cout << "\t Percentage:  " << percentageTargets;
    cout << "\t Method: " << method;
    cout << "\t Nodes removed: " << removeNodes;
    cout << "\t Seed selection case: " << initialSeed;
    cout << "\t Top best outdegree threshold : " <<topBestThreshold;
    
    resultLogFile <<"\n Conducting experiments for:\n";
    resultLogFile <<" Graph: " << graphFileName;
    resultLogFile << "\t Budget: " << budget;
    resultLogFile << "\t Non Target Threshod: " << nonTargetThreshold;
    resultLogFile << "\t Percentage:  " << percentageTargets;
    resultLogFile << "\t Method: " << method;
    resultLogFile << "\t Nodes removed: " << removeNodes;
    resultLogFile << "\t Seed selection case: " << initialSeed;
    resultLogFile << "\t Top best outdegree threshold : " <<topBestThreshold;
    
    if(useIndegree) {
        cout << "\t Probability: Indegree";
        resultLogFile << "\t Probability: Indegree";
    } else {
        cout << "\t Probability: " <<  probability;
        resultLogFile << "\t Probability: " <<  probability;
    }
    if(fromFile) {
        cout << "\n Reading Non targets from file: " << nonTargetsFileName;
        resultLogFile << "\n Reading Non targets from file: " << nonTargetsFileName;
    }
    
    FILE_LOG(logDEBUG) << "\n Conducting experiments for:\n";
    FILE_LOG(logDEBUG) <<" Graph: " << graphFileName;
    FILE_LOG(logDEBUG) << "\t Budget: " << budget;
    FILE_LOG(logDEBUG) << "\t Non Target Threshod: " << nonTargetThreshold;
    FILE_LOG(logDEBUG) << "\t Percentage:  " << percentageTargets;
    FILE_LOG(logDEBUG) << "\t Method: " << method;
    if(fromFile) {
        FILE_LOG(logDEBUG) << "\n Reading Non targets from file: " << nonTargetsFileName;
    }
 
    string resultFile;
    resultFile=graphFileName;
    resultFile+="_RRapproach_results.txt";
    resultFile = "results/" + resultFile;
    myfile.open (resultFile,std::ios::app);
    myfile <<"\n"<<budget<<" "<<removeNodes<<" ";
    executeTIMTIMfullGraph(result);
    //executeTIMTIM(result);
    disp_mem_usage("");
    return 0;
}
