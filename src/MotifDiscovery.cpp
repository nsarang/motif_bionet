//============================================================================
// Name        : MotifDiscovery.cpp
// Author      : Nima Sarang
// Version     :
// Copyright   : Copyright © 2017 Nima Sarang. All rights reserved.
// Description :
//============================================================================


#ifndef __MOTIFDISCOVERY_CPP
#define __MOTIFDISCOVERY_CPP

#include "MotifDiscovery.h"


template<class T1, class T2>
int intersectionSize(set<T1> list1, T2 list2)
{
    int cnt = 0;
    for (auto& elem : list2)
        if (list1.count(elem))
            cnt++;
    return cnt;
}



void getAllSubgraphs_Isomorphic_to_BasicPatterns(Graph &G, vector<set<Graph>> &vecBPSf1)
{
    // M1
    for (const Vertex& v : G.nodeVec)
    {
        auto &list = G.adjList[v];
        
        for (auto itA = list.begin(), itB = itA++; itA != list.end(); ++itA, ++itB)
        {
            Graph pattern1({v, *itA, *itB},
                           {Edge(v, *itA), Edge(v, *itB)});
            
            vecBPSf1[0].insert(pattern1);
        }
    }
    
    
    // M2
    for (const Graph& pattern1 : vecBPSf1[0])
    {
        Edge e = {pattern1.nodeVec[1], pattern1.nodeVec[2]};
        
        if (G.existEdge(e))
        {
            Graph pattern2 = pattern1;
            pattern2.addEdge(e);
            vecBPSf1[1].insert(pattern2);
        }
    }
    
    
    // M3
    for (const Graph& pattern1 : vecBPSf1[0])
    {
        Vertex u = pattern1.nodeVec[0];
        for (const Vertex& v : G.adjList[u])
        {
            Edge e = {u, v};
            if (!pattern1.existEdge(e))
            {
                Graph pattern3 = pattern1;
                pattern3.addEdge(e);
                vecBPSf1[2].insert(pattern3);
            }
        }
    }
    
    
    // M4
    for (const Graph& pattern1 : vecBPSf1[0])
    {
        Vertex u = pattern1.nodeVec[1];
        for (const Vertex& v : G.adjList[u])
        {
            if (v != pattern1.nodeVec[0] && v != pattern1.nodeVec[2])
            {
                Graph pattern4 = pattern1;
                pattern4.addEdge({u, v});
                vecBPSf1[3].insert(pattern4);
                
            }
        }
        
        u = pattern1.nodeVec[2];
        for (const Vertex& v : G.adjList[u])
        {
            if (v != pattern1.nodeVec[0] && v != pattern1.nodeVec[1])
            {
                Graph pattern4 = pattern1;
                pattern4.addEdge({u, v});
                vecBPSf1[3].insert(pattern4);
            }
        }
    }
}




void extract_maxDisjointSubgraphs_PerBasicPattern(Graph &G, vector<set<Graph>> &vecBPSf1, vector<set<Graph>> &vecBPS)
{
    vector<pair<int, set<Graph>::iterator>> vecDegree;
    vecDegree.reserve(G.size() / 2);
    int degree;
    
    
    auto extractMISfromVecDegree =
    [&](vector<pair<int, set<Graph>::iterator>> &vecDegree, set<Graph> &setBPSf1, set<Graph> &setBPS) -> void
    {
        sort(vecDegree.begin(), vecDegree.end(), [&](const pair<int, set<Graph>::iterator>& a, const pair<int, set<Graph>::iterator>& b)
             -> bool {return a.first < b.first;});
        
        Graph tmpGraph;
        for (auto& elem : vecDegree)
        {
            auto& currGraph = *elem.second;
            if (!tmpGraph.edgeIntersection(currGraph))
            {
                setBPS.insert(currGraph);
                tmpGraph.join(currGraph);
            }
        }
    };
    
    
    // M1
    for (auto itG = vecBPSf1[0].begin(); itG != vecBPSf1[0].end(); ++itG)
    {
        auto& pattern1 = *itG;
        degree = 2 * G.deg(pattern1.nodeVec[0]) + G.deg(pattern1.nodeVec[1]) + G.deg(pattern1.nodeVec[2]) - 6;
        vecDegree.push_back(make_pair(degree, itG));
    }
    
    extractMISfromVecDegree(vecDegree, vecBPSf1[0], vecBPS[0]);
    vecDegree.clear();
    
    
    // M2
    for (auto itG = vecBPSf1[1].begin(); itG != vecBPSf1[1].end(); ++itG)
    {
        auto& pattern2 = *itG;
        degree = 0;
        
        for (int j = 0; j <= 1; ++j)
            for (int l = j + 1; l <= 2; ++l)
                degree += intersectionSize(G.adjList[pattern2.nodeVec[j]], G.adjList[pattern2.nodeVec[l]]) - 1;
        
        vecDegree.push_back(make_pair(degree, itG));
    }
    
    extractMISfromVecDegree(vecDegree, vecBPSf1[1], vecBPS[1]);
    vecDegree.clear();
    
    
    // M3
    for (auto itG = vecBPSf1[2].begin(); itG != vecBPSf1[2].end(); ++itG)
    {
        auto& pattern3 = *itG;
        int d = G.deg(pattern3.nodeVec[0]);
        degree = (d - 2) * (d - 3);
        
        for (int j = 1; j <= 3; ++j)
        {
            d = G.deg(pattern3.nodeVec[j]);
            degree += (d - 1) * (d - 2);
        }
        
        degree = (degree * 3) / 2;
        vecDegree.push_back(make_pair(degree, itG));
    }
    
    extractMISfromVecDegree(vecDegree, vecBPSf1[2], vecBPS[2]);
    vecDegree.clear();
    
    // M4
    for (auto itG = vecBPSf1[3].begin(); itG != vecBPSf1[3].end(); ++itG)
    {
        auto nodeVec = itG->nodeVec;
        degree = (G.deg(nodeVec[0]) - 1) * (G.deg(nodeVec[1]) - 1) - intersectionSize(G.adjList[nodeVec[0]], G.adjList[nodeVec[1]]) - 1 + // extend from middle edge
        G.deg(nodeVec[2]) - intersectionSize(G.adjList[nodeVec[2]], nodeVec) + // extend from leaf
        G.deg(nodeVec[3]) - intersectionSize(G.adjList[nodeVec[3]], nodeVec);
        
        vecDegree.push_back(make_pair(degree, itG));
    }
    
    extractMISfromVecDegree(vecDegree, vecBPSf1[3], vecBPS[3]);
    vecDegree.clear();
}



void extractMaxDisjointSubgraphsPerPattern(vector<set<Graph>> &NS, vector<set<Graph>> &CS)
{
    for (const auto& patternSet : NS)
    {
        vector<set<Graph>::iterator> iterVec;
        iterVec.reserve(patternSet.size());
        
        Graph overlapGraph;
        set<Graph> outSet;
        
        int i = 0, j;
        for (auto currGraph = patternSet.begin(); currGraph != patternSet.end(); ++currGraph, ++i)
        {
            iterVec.push_back(currGraph);
            overlapGraph.addNode(to_string(i));
            j = i + 1;
            for (auto nextGraph = next(currGraph); nextGraph != patternSet.end(); ++nextGraph, ++j)
                if (currGraph->edgeIntersection(*nextGraph))
                    overlapGraph.addEdge(Edge(to_string(i), to_string(j)));
        }
        
        while (overlapGraph.order() > 0)
        {
            Vertex v = overlapGraph.nodeVec.back();
            outSet.insert(*iterVec[stoi(v)]);
            
            auto list = overlapGraph.adjList[v];
            for (Vertex u : list)
                overlapGraph.deleteNode(u);
            overlapGraph.deleteNode(v);
        }
        
        CS.push_back(outSet);
    }
}


#endif /* __MOTIFDISCOVERY_CPP */
