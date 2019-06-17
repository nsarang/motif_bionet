//============================================================================
// Name        : Graph.cpp
// Author      : Nima Sarang
// Version     :
// Copyright   :
// Description :
//============================================================================


#ifndef __GRAPH_CPP
#define __GRAPH_CPP

#include "Graph.h"
#include <iostream>
#include <cmath>

#define INF 1e8


ostream &operator<<(ostream &out, const Graph &G)
{
    out << "--------------\n";
    out << "nodeCount: " << G.nodeVec.size() << endl;
    out << "edgeCount: " << G.edgeCount << endl;

    for (auto V : G.nodeVec)
        for (auto U : G.adjList.at(V))
            out << V << "\t" << U << "\n";
    out << "--------------\n\n";
    return out;
}


Graph::Graph(bool sortbyDegree) : edgeCount( 0 ), sortbyDeg( sortbyDegree ), chLables( false )
{
}


Graph::Graph(vector<Vertex> inpNodes, vector<Edge> inpEdges, bool sortbyDegree)
    : nodeVec( inpNodes ), sortbyDeg( sortbyDegree ), chLables( true )
{
    for (Edge e : inpEdges)
    {
        adjList[e.u].insert(e.v);
        adjList[e.v].insert(e.u);
    }

    edgeCount = inpEdges.size();
    sortNodeVec();
}


bool Graph::existEdge(Edge e) const
{
    try {
        return adjList.at(e.v).count(e.u);
    }
    catch (const std::out_of_range& exp) {
        return false;
    }
}


void Graph::addEdge(Edge e)
{
    addNode(e.u);
    addNode(e.v);

    if (adjList[e.v].count(e.u))
        return; // edge already exists

    adjList[e.v].insert(e.u);
    adjList[e.u].insert(e.v);

    edgeCount++;

    if (sortbyDeg)
        sortNodeVec();
    chLables = true;
}


bool Graph::existNode(Vertex v) const
{
    return adjList.find(v) != adjList.end();
}


void Graph::addNode(Vertex v)
{
    if (!existNode(v))
    {
        nodeVec.push_back(v);
        adjList[v];
    }
    chLables = true;
}


void Graph::deleteNode(Vertex v)
{
    nodeVec.erase(remove(nodeVec.begin(), nodeVec.end(), v));
    edgeCount -= adjList[v].size();
    
    for (Vertex u : adjList[v])
        adjList[u].erase(v);
    
    adjList.erase(v);

    if (sortbyDeg)
        sortNodeVec();
    chLables = true;
}


int Graph::deg(Vertex v) const
{
    return adjList.at(v).size();
}


int Graph::components() const
{
    unordered_map<Vertex, bool> nodeMark;
    function<void(Vertex)> DFSUtil = [&](const Vertex& v)
    {
        nodeMark[v] = true;
        for (auto & u: adjList.at(v))
            if (!nodeMark[u])
                DFSUtil(u);
    };
    
    int cnt = 0;
    for (auto & v : nodeVec)
        if (!nodeMark[v])
            DFSUtil(v), cnt++;
    
    return cnt;
}

bool Graph::edgeIntersection(const Graph &P, bool explicitPartly) const
{
    bool fullyIntersects = true,
         partlyIntersects = false;

    for (const Vertex& v : P.nodeVec)
    {
        if (!existNode(v))
        {
            fullyIntersects = false;
            continue;
        }

        for (const Vertex& u : P.adjList.at(v))
        {
            if (adjList.at(v).count(u))
                partlyIntersects = true;
            else
                fullyIntersects = false;
        }

        if (!explicitPartly & partlyIntersects)
            break;
    }

    return partlyIntersects & (!fullyIntersects | !explicitPartly);
}


bool Graph::nodeIntersection(const Graph &P) const
{
    bool nodeIntersects = false;
    
    for (const Vertex& v : P.nodeVec)
        if (existNode(v))
        {
            nodeIntersects = true;
            break;
        }
    
    return nodeIntersects;
}


void Graph::join(const Graph &P)
{
    int cntNewEdges = 0;

    for (const Vertex& v : P.nodeVec)
    {
        addNode(v);
        for (const Vertex& u : P.adjList.at(v))
        {
            auto ret = adjList[v].insert(u);
            if (ret.second == true)
                cntNewEdges++;
        }
    }

    edgeCount += cntNewEdges / 2;

    if (sortbyDeg)
        sortNodeVec();
    chLables = true;
}


void Graph::sortNodeVec()
{
    sort(nodeVec.begin(), nodeVec.end(), [&](const Vertex & a, const Vertex & b) -> bool
    {
        int d1 = adjList[a].size(), d2 = adjList[b].size();
        return  d1 != d2 ? d1 > d2 : a < b;
    });
}


bool Graph::operator<(const Graph& P) const
{
    if (order() != P.order())
        return order() < P.order();

    if (size() != P.size())
        return size() < P.size();

    for (int i = 0; i < order(); ++i)
    {
        auto &list1 = adjList.at(nodeVec[i]),
              &list2 = P.adjList.at(P.nodeVec[i]);
        if (list1 != list2)
            return list1 < list2;
    }
    return false;
}


bool Graph::operator==(const Graph& P) const
{
    if (order() != P.order() || size() != P.size() || nodeVec != P.nodeVec)
        return false;


    for (int i = 0; i < order(); ++i)
    {
        auto &list1 = adjList.at(nodeVec[i]),
              &list2 = P.adjList.at(P.nodeVec[i]);
        if (list1 != list2)
            return false;
    }
    return true;
}


bool Graph::isIsomorphic(Graph &P)
{
    if (order() != P.order() || size() != P.size())
        return false;

    if (!sortbyDeg)
        sortNodeVec();

    if (!P.sortbyDeg)
        P.sortNodeVec();
    
    for (int i = 0; i < order(); ++i)
        if (deg(nodeVec[i]) != P.deg(P.nodeVec[i]))
            return false;

    if (chLables)
        calcLables();

    if (P.chLables)
        P.calcLables();

    if (canoLabels != P.canoLabels)
        return false;

    return true;
}


void Graph::calcLables()
{
    int n = order();
    vector<vector<int>> distMatrix(n, vector<int> (n, INF));

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
        {
            if (i == j)
                distMatrix[i][j] = 0;
            else if (existEdge(Edge(nodeVec[i], nodeVec[j])))
                distMatrix[i][j] = 1;
        }

    int diameter = 1;

    for (int k = 0; k < n; ++k)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                if (distMatrix[i][j] > distMatrix[i][k] + distMatrix[k][j])
                {
                    distMatrix[i][j] = distMatrix[i][k] + distMatrix[k][j];
                    diameter = max(diameter, distMatrix[i][j]);
                }

    canoLabels.clear();
    unordered_map<Vertex, int> nodeToIndex;

    for (int i = 0; i < n; ++i)
        nodeToIndex[nodeVec[i]] = i;

    for (Vertex& v : nodeVec)
    {
        double label = 0;
        for (const auto& u : nodeVec)
        {
            label += pow(2, diameter - distMatrix[nodeToIndex[v]][nodeToIndex[u]] - deg(u));
        }
        canoLabels.push_back(label);
    }

    sort(canoLabels.begin(), canoLabels.end());
    chLables = false;
}


#endif /* __GRAPH_CPP */
