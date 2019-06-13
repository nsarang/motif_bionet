//============================================================================
// Name        : Graph.h
// Author      : Nima Sarang
// Version     :
// Copyright   : Copyright © 2017 Nima Sarang. All rights reserved.
// Description :
//============================================================================

#ifndef __GRAPH_H
#define __GRAPH_H

#include <vector>
#include <set>
#include <unordered_map>
#include <string>
using namespace std;

typedef string Vertex;

struct Edge
{
    Edge(Vertex a, Vertex b) : u( a ), v( b ) {}
    Vertex u, v;
};


class Graph
{
public:
    Graph(bool sortbyDegree = true);
    
    Graph(vector<Vertex> inpNodes, vector<Edge> inpEdges, bool sortbyDegree = true);
    
    bool existEdge(Edge e) const;
    
    void addEdge(Edge e);
    
    void addNode(Vertex v);

    bool existNode(Vertex v) const;
    
    void deleteNode(Vertex v);
    
    int deg(Vertex v) const;
    
    void join(const Graph& P);
    
    bool edgeIntersection(const Graph& P, bool explicitPartly = false) const;
    
    bool nodeIntersection(const Graph& P) const;
    
    bool operator<(const Graph& P) const;
    
    bool operator==(const Graph& P) const;
    
    friend ostream &operator<<(ostream &out, const Graph &G);

    bool isIsomorphic(Graph &P);
    
    int order() const {return nodeVec.size();}
    
    int size() const {return edgeCount;}
    
    int components() const;
    
    void sortNodeVec();

    void calcLables();
    
    vector<Vertex> nodeVec;
    unordered_map<Vertex, set<Vertex>> adjList;
    bool sortbyDeg;
    bool chLables;
    int edgeCount;
    vector<double> canoLabels;
};

#endif /* __GRAPH_H */
