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
void Graph::readGraph(string fileName,std::ofstream& resultLogFile) {
    readGraph(fileName, 0.8,resultLogFile);
}
bool sortbydegree(const set<int> &a,const set<int> &b)
{
    return (a.size() > b.size());
}


Graph::Graph() {
    this->standardProbability = false;
}
/*
AS::AS(int value, AS* n)
{
    data=value;
    count=0;
}
 */
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

void Graph::readReverseGraph(string fileName, float percentage){
    this->graphName = fileName;
    this->percentageTargets = percentage;
    cout << "\n Reading Reverse Influence graph: ";
    
    ifstream myFile("graphs/" + fileName);
    string s;
    if(myFile.is_open()) {
        myFile >> n >> m;
        graph=vector<vector<int>>(n,vector<int>());
        visited=vector<bool>(n,false);
        labels=vector<bool>(n,true);
        inDegree=vector<int>(n,0);

        int from, to;
        int maxDegree = 0;
        while (myFile >> from >> to) {
            graph[to].push_back(from);
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
}

//********** Function for generating half graph ********
void Graph::readHalfGraph(string fileName, float percentage, int graphCutValue, std::ofstream& resultLogFile){
    this->graphName = fileName;
    this->percentageTargets = percentage;
    cout << "\n Generating graph: "<<100-graphCutValue<<"%"<<flush;
    resultLogFile<< "\n Generating graph: "<<100-graphCutValue<<"%";
    
    ifstream myFile("graphs/" + fileName);
    
    vector<vector<int>> oldGraph;
    
    
    string s;
    if(myFile.is_open()) {
        myFile >> n >> m;
        graph=vector<vector<int>>(n,vector<int>());
        oldGraph=vector<vector<int>>(n,vector<int>());
        visited=vector<bool>(n,false);
        labels=vector<bool>(n,false);
        inDegree=vector<int>(n,0);
        
        int from, to;
        int maxDegree = 0;
        while (myFile >> from >> to) {
            oldGraph[from].push_back(to);
        }
        
        int X=n-(n*(float)graphCutValue/100);
        for(int i=0;i<X;i++){
            int randomNum=rand()%n;
            labels[randomNum]=true;
            graph[randomNum]=oldGraph[randomNum];
            for(int j:graph[randomNum]){
                inDegree[j] = inDegree[j]+1;
                if(inDegree[j] > maxDegree) {
                    maxDegree = inDegree[j];
                }
                labels[j]=true;
            }
        }
        vector<vector<int>>().swap(oldGraph);
        myFile.close();
    }
    
    graphTranspose = constructTranspose(graph);
    visitMark = vector<int>(n);
    this->numberOfTargets = n;
    this->numberOfNonTargets = (int)nonTargets.size();
}

//********** Function for generating half graph ********
void Graph::readInfluencedHalfGraph(string fileName, float percentage, string convertedFile,int graphCutValue,std::ofstream& resultLogFile){
    this->graphName = fileName;
    this->percentageTargets = percentage;
    cout << "\n Generating graph: "<<100-graphCutValue<<"%"<<flush;
    ifstream myFile(convertedFile);
    vector<vector<int>> oldGraph;
    
    string s;
    if(myFile.is_open()) {
        myFile>>n;
        //modified
        //myFile>>n>>m;
        //
        graph=vector<vector<int>>(n,vector<int>());
        oldGraph=vector<vector<int>>(n,vector<int>());
        visited=vector<bool>(n,false);
        labels=vector<bool>(n,false);
        //modified
        //labels=vector<bool>(n,true);
        //
        inDegree=vector<int>(n,0);
        
        int from, to;
        int maxDegree = 0;
        while (myFile >> from >> to) {
            if(from==-1 && to==-1){
                break;
            }
            oldGraph[from].push_back(to);
        }
        
        int X=n-(n*(float)graphCutValue/100);
        for(int i=0;i<X;i++){
            int randomNum=rand()%n;
            labels[randomNum]=true;
            graph[randomNum]=oldGraph[randomNum];
            for(int j:graph[randomNum]){
                inDegree[j] = inDegree[j]+1;
                if(inDegree[j] > maxDegree) {
                    maxDegree = inDegree[j];
                }
                labels[j]=true;
            }
        }
        vector<vector<int>>().swap(oldGraph);
        myFile.close();
    }
    
    graphTranspose = constructTranspose(graph);
    visitMark = vector<int>(n);
    this->numberOfTargets = n;
    this->numberOfNonTargets = (int)nonTargets.size();
}


vector<int> Graph::writeInfluencedGraph(string fileName, float percentage, string convertedFile, vector<int> *seedNodes,vector<int> *seedOrder) {
    this->graphName = fileName;
    this->percentageTargets = percentage;
    //cout << "\n Reading Reverse targets graph: ";
    unordered_set<int> activatedSet;
    ifstream myFile(convertedFile);
    string s;
    if(myFile.is_open()) {
        myFile >>n;
        graph=vector<vector<int>>(n,vector<int>());
        visited=vector<bool>(n,false);
        labels=vector<bool>(n,true);
        inDegree=vector<int>(n,0);
        
        int from, to;
        int maxDegree = 0;

        while (myFile >> from >> to) {
            if(from ==-1 && to==-1){
                break;
            }
                graph[from].push_back(to);
                inDegree[to] = inDegree[to]+1;
                if(inDegree[to] > maxDegree) {
                    maxDegree = inDegree[to];
                }
                activatedSet.insert(from);
                activatedSet.insert(to);
        }
        if(seedNodes!=NULL && seedOrder!=NULL){
            int str;
            int i=0;
            while (myFile>>str) {
                if(str==-1)
                    break;
                (*seedNodes)[i]=str;
                i++;
            }
            //just to skip one more -1
            myFile>>str;
            i=0;
            while (myFile>>str) {
                (*seedOrder)[i]=str;
                i++;
            }
        }
        myFile.close();
    }
    graphTranspose = constructTranspose(graph);
    visitMark = vector<int>(n);
    this->numberOfTargets =(int) activatedSet.size();
    this->numberOfNonTargets =(int)nonTargets.size();
    
    vector<int> activatedVector=vector<int>(activatedSet.size());
    int k=0;
    for(int i:activatedSet){
        activatedVector[k++]=i;
    }
    return activatedVector;
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
        graph=vector<vector<int>>(n,vector<int>());
        visited=vector<bool>(n,false);
        labels=vector<bool>(n,false);
        inDegree=vector<int>(n,0);

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
    this->numberOfNonTargets =(int)nonTargets.size();
}


void Graph::readGraph(string fileName, float percentage,std::ofstream& resultLogFile) {
    this->graphName = fileName;
    this->percentageTargets = percentage;
    cout << "\n Reading all targets graph: ";
    
    ifstream myFile("graphs/" + fileName);
    string s;
    if(myFile.is_open()) {
        myFile >> n >> m;
        graph=vector<vector<int>>(n,vector<int>());
        visited=vector<bool>(n,false);
        labels=vector<bool>(n,true);
        inDegree=vector<int>(n,0);

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

void Graph::writeLabels(std::ofstream& resultLogFile) {
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
void Graph::generateRandomRRSetsFromTargets(int R, vector<int> activatedSet,string modular,std::ofstream& resultLogFile) {
    clock_t begin = clock();
    visitMark = vector<int>(n);
    //associatedSet=vector<vector<set<int>>>();
    
    /*for(int i=0;i<n;i++){
     //NodeinRRsetsWithCounts.push_back(0);
     associatedSet.push_back(vector<set<int>>(n));
     }*/
    
    long totalSize = 0;
    
    this->rrSets =vector<vector<int>>();
    while(rrSets.size()<R) {
        rrSets.push_back(vector<int>());
    }
    
    if(modular.compare("modular")==0){
        NodeinRRsetsWithCounts=vector<int>(n,0);
        if(activatedSet.size()==0){
            for(int i=0;i<R;i++) {
                int randomVertex;
                randomVertex = rand() % n;
                while(!labels[randomVertex]) {
                    randomVertex = rand() % n;
                }
                generateRandomRRSetwithCountMod(randomVertex, i);
                totalSize+=rrSets[i].size();
            }
        }
            else{
                vector<pair<int,int>> checkRR;
                int t=(int)activatedSet.size();
                for(int i=0;i<R;i++) {
                    int randomVertex;
                    randomVertex = activatedSet[rand() % t];
                    
                    generateRandomRRSetwithCountMod(randomVertex, i);
                    totalSize+=rrSets[i].size();
                }
            }
        }
        else if(modular.compare("submodular")==0){
            coverage=vector<int>(n,0);
            //AStree=vector<unordered_map<AS,vector<AS>>>(n);
            //match= new vector<AS>(n);

            pairAssociatedSet=vector<unordered_map<int,unordered_set<int>>>(n);
            nodeAS=vector<vector<int>>(n);
            int t=(int)activatedSet.size();
            for(int i=0;i<R;i++) {
                RRgraph=vector<vector<int>>(n) ;
                int randomVertex;
                randomVertex = activatedSet[rand() % t];
                generateRandomRRSetwithCount(randomVertex, i);
                totalSize+=rrSets[i].size();
            }
        }
    
        else{
            int t=(int)activatedSet.size();
            nodeAS=vector<vector<int>>(n);
            pairAssociatedSet=vector<unordered_map<int,unordered_set<int>>>(n);
            coverage=vector<int>(n,0);
            for(int i=0;i<R;i++) {
                RRgraph=vector<vector<int>>(n) ;
                outdegree=vector<int>(n);
                int randomVertex;
                randomVertex = activatedSet[rand() % t];
                generateRandomRRSetwithRRgraphs(randomVertex, i);
                totalSize+=rrSets[i].size();
            }
            
        }
        visitMark.clear();
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        cout <<"\n Generated reverse" << R << " RR sets\n";
        cout << "Elapsed time " << elapsed_secs;
        cout<< " \n Time per RR Set is " << elapsed_secs/R;
        cout<< "\n Total Size is " << totalSize<<flush;
        cout<<"\n Average size is " << (float)totalSize/(float)R;
    
    resultLogFile <<"\n Generated reverse" << R << " RR sets\n";
    resultLogFile << "Elapsed time " << elapsed_secs;
    resultLogFile<< " \n Time per RR Set is " << elapsed_secs/R;
    resultLogFile<< "\n Total Size is " << totalSize<<flush;
    resultLogFile<<"\n Average size is " << (float)totalSize/(float)R;
    }


//********** Function only for the influenced graph ********
void Graph::generateRandomRRSetwithRRgraphs(int randomVertex, int rrSetID) {
    
    q.clear();
    rrSets[rrSetID].push_back(randomVertex);
    q.push_back(randomVertex);
    int nVisitMark = 0;
    visitMark[nVisitMark++] = randomVertex;
    visited[randomVertex] = true;
    outdegree[randomVertex]=0;
    
    while(!q.empty()) {
        int expand=q.front();
        q.pop_front();
        nodeAS[expand].push_back(expand);
        for(int j=0; j<(int)graphTranspose[expand].size(); j++){
            int v=graphTranspose[expand][j];
            if(!labels[v])
                continue;
            
            if(visited[v]){
                continue;
            }
            
            if(!this->flipCoinOnEdge(v, expand))
                continue;
            
            if(!visited[v])
            {
                visitMark[nVisitMark++]=v;
                visited[v]=true;
            }
            q.push_back(v);
            rrSets[rrSetID].push_back(v);
            RRgraph[expand].push_back(v);
            outdegree[v]++;
        }
    }
    for(int i=0;i<nVisitMark;i++) {
        visited[visitMark[i]] = false;
    }
    
    BFSonRRgraphs(randomVertex,rrSetID);
}


void Graph:: BFSonRRgraphs(int randomVertex,int rrSetID){
    
    priority_queue<int, vector<int>, greater<int>> workQueue;
    workQueue.push(randomVertex);
    
    while(!workQueue.empty()) {
        int expand=workQueue.top();
        workQueue.pop();
        
        for(int v:RRgraph[expand]){
            outdegree[v]--;
            workQueue.push(v);
            pair<int,unordered_set<int>> node;
            nodeAS[v].insert(nodeAS[v].end(), nodeAS[expand].begin(),nodeAS[expand].end());
        }
    }
    pair<int,unordered_set<int>> node;
    //creation of associated set matrix
    for(int i=0;i<nodeAS.size();i++){
        if(nodeAS[i].size()>0){
            for(int j:nodeAS[i]){
                if(pairAssociatedSet[j].empty()){
                    node=pair<int,unordered_set<int>>();
                    node.first=i;
                    node.second.insert(rrSetID);
                    pairAssociatedSet[j].insert(node);
                    coverage[j]++;
                }
                
                else{
                    std::unordered_map<int, unordered_set<int>>::iterator it = pairAssociatedSet[j].find(i);
                    if (it != pairAssociatedSet[j].end()){
                        it->second.insert(rrSetID);
                        coverage[j]++;
                    }
                    else{
                        node=pair<int,unordered_set<int>>();
                        node.first=i;
                        node.second.insert(rrSetID);
                        pairAssociatedSet[j].insert(node);
                        coverage[j]++;
                    }
                }
            }
            nodeAS[i].clear();
        }
    }
}


//********** Function only for the influenced graph with modular property********
void Graph::generateRandomRRSetwithCountMod(int randomVertex, int rrSetID) {
    NodeinRRsetsWithCounts[randomVertex]++;
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
            if(!labels[v])
                continue;
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
            
            NodeinRRsetsWithCounts[v]++;
        }
    }
    for(int i=0;i<nVisitMark;i++) {
        visited[visitMark[i]] = false;
    }
}

//********** Function only for the influenced graph ********
void Graph::generateRandomRRSetwithCount(int randomVertex, int rrSetID) {

    pair<int,unordered_set<int>> node;
    
    q.clear();
    rrSets[rrSetID].push_back(randomVertex);
    q.push_back(randomVertex);
    int nVisitMark = 0;
    visitMark[nVisitMark++] = randomVertex;
    visited[randomVertex] = true;
    while(!q.empty()) {
        int expand=q.front();
        nodeAS[expand].push_back(expand);
        
        q.pop_front();
        for(int j=0; j<(int)graphTranspose[expand].size(); j++){
            int v=graphTranspose[expand][j];
            if(!labels[v])
                continue;
            
            if(visited[v]){
                //find difference here
                std::vector<int> intersect;
                std::sort(nodeAS[expand].begin(), nodeAS[expand].end());
                std::sort(nodeAS[v].begin(), nodeAS[v].end());
                 std::set_intersection(nodeAS[v].begin(), nodeAS[v].end(),nodeAS[expand].begin(), nodeAS[expand].end(),std::back_inserter(intersect));
                nodeAS[v]=intersect;
                nodeAS[v].push_back(v);
                nodeAS[expand]=intersect;
                nodeAS[expand].push_back(expand);
                deque<int> qu;
                qu.push_back(v);
                while(!qu.empty()){
                    int explore=qu.front();
                    qu.pop_front();
                    for(int i:RRgraph[explore]){
                        qu.push_back(i);
                        nodeAS[i]=intersect;
                        nodeAS[i].push_back(i);
                    }
                }
                continue;
            }
            
            if(!this->flipCoinOnEdge(v, expand))
                continue;
            
            if(!visited[v])
            {
                visitMark[nVisitMark++]=v;
                visited[v]=true;
                RRgraph[expand].push_back(v);
            }
            q.push_back(v);
            rrSets[rrSetID].push_back(v);
            
            nodeAS[v].insert(nodeAS[v].end(), nodeAS[expand].begin(),nodeAS[expand].end());

            }
    }
    for(int i=0;i<nVisitMark;i++) {
        visited[visitMark[i]] = false;
    }
    /*
    //test
    for(int i=0;i<nodeAS.size();i++){
        if(nodeAS[i].size()>0){
            for(int j:nodeAS[i]){
                //AS temp= match->at(j);
                AS *Rval;
                AS *node;
                if(AStree[i].count(rrSetID)==0){
                    Rval= new AS;
                    node= new AS;
                    //struct AS *Rval= (struct AS*) malloc(sizeof(struct AS));
                    //struct AS *node= (struct AS*) malloc(sizeof(struct AS));
                    node->next->push_back(*Rval) ;
                    node->count++;
                    Rval->next->push_back(*node);
                    (*match)[j]= (*Rval);
                    
                    pair<AS,vector<AS>> h=pair<AS,vector<AS>>();
                    h.first= *Rval;
                    h.second.push_back(*node);
                    AStree[i].insert(h);
                }
                else{
                    //temp.next->next.insert(j);
                    AS *node= new AS;
                    vector<AS> Rval=AStree[i].find(rrSetID)->second;
                }
            }
        }
    }
    */
    
    //creation of associated set matrix
    for(int i=0;i<nodeAS.size();i++){
        if(nodeAS[i].size()>0){
            for(int j:nodeAS[i]){
                if(pairAssociatedSet[j].empty()){
                    node=pair<int,unordered_set<int>>();
                    node.first=i;
                    node.second.insert(rrSetID);
                    pairAssociatedSet[j].insert(node);
                    coverage[j]++;
                }
                
                else{
                    std::unordered_map<int, unordered_set<int>>::iterator it = pairAssociatedSet[j].find(i);
                    if (it != pairAssociatedSet[j].end()){
                        it->second.insert(rrSetID);
                        coverage[j]++;
                    }
                    else{
                        node=pair<int,unordered_set<int>>();
                        node.first=i;
                        node.second.insert(rrSetID);
                        pairAssociatedSet[j].insert(node);
                        coverage[j]++;
                    }
                }
            }
            nodeAS[i].clear();
        }
    }
}


void Graph::generateRandomRRSets(int R, bool label,std::ofstream& resultLogFile) {
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
        if(label){
            generateRandomRRSet(randomVertex, i);
        }
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
    
    resultLogFile <<"\n Generated " << R << " RR sets\n";
    resultLogFile << "Elapsed time " << elapsed_secs;
    resultLogFile<< " \n Time per RR Set is " << elapsed_secs/R;
    resultLogFile<< "\n Total Size is " << totalSize;
    resultLogFile<<"\n Average size is " << (float)totalSize/(float)R;
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
            if(!labels[v])
                continue;
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
    inDegree[vertex]=0;
    labels[vertex]=false;
    vector<int> outgoingNodes=vector<int>();
    outgoingNodes=graph[vertex];
    
    for(int i:outgoingNodes){
        vector<int> outgoingEdges=vector<int>();
        vector<int> incomingEdges=vector<int>();
        for(int j:graphTranspose[i]){
            if(j!=vertex)
                outgoingEdges.push_back(j);
        }
         graphTranspose[i]=outgoingEdges;
       
        for(int j:graph[i]){
            if(j!=vertex)
                incomingEdges.push_back(j);
        }
        graph[i]=incomingEdges;
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
    for(pair<int,unordered_set<int>> RRpair : pairAssociatedSet[vertex]){
        if(RRpair.second.size()>0){
            for(int RRi:RRpair.second){
                for(int j:rrSets[RRi]){
                    std::unordered_map<int, unordered_set<int>>::iterator it = pairAssociatedSet[j].find(RRpair.first);
                    if (it != pairAssociatedSet[j].end() && it->second.count(RRi)==1){
                        it->second.erase(RRi);
                        coverage[j]--;
                    }
                }
            }
        }
    }
    pairAssociatedSet[vertex].clear();
    coverage[vertex]=0;
}



    
        
    
    
