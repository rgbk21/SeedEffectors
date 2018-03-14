//
//  Graph.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/4/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "Graph.hpp"
#include <assert.h>
#include <sstream>
#include <iomanip>
using namespace std;
void Graph::readGraph(string fileName) {
    readGraph(fileName, 0.8);
}

Graph::Graph() {
    this->standardProbability = false;
}

void Graph::setPropogationProbability(float p) {
    this->propogationProbability = p;
    this->standardProbability = true;
    this->propogationProbabilityNumber = (float)1/p;
}

int Graph::getPropogationProbabilityNumber() {
    return this->propogationProbabilityNumber;
}

int Graph:: generateRandomNumber(int u, int v) {
    int randomNumberLimit;
    if(this->standardProbability) {
        randomNumberLimit = this->propogationProbabilityNumber;
    }
    else {
        randomNumberLimit = inDegree[v];
    }
    return rand() % randomNumberLimit;
}

bool Graph:: flipCoinOnEdge(int u, int v) {
    int randomNumber = generateRandomNumber(u, v);
    return randomNumber==0;
}

//********** Function for generating half graph ********
void Graph::readHalfGraph(string fileName, float percentage){
    this->graphName = fileName;
    this->percentageTargets = percentage;
    cout << "\n Generating half graph: ";
    
    ifstream myFile("graphs/" + fileName);
    string s;
    if(myFile.is_open()) {
        myFile >> n >> m;
        n=n/2;
        for(int i =0;i<n;i++) {
            graph.push_back(vector<int>());
            visited.push_back(false);
            inDegree.push_back(0);
            labels.push_back(true);
        }
        int from, to;
        int maxDegree = 0;
        while (myFile >> from >> to) {
            if(from< n && to < n){
                graph[from].push_back(to);
                inDegree[to] = inDegree[to]+1;
                if(inDegree[to] > maxDegree) {
                    maxDegree = inDegree[to];
                }
            }
        }
        myFile.close();
    }
    
    graphTranspose = constructTranspose(graph);
    visitMark = vector<int>(n);
    this->numberOfTargets = this->getNumberOfVertices();
    this->numberOfNonTargets = (int)nonTargets.size();
}

//********** Function only for the influenced graph ********

void Graph::readInfluencedGraph(string fileName, float percentage, vector<int> activatedSet) {
    this->graphName = fileName;
    this->percentageTargets = percentage;
    //cout << "\n Reading Reverse targets graph: ";
    
    ifstream myFile("graphs/" + fileName);
    string s;
    if(myFile.is_open()) {
        myFile >> n >> m;
        
        //cout<<"value of n in graph "<<n<<flush;
        for(int i =0;i<n;i++) {
            graph.push_back(vector<int>());
            visited.push_back(false);
            inDegree.push_back(0);
            labels.push_back(false);
        }
        for(int i:activatedSet) {
            labels[i]=true;
        }
        
        int from, to;
        int maxDegree = 0;
        while (myFile >> from >> to) {
            graph[from].push_back(to);
            inDegree[to] = inDegree[to]+1;
            if(inDegree[to] > maxDegree) {
                maxDegree = inDegree[to];
            }
        }
        myFile.close();
    }
    graphTranspose = constructTranspose(graph);
    visitMark = vector<int>(n);
    this->numberOfTargets =(int) activatedSet.size();
    this->numberOfNonTargets =this->getNumberOfVertices()-this->numberOfTargets;
}

void Graph::readGraph(string fileName, float percentage) {
    this->graphName = fileName;
    this->percentageTargets = percentage;
    cout << "\n Reading all targets graph: ";
    
    ifstream myFile("graphs/" + fileName);
    string s;
    if(myFile.is_open()) {
        myFile >> n >> m;
        for(int i =0;i<n;i++) {
            graph.push_back(vector<int>());
            visited.push_back(false);
            inDegree.push_back(0);
            labels.push_back(true);
        }
        int from, to;
        int maxDegree = 0;
        while (myFile >> from >> to) {
            graph[from].push_back(to);
            inDegree[to] = inDegree[to]+1;
            if(inDegree[to] > maxDegree) {
                maxDegree = inDegree[to];
            }
        }
        myFile.close();
    }
    
    graphTranspose = constructTranspose(graph);
    visitMark = vector<int>(n);
    this->numberOfTargets = this->getNumberOfVertices();
    this->numberOfNonTargets = (int)nonTargets.size();
    
    /*labels = vector<bool>(n);
    stringstream stream;
    stream << fixed << setprecision(2) << percentage;
    s = stream.str();
    cout << "\n Reading graph: " << fileName;
    cout << "\n Reading labels file name: " << "graphs/" + fileName + "_" + s + "_labels.txt";
    readLabels("graphs/" + fileName + "_" + s + "_labels.txt");*/
}


void Graph::readLabels(string fileName) {
    ifstream myFile(fileName);
    bool labelBool;
    if(myFile.is_open()) {
        int vertex;
        char label;
        while (myFile >> vertex >> label) {
            labelBool = (tolower(label)=='a');
            labels[vertex] = labelBool;
            if(!labelBool) {
                nonTargets.push_back(vertex);
            }
        }
        myFile.close();
    }
    this->numberOfTargets = this->getNumberOfVertices() - (int)nonTargets.size();
    this->numberOfNonTargets = (int)nonTargets.size();
}

void Graph::writeLabels() {
    string s;
    stringstream stream;
    stream << fixed << setprecision(2) << this->percentageTargets;
    s = stream.str();
    string labelFileName ="graphs/" + this->graphName + "_" + s + "_labels.txt";
    
    ofstream myfile;
    string fileName = labelFileName;
    myfile.open (fileName);
    string targetLabel = "A";
    string nonTargetLabel = "B";
    for(int i=0; i<this->n; i++) {
        if(this->labels[i]) {
            myfile << i << " " << targetLabel << "\n";
            cout << i << " " << targetLabel << "\n";
        } else {
            myfile << i << " " << nonTargetLabel << "\n";
            cout << i << " " << nonTargetLabel << "\n";
        }
        
    }
    myfile.close();
}

void Graph::setLabels(vector<bool> labels, float percentageTargets) {
    this->labels = labels;
    this->percentageTargets = percentageTargets;
}

int Graph::getNumberOfVertices() {
    return this->n;
}

int Graph::getNumberOfEdges() {
    return this->m;
}

int Graph::getNumberOfTargets() {
    return this->numberOfTargets;
}

int Graph::getNumberOfNonTargets() {
    return this->numberOfNonTargets;
}

vector<int>* Graph::getNonTargets() {
    return &this->nonTargets;
}

//********** Function only for the influenced graph ********
void Graph::generateRandomRRSetsFromTargets(int R, vector<int> activatedSet) {
    visitMark = vector<int>(n);
    associatedSet=vector<vector<set<int>>>();
    for(int i=0;i<n;i++){
        //NodeinRRsetsWithCounts.push_back(0);
        associatedSet.push_back(vector<set<int>>(n));
    }
    long totalSize = 0;
    clock_t begin = clock();
    this->rrSets =vector<vector<int>>();
     while(rrSets.size()<R) {
        rrSets.push_back(vector<int>());
    }
    
    //to do... random RR sets from activated nodes in Influenced graph
    int t=(int)activatedSet.size();
    for(int i=0;i<R;i++) {
        int randomVertex;
        randomVertex = activatedSet[rand() % t];
        generateRandomRRSetwithCount(randomVertex, i);
        totalSize+=rrSets[i].size();

    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    /*cout <<"\n Generated " << R << " RR sets\n";
    cout << "Elapsed time " << elapsed_secs;
    cout<< " \n Time per RR Set is " << elapsed_secs/R;
    cout<< "\n Total Size is " << totalSize;
    cout<<"\n Average size is " << (float)totalSize/(float)R;*/
}

//********** Function only for the influenced graph ********
void Graph::generateRandomRRSetwithCount(int randomVertex, int rrSetID) {
    q.clear();
    vector<set<int>> oneAS=vector<set<int>>(n);
    rrSets[rrSetID].push_back(randomVertex);
    
    q.push_back(randomVertex);
    int nVisitMark = 0;
    visitMark[nVisitMark++] = randomVertex;
    visited[randomVertex] = true;
    while(!q.empty()) {
        int expand=q.front();
        oneAS[expand].insert(expand);
        //associatedSet[expand][expand].insert(rrSetID);

        q.pop_front();
        for(int j=0; j<(int)graphTranspose[expand].size(); j++){
            int v=graphTranspose[expand][j];
            if(!this->flipCoinOnEdge(v, expand))
                continue;
            if(visited[v])
                continue;
            
            if(!labels[v])
            continue;
            
            if(!visited[v])
            {
                visitMark[nVisitMark++]=v;
                visited[v]=true;
            }
            q.push_back(v);
            rrSets[rrSetID].push_back(v);
           
            /*std::sort(associatedSet[v][expand].begin(), associatedSet[v][expand].end());
            std::sort(v2.begin(), v2.end());
            std::vector<int> v_intersection;
            std::set_intersection(v1.begin(), v1.end(),v2.begin(), v2.end(),std::back_inserter(v_intersection));
            */
            oneAS[v].insert(oneAS[expand].begin(),oneAS[expand].end());
            
            /*
            // get the count of the node
            int count=1;
            if(NodeinRRsetsWithCounts[v]!=0){
                count=NodeinRRsetsWithCounts[v];
                count+=1;
            }
            NodeinRRsetsWithCounts[v]=count;*/
        }
    }

    for(int v=0;v<oneAS.size();v++){
        for(int i: oneAS[v]){
            associatedSet[i][v].insert(rrSetID);
        }
    }
    
    for(int i=0;i<nVisitMark;i++) {
        visited[visitMark[i]] = false;
    }
}


void Graph::generateRandomRRSets(int R, bool label) {
    this->rrSets = vector<vector<int>>();
    long totalSize = 0;
    clock_t begin = clock();
    while(rrSets.size()<R) {
        rrSets.push_back(vector<int>());
    }
    //to do... random RR sets from activated nodes in Influenced graph
    for(int i=0;i<R;i++) {
        int randomVertex;
        randomVertex = rand() % n;
        while(!labels[randomVertex]) {
            randomVertex = rand() % n;
        }
        if(label)
            generateRandomRRSet(randomVertex, i);
        else
            generateRandomRRSetwithCount(randomVertex, i);
        totalSize+=rrSets[i].size();
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout <<"\n Generated " << R << " RR sets\n";
    cout << "Elapsed time " << elapsed_secs;
    cout<< " \n Time per RR Set is " << elapsed_secs/R;
    cout<< "\n Total Size is " << totalSize;
    cout<<"\n Average size is " << (float)totalSize/(float)R;
}

vector<vector<int>>* Graph::getRandomRRSets() {
     return &rrSets;
}

void Graph::clearRandomRRSets() {
    rrSets.clear();
}

vector<int> Graph::generateRandomRRSet(int randomVertex, int rrSetID) {
    q.clear();
    rrSets[rrSetID].push_back(randomVertex);
    q.push_back(randomVertex);
    int nVisitMark = 0;
    visitMark[nVisitMark++] = randomVertex;
    visited[randomVertex] = true;
    while(!q.empty()) {
        int expand=q.front();
        q.pop_front();
        for(int j=0; j<(int)graphTranspose[expand].size(); j++){
            int v=graphTranspose[expand][j];
            if(!this->flipCoinOnEdge(v, expand))
                continue;
            if(visited[v])
                continue;
            if(!visited[v])
            {
                visitMark[nVisitMark++]=v;
                visited[v]=true;
            }
            q.push_back(v);
            rrSets[rrSetID].push_back(v);
        }
    }
    for(int i=0;i<nVisitMark;i++) {
        visited[visitMark[i]] = false;
    }
    return rrSets[rrSetID];
}

vector<vector<int>> Graph::constructTranspose(vector<vector<int>> someGraph) {
    vector<vector<int>> transposedGraph = vector<vector<int>>();
    for(int i=0;i<someGraph.size();i++) {
        transposedGraph.push_back(vector<int>());
    }
    for(int i=0; i<someGraph.size();i++) {
        for(int v:someGraph[i]) {
            transposedGraph[v].push_back(i);
        }
    }
    return transposedGraph;
}

void Graph::assertTransposeIsCorrect() {
    assert(graph.size()==n);
    int edges = 0;
    
    for (int i=0; i< n; i++) {
        edges+=graph[i].size();
    }
    assert(edges==m);
    int edgeCount = 0;
    int reverseEdgeCount = 0;
    for (int i=0; i< n; i++) {
        int u = i;
        for (int j=0; j< graph[u].size(); j++) {
            edgeCount++;
            int v = graph[u][j];
            bool reverseEdgePresent = false;
            vector<int> reverseEdges = graphTranspose[v];
            for(int uPrime:reverseEdges) {
                if(uPrime==u) {
                    reverseEdgeCount++;
                    reverseEdgePresent = true;
                }
            }
            assert(reverseEdgePresent);
        }
        
    }
    assert(edgeCount==reverseEdgeCount);
    assert(edgeCount==m);
    
}


vector<int> Graph::oldRRSetGeneration(int randomVertex, int rrSetID) {
    //Most of this code is used from the source code of TIM - Influence Maximization: Near-Optimal Time Complexity Meets Practical Efficiency by Tang et al.
    // Source code - https://sourceforge.net/projects/timplus/
    // License GNU GENERAL PUBLIC LICENSE Version 3

    int n_visit_edge=0;
    int uStart = randomVertex;
    int hyperiiid = rrSetID;
    
    int n_visit_mark=0;
    q.clear();
    q.push_back(uStart);
    rrSets[hyperiiid].push_back(uStart);
    visitMark[n_visit_mark++]=uStart;
    visited[uStart]=true;
    while(!q.empty()) {
        int expand=q.front();
        q.pop_front();
        int i=expand;
        for(int j=0; j<(int)graphTranspose[i].size(); j++){
            int v=graphTranspose[i][j];
            n_visit_edge++;
            int randDouble = rand() % (int)(inDegree[i]);
            //     continue;
            if(randDouble!=0)
                continue;
            if(visited[v])
                continue;
            if(!visited[v])
            {
                assert(n_visit_mark<n);
                visitMark[n_visit_mark++]=v;
                visited[v]=true;
            }
            q.push_back(v);
            assert((int)rrSets.size() > hyperiiid);
            rrSets[hyperiiid].push_back(v);
        }
        
    }
    for(int i=0; i<n_visit_mark; i++)
        visited[visitMark[i]]=false;
    return rrSets[hyperiiid];
}

void Graph:: removeOutgoingEdges(int vertex){
    vector<int> outgoingNodes=vector<int>();
    outgoingNodes=graph[vertex];
    
    for(int i:outgoingNodes){
        vector<int> outgoingEdges=vector<int>();
        for(int j:graphTranspose[i]){
            if(j!=vertex)
                outgoingEdges.push_back(j);
        }
         graphTranspose[i]=outgoingEdges;
    }
    graph[vertex]=vector<int>();
    
    
    vector<int> incomingNodes=vector<int>();
    incomingNodes=graphTranspose[vertex];
    
    for(int i:incomingNodes){
        vector<int> outgoingEdges=vector<int>();
        vector<int> incomingEdges=vector<int>();
        for(int j:graph[i]){
            if(j!=vertex)
                outgoingEdges.push_back(j);
        }
        graph[i]=outgoingEdges;
        
        for(int j:graphTranspose[i]){
            if(j!=vertex)
                incomingEdges.push_back(j);
        }
        graphTranspose[i]=incomingEdges;
    }
    graphTranspose[vertex]=vector<int>();
}

void Graph:: removeNodeFromRRset(int vertex){
    /*vector<vector<set<int>>> newassociatedSet= vector<vector<set<int>>>(associatedSet);
    for(int i=0;i<n;i++){
        set<int> RRid3 = associatedSet[vertex][i];
        if(RRid3.size()>0){
            for(int j=0;j<n;j++){
                set<int> RRid4=associatedSet[j][i];
                if(RRid4.size()>0){
                    for(int r:RRid3){
                        if(RRid4.count(r)==1){
                            RRid4.erase(r);
                            //coverage[j]--;
                        }
                    }
                    associatedSet[j][i]=RRid4;
                }
            }
            associatedSet[vertex][i].clear();
        }
    }
    //coverage[vertex]=0;
    */
    
    for(int i=0;i<n;i++){
        set<int> RRid1 = associatedSet[vertex][i];
        if(RRid1.size()>0){
            for(int r:RRid1){
                for(int j:rrSets[r]){
                set<int> RRid2=associatedSet[j][i];
                    if(RRid2.count(r)==1){
                        RRid2.erase(r);
                        coverage[j]--;
                    }
                   associatedSet[j][i]=RRid2;
                }
            }
        }
        associatedSet[vertex][i].clear();
    }
    coverage[vertex]=0;
    /*for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            if(associatedSet[j][i]!=newassociatedSet[j][i])
                cout<<"false"<<associatedSet[j][i].size()<<" "<<newassociatedSet[j][i].size();
        }
    }*/
}


    
        
    
    
