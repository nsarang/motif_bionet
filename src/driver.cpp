//============================================================================
// Name        : driver.cpp
// Author      : Nima Sarang
// Version     :
// Copyright   :
// Description :
//============================================================================


#include <iostream>
#include <set>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cassert>
#include "Graph.h"
#include "MotifDiscovery.h"
#include "Stopwatch.cpp"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;



int main(int argc, const char * argv[])
{
    assert(argc == 5);

    string inPath = argv[1],
           outPath = argv[2];

    int mu = stoi(argv[3]),
        alpha = stoi(argv[4]);



    fstream inpFile(argv[1]);

    Stopwatch clock;
    Graph G(false);

    Vertex u, v;
    while (inpFile >> u >> v)
        G.addEdge(Edge(u, v));

    cout << "Main graph constructed...\n\n";


    vector<set<Graph>> vecBPSf1(4), vecBPS(4), CS, NS, outVec;

    getAllSubgraphs_Isomorphic_to_BasicPatterns(G, vecBPSf1);
    cout << "BPSf1 created..\n";
    for (int i = 0; i < 4; ++i)
        cout << "M" + to_string(i + 1) + ":\t" << vecBPSf1[i].size() << endl;


    extract_maxDisjointSubgraphs_PerBasicPattern(G, vecBPSf1, vecBPS);
    cout << "\n\nBPS created..\n";
    for (int i = 0; i < 4; ++i)
        cout << "M" + to_string(i + 1) + ":\t" << vecBPS[i].size() << endl;

    cout << "\nBasic patterns extracted...\n\n\n";




    vecBPSf1.clear();
    CS = vecBPS;

    int itCount = 1;

    while (!CS.empty())
    {
        for (const auto& patternSet : CS)
        {
            for (const auto& basicPtrnSet : vecBPS)
            {
                if (*patternSet.begin() == *basicPtrnSet.begin())
                    continue; // continue if the patters are the same

                for (const Graph& pattern1Subgraph : patternSet)
                    for (const Graph& pattern2Subgraph : basicPtrnSet)
                    {
                        if (pattern1Subgraph.edgeIntersection(pattern2Subgraph, true))
                        {
                            Graph newPattern = pattern1Subgraph;
                            newPattern.join(pattern2Subgraph);

                            if (newPattern.order() > mu)
                                continue;

                            bool bAddPattern = true;

                            if (newPattern.order() == mu)
                            {
                                for (auto& outPtrnSet : outVec)
                                    if (newPattern.isIsomorphic(const_cast <Graph&> (*outPtrnSet.begin())))
                                    {
                                        // outPtrnSet.insert(newPattern); NEED TO DOUBLE CHECK FOR INTERSECTIONS
                                        bAddPattern = false;
                                        break;
                                    }
                            }

                            if (!bAddPattern)
                                continue;

                            for (auto& newPtrnSet : NS)
                                if (newPattern.isIsomorphic(const_cast <Graph&> (*newPtrnSet.begin())))
                                {
                                    newPtrnSet.insert(newPattern);
                                    bAddPattern = false;
                                    break;
                                }

                            if (bAddPattern)
                                NS.push_back(set<Graph>({newPattern}));
                        }
                    }
            }
        }

        CS.clear();
        extractMaxDisjointSubgraphsPerPattern(NS, CS);
        NS.clear();

        for (int i = 0; i < CS.size(); ++i)
        {
            if (CS[i].size() < alpha)
            {
                CS.erase(CS.begin() + i);
                i--;
            }
            else if (CS[i].begin()->order() == mu)
            {
                outVec.push_back(CS[i]);
            }
        }

        cout << "Iteration " + to_string(itCount++) + "\t\tCS size: " << CS.size() << "  \toutVec size: " << outVec.size() << "\n";
    }


    int maxSize = 0,
        idx = 0;
    for (int i = 0; i < outVec.size(); ++i)
        if (outVec[i].size() > maxSize)
        {
            maxSize = outVec[i].size();
            idx = i;
        }

    cout.precision(5);
    cout << "\n\nFinished...\nTotal running time: " << clock.elapsedTime() << " seconds.\n";
    cout << "Max motif frequency: " << maxSize << "\n";


    if (maxSize == 0)
    {
        cout << "\nNothing found.\n";
        return 0;
    }


    fstream outFile(outPath, fstream::out),
            out_jsonFile(outPath + ".json", fstream::out);

    cout << "\nWriting to TXT file...\n";

    auto& patternSet = outVec[idx];

    outFile << "∫New pattern\n"
            << "Frequency: " << patternSet.size() << "\n\n";
    for (auto pattern : patternSet)
    {
        outFile << pattern;
        outFile << "\n\n\n";
    }



    cout << "Writing to JSON file...\n\n";

    unordered_map<Vertex, int> colVertex;
    unordered_map<Vertex, int> colEdge;
    json jsonFormat;

    int colorNO = 2; // "1" is reserved for the main graph

    for (auto& pattern : patternSet)
    {
        for (auto& v : pattern.nodeVec)
        {
            colVertex[v] = colorNO;
            for (auto & u : pattern.adjList.at(v))
                colEdge[v + "#" + u] = colorNO;
        }
        colorNO++;
    }

    for (auto& v : G.nodeVec)
    {
        if (!colVertex[v])
            colVertex[v] = 1;

        jsonFormat["nodes"].push_back({{"id", v}, {"color", colVertex[v]}});

        for (auto & u : G.adjList[v])
        {
            if (!colEdge[v + "#" + u])
                colEdge[v + "#" + u] = 1;

            jsonFormat["links"].push_back({{"source", v}, {"target", u}, {"value", colEdge[v + "#" + u]}});
        }
    }

    jsonFormat["specs"] = {
        {"graphOrder", G.order()}, {"graphSize", G.size()}, {"graphComps", G.components()},
        {"patternOrder", patternSet.begin()->order()}, {"patternSize", patternSet.begin()->size()},
        {"patternFreq", patternSet.size()}
    };

    out_jsonFile << jsonFormat.dump(4);

    cout << "Done.\n\n";
}
